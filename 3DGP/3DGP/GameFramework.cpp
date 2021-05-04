#include "stdafx.h"
#include "GameFramework.h"
#include "GameFrameworkHelper.h"

/////////////////////////////////////////////////////////////////////////////////////////////////

GameFramework::GameFramework() :
	mhInstance{},
	mhWnd{},
	mWndClientWidth{ FRAME_BUFFER_WIDTH },
	mWndClientHeight{ FRAME_BUFFER_HEIGHT },
	mcomDxgiFactory{},
	mcomD3dDevice{},
	mcomDxgiSwapChain{},
	mSwapChainBufferIndex{},
	mcomD3dRtvDescriptorHeap{},
	mRtvDescriptorIncrementSize{},
	mcomD3dDepthStencilBuffer{},
	mcomD3dDsvDescriptorHeap{},
	mDsvDescriptorIncrementSize{},
	mcomD3dCommandQueue{},
	mcomD3dCommandAllocator{},
	mcomD3dCommandList{},
	mcomD3dPipelineState{},
	mcomD3dFence{},
	mFenceValue{},
	mhFenceEvent{},
	mD3dViewport{},
	mD3dScissorRect{},
	mcomvD3dRenderTargetBuffers{},
	mGameTimer{}
{
	_tcscpy_s(mpszFrameRate._Elems, _T("Sunkue D3D12 ("));
}

GameFramework::~GameFramework()
{

}

bool GameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	mhInstance = hInstance;
	mhWnd = hMainWnd;

	CreateDirect3DDevice();

	//fence
	//msaa
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	//CreateRenderTargetViews();
	CreateDepthStencilView();
	//viewport
	BuildObjects();

	return true;
}

void GameFramework::OnDestroy()
{
	WaitForGpuComplete();
	ReleaseObjects();

	CloseHandle(mhFenceEvent);

	mcomDxgiSwapChain->SetFullscreenState(FALSE, NULL);

	//#if defined(_DEBUG)IDXGIDebug1 *pdxgiDebug = NULL;
	//	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgiDebug);
	//	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
	//		DXGI_DEBUG_RLO_DETAIL);
	//	pdxgiDebug->Release();
	//#endif
}

void GameFramework::CreateDirect3DDevice()
{
	UINT DXGIFactoryFlags = 0;
#ifdef _DEBUG
	{
		ComPtr<ID3D12Debug> comD3dDebugController{ nullptr };
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(comD3dDebugController.GetAddressOf())));
		if (comD3dDebugController.Get()) { comD3dDebugController->EnableDebugLayer(); }
		DXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif // _DEBUG

	ThrowIfFailed(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(mcomDxgiFactory.GetAddressOf())));

	ComPtr<IDXGIAdapter1> comD3dAdapter{};
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != mcomDxgiFactory->EnumAdapters1(i, comD3dAdapter.GetAddressOf()); ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		comD3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)continue;
		if (SUCCEEDED(D3D12CreateDevice(comD3dAdapter.Get(), D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(mcomD3dDevice.GetAddressOf()))))break;
	}

	if (nullptr == comD3dAdapter.Get())
	{
		mcomDxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(comD3dAdapter.GetAddressOf()));
		D3D12CreateDevice(comD3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mcomD3dDevice.GetAddressOf()));
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels{};
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	mcomD3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(decltype(d3dMsaaQualityLevels)));
	mMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	mbMssa4xEnable = (1 < mMsaa4xQualityLevels) ? true : false;

	constexpr decltype(mFenceValue) FENCE_INIT_VALUE{ 0 };
	ThrowIfFailed(mcomD3dDevice->CreateFence(FENCE_INIT_VALUE, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(mcomD3dFence.GetAddressOf())));
	mFenceValue = FENCE_INIT_VALUE;

	mhFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	mD3dViewport.TopLeftX = 0;
	mD3dViewport.TopLeftY = 0;
	mD3dViewport.Width = static_cast<float>(mWndClientWidth);
	mD3dViewport.Height = static_cast<float>(mWndClientHeight);
	mD3dViewport.MinDepth = 0.0f;
	mD3dViewport.MaxDepth = 1.0f;

	mD3dScissorRect = { 0,0,mWndClientWidth,mWndClientHeight };
}

void GameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc{};
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(mcomD3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,
		IID_PPV_ARGS(mcomD3dCommandQueue.GetAddressOf())));

	ThrowIfFailed(mcomD3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mcomD3dCommandAllocator.GetAddressOf())));

	ThrowIfFailed(mcomD3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mcomD3dCommandAllocator.Get(), nullptr,
		IID_PPV_ARGS(mcomD3dCommandList.GetAddressOf())));

	ThrowIfFailed(mcomD3dCommandList->Close());
}

void GameFramework::CreateSwapChain()
{
	RECT rcClient;
	GetClientRect(mhWnd, &rcClient);
	mWndClientWidth = rcClient.right - rcClient.left;
	mWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};
	dxgiSwapChainDesc.BufferDesc.Width = mWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = mWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (mbMssa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (mbMssa4xEnable) ? (mMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = mSwapChainBuffers;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = RFR;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainDesc.OutputWindow = mhWnd;
	dxgiSwapChainDesc.Windowed = true;

	ThrowIfFailed(mcomDxgiFactory->CreateSwapChain(mcomD3dCommandQueue.Get(), &dxgiSwapChainDesc,
		reinterpret_cast<IDXGISwapChain**>(mcomDxgiSwapChain.GetAddressOf())));
	mSwapChainBufferIndex = mcomDxgiSwapChain->GetCurrentBackBufferIndex();

	ThrowIfFailed(mcomDxgiFactory->MakeWindowAssociation(mhWnd, DXGI_MWA_NO_ALT_ENTER));

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif
}

void GameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc{};

	d3dDescriptorHeapDesc.NumDescriptors = mSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	ThrowIfFailed(mcomD3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		IID_PPV_ARGS(mcomD3dRtvDescriptorHeap.GetAddressOf())));
	mRtvDescriptorIncrementSize = mcomD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ThrowIfFailed(mcomD3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		IID_PPV_ARGS(mcomD3dDsvDescriptorHeap.GetAddressOf())));
	mDsvDescriptorIncrementSize = mcomD3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void GameFramework::CreateRenderTargetViews()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle{ mcomD3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < mSwapChainBuffers; ++i)
	{
		ThrowIfFailed(mcomDxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(mcomvD3dRenderTargetBuffers.at(i).GetAddressOf())));
		mcomD3dDevice->CreateRenderTargetView(mcomvD3dRenderTargetBuffers.at(i).Get(), nullptr, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.Offset(1, mRtvDescriptorIncrementSize);
	}
}

void GameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc{};
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = mWndClientWidth;
	d3dResourceDesc.Height = mWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (mbMssa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (mbMssa4xEnable) ? (mMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapPropertices{};
	d3dHeapPropertices.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapPropertices.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertices.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertices.CreationNodeMask = 1;
	d3dHeapPropertices.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue{};
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	ThrowIfFailed(mcomD3dDevice->CreateCommittedResource(&d3dHeapPropertices, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		IID_PPV_ARGS(mcomD3dDepthStencilBuffer.GetAddressOf())));

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle{ mcomD3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	mcomD3dDevice->CreateDepthStencilView(mcomD3dDepthStencilBuffer.Get(), nullptr, d3dDsvCPUDescriptorHandle);
}

void GameFramework::BuildObjects()
{

}

void GameFramework::ReleaseObjects()
{

}

void GameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState{ false };
	mcomDxgiSwapChain->GetFullscreenState(&bFullScreenState, nullptr);
	mcomDxgiSwapChain->SetFullscreenState(!bFullScreenState, nullptr);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = mWndClientWidth;
	dxgiTargetParameters.Height = mWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = RFR;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	mcomDxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (auto& RT : mcomvD3dRenderTargetBuffers)RT->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	mcomDxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	mcomDxgiSwapChain->ResizeBuffers(mSwapChainBuffers, mWndClientWidth, mWndClientHeight,
		dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	mSwapChainBufferIndex = mcomDxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GameFramework::OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam)
{
	switch (messageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_MOUSEWHEEL:
		break;
	default:break;
	}
}

void GameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam)
{
	switch (messageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F1:
			cout << "µµ¿ò¸» : \n";
			break;
		case VK_F9:
			ChangeSwapChainState();
			break;
		default:break;
		}
		break;
	default:break;
	}
}

LRESULT CALLBACK GameFramework::OnProcessingWindowMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam)
{
	switch (messageID)
	{
	case WM_SIZE:
	{
		mWndClientWidth = LOWORD(lParam);
		mWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, messageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, messageID, wParam, lParam);
		break;
	}
	return 0;
}

void GameFramework::ProcessInput()
{

}

void GameFramework::AnimateObjects()
{

}

void GameFramework::PopulateCommandList()
{
	ThrowIfFailed(mcomD3dCommandAllocator->Reset());
	ThrowIfFailed(mcomD3dCommandList->Reset(mcomD3dCommandAllocator.Get(), nullptr));
	//ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

//	m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	mcomD3dCommandList->RSSetViewports(1, &mD3dViewport);
	mcomD3dCommandList->RSSetScissorRects(1, &mD3dScissorRect);

	D3D12_RESOURCE_BARRIER RB{ CD3DX12_RESOURCE_BARRIER::Transition(mcomvD3dRenderTargetBuffers[mSwapChainBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET) };
	mcomD3dCommandList->ResourceBarrier(1, &RB);

	CD3DX12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle(mcomD3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		mSwapChainBufferIndex, mRtvDescriptorIncrementSize);

	constexpr float pfClearColor[]{ 0.0f,0.125f,0.3f,1.0f };
	mcomD3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle, pfClearColor, 0, nullptr);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle{ mcomD3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	mcomD3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mcomD3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, true, &d3dDsvCPUDescriptorHandle);
	//
	// 	   RENDERING HERE 
	/*
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	m_commandList->DrawInstanced(3, 1, 0, 0);
	*/
	//
	RB.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	RB.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mcomD3dCommandList->ResourceBarrier(1, &RB);

	ThrowIfFailed(mcomD3dCommandList->Close());
}

void GameFramework::WaitForGpuComplete()
{
	const UINT64 Fence{ mFenceValue };
	ThrowIfFailed(mcomD3dCommandQueue->Signal(mcomD3dFence.Get(), Fence));

	++mFenceValue;

	if (mcomD3dFence->GetCompletedValue() < Fence)
	{
		ThrowIfFailed(mcomD3dFence->SetEventOnCompletion(Fence, mhFenceEvent));
		WaitForSingleObject(mhFenceEvent, INFINITE);
	}

	mSwapChainBufferIndex = mcomDxgiSwapChain->GetCurrentBackBufferIndex();
}

void GameFramework::FrameAdvance()
{
	mGameTimer.Tick();

	ProcessInput();

	AnimateObjects();

	PopulateCommandList();

	ID3D12CommandList* comD3dCommandLists[]{ mcomD3dCommandList.Get() };
	mcomD3dCommandQueue->ExecuteCommandLists(_countof(comD3dCommandLists), comD3dCommandLists);


	DXGI_PRESENT_PARAMETERS dxgiPresentParameters{};
	ThrowIfFailed(mcomDxgiSwapChain->Present1(1, 0, &dxgiPresentParameters));


	_tcscpy_s(mpszFrameRate._Elems, _T("Sunkue D3D12 ("));
	const size_t mpszFrameRateLen{ wcslen(mpszFrameRate.data()) };
	mGameTimer.GetFrameRate(mpszFrameRate.data() + mpszFrameRateLen, mpszFrameRate.size());
	SetWindowText(mhWnd, mpszFrameRate.data());

	WaitForGpuComplete();
}