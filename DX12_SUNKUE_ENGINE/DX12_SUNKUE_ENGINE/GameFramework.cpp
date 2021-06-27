#include "stdafx.h"
#include "Camera.h"
#include "Scene.h"
#include "GameFramework.h"
#include "Effect.h"
#include "GameTimer.h"


GameFramework* GameFramework::APP = nullptr;

GameFramework::~GameFramework()
{
	WaitForGpuComplete();
	ReleaseObjects();

	CloseHandle(m_hFenceEvent);

	m_SwapChain->SetFullscreenState(FALSE, NULL);

	APP = nullptr;

#if defined(_DEBUG)
	ComPtr<IDXGIDebug1> comDxgiDebug = NULL;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(comDxgiDebug.GetAddressOf()));
	comDxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,DXGI_DEBUG_RLO_DETAIL);
#endif
}

bool GameFramework::Initialize(HWND hwnd, HINSTANCE hInstance)
{
	m_hInstance = hInstance;
	m_hWnd = hwnd;
	if (!InitDirect3D())return false;
#ifdef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	ChanegeFullScreenMode();
#endif
	READY = true;
	APP = this;
	return true;
}

bool GameFramework::InitDirect3D()
{
	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateDepthStencilView();
	BuildObjects();
	
	return true;
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
	ThrowIfFailed(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(m_Factory.GetAddressOf())));

	ComPtr<IDXGIAdapter1> comD3dAdapter{};
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_Factory->EnumAdapters1(i, comD3dAdapter.GetAddressOf()); ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		comD3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)continue;
		if (SUCCEEDED(D3D12CreateDevice(comD3dAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_Device.GetAddressOf()))))break;
	}

	if (nullptr == comD3dAdapter.Get())
	{
		m_Factory->EnumWarpAdapter(IID_PPV_ARGS(comD3dAdapter.GetAddressOf()));
		D3D12CreateDevice(comD3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_Device.GetAddressOf()));
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels{};
	d3dMsaaQualityLevels.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount		= 4;
	d3dMsaaQualityLevels.Flags				= D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels	= 0;
	m_Device->CheckFeatureSupport(
		  D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS
		, &d3dMsaaQualityLevels
		, sizeof(decltype(d3dMsaaQualityLevels)));
	m_Msaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_IsMssa4xEnable = (1 < m_Msaa4xQualityLevels) ? (true) : (false);

	constexpr UINT64 FENCE_INIT_VALUE{ 0 };
	ThrowIfFailed(m_Device->CreateFence(
		  FENCE_INIT_VALUE
		, D3D12_FENCE_FLAG_NONE
		, IID_PPV_ARGS(m_Fence.GetAddressOf())));
	m_FenceValues.fill(FENCE_INIT_VALUE);

	m_hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void GameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc{};
	d3dCommandQueueDesc.Flags	= D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type	= D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_Device->CreateCommandQueue(
		  &d3dCommandQueueDesc
		, IID_PPV_ARGS(m_CommandQueue.GetAddressOf())));

	ThrowIfFailed(m_Device->CreateCommandAllocator(
		  D3D12_COMMAND_LIST_TYPE_DIRECT
		, IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())));

	ThrowIfFailed(m_Device->CreateCommandList(
		  0
		, D3D12_COMMAND_LIST_TYPE_DIRECT
		, m_CommandAllocator.Get()
		, nullptr
		, IID_PPV_ARGS(m_CommandList.GetAddressOf())));

	ThrowIfFailed(m_CommandList->Close());
}

void GameFramework::CreateSwapChain()
{
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	m_ClientWidth		= rcClient.right - rcClient.left;
	m_ClientHeight	= rcClient.bottom - rcClient.top;
	m_AspectRatio = static_cast<float>(m_ClientWidth) / m_ClientHeight;
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};
	dxgiSwapChainDesc.BufferDesc.Width		= m_ClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height		= m_ClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count		= (m_IsMssa4xEnable) ? (4) : (1);
	dxgiSwapChainDesc.SampleDesc.Quality	= (m_IsMssa4xEnable) ? (m_Msaa4xQualityLevels - 1) : (0);
	dxgiSwapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount			= static_cast<UINT>(m_RenderTargetBuffers.size());
	dxgiSwapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.Flags					= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator		= RFR;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainDesc.OutputWindow			= m_hWnd;
	dxgiSwapChainDesc.Windowed				= true;

	ThrowIfFailed(m_Factory->CreateSwapChain(
		  m_CommandQueue.Get()
		, &dxgiSwapChainDesc
		, reinterpret_cast<IDXGISwapChain**>(m_SwapChain.GetAddressOf())));
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	ThrowIfFailed(m_Factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER));

	CreateRenderTargetViews();
}

void GameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc{};
	d3dDescriptorHeapDesc.NumDescriptors	= static_cast<UINT>(m_RenderTargetBuffers.size());
	d3dDescriptorHeapDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags				= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask			= 0;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(
		  &d3dDescriptorHeapDesc
		, IID_PPV_ARGS(m_RtvDescriptorHeap.GetAddressOf())));
	m_RtvDescriptorIncrementSize				= m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors	= 1;
	d3dDescriptorHeapDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ThrowIfFailed(m_Device->CreateDescriptorHeap(
		  &d3dDescriptorHeapDesc
		, IID_PPV_ARGS(m_DsvDescriptorHeap.GetAddressOf())));
	m_DsvDescriptorIncrementSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void GameFramework::CreateRenderTargetViews()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle{ m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < m_RenderTargetBuffers.size(); ++i)
	{
		ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(m_RenderTargetBuffers.at(i).GetAddressOf())));
		m_Device->CreateRenderTargetView(
			  m_RenderTargetBuffers.at(i).Get()
			, nullptr
			, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.Offset(1, m_RtvDescriptorIncrementSize);
	}
}

void GameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc{};
	d3dResourceDesc.DepthOrArraySize	= 1;
	d3dResourceDesc.SampleDesc.Count	= (m_IsMssa4xEnable) ? (4) : (1);
	d3dResourceDesc.SampleDesc.Quality	= (m_IsMssa4xEnable) ? (m_Msaa4xQualityLevels - 1) : (0);
	d3dResourceDesc.Dimension	= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment	= 0;
	d3dResourceDesc.Width		= m_ClientWidth;
	d3dResourceDesc.Height		= m_ClientHeight;
	d3dResourceDesc.MipLevels	= 1;
	d3dResourceDesc.Format		= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.Flags		= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	d3dResourceDesc.Layout		= D3D12_TEXTURE_LAYOUT_UNKNOWN;

	D3D12_HEAP_PROPERTIES d3dHeapPropertices{};
	d3dHeapPropertices.Type					= D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapPropertices.CPUPageProperty		= D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertices.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertices.CreationNodeMask		= 1;
	d3dHeapPropertices.VisibleNodeMask		= 1;

	D3D12_CLEAR_VALUE d3dClearValue{};
	d3dClearValue.Format					= DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth		= 1.0f;
	d3dClearValue.DepthStencil.Stencil		= 0;

	ThrowIfFailed(m_Device->CreateCommittedResource(
		  &d3dHeapPropertices
		, D3D12_HEAP_FLAG_NONE
		, &d3dResourceDesc
		, D3D12_RESOURCE_STATE_DEPTH_WRITE
		, &d3dClearValue
		, IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle{ m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	m_Device->CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, d3dDsvCPUDescriptorHandle);
}

void GameFramework::ReleaseObjects()
{
	if (m_Scene)m_Scene->ReleaseObjects();
	if (m_Camera)m_Camera->ReleaseShaderVariables();
	if (m_Effect)m_Effect->ReleaseShaderVariables();
}

void GameFramework::ChanegeFullScreenMode()
{
	WaitForGpuComplete();

	BOOL bFullScreenNow{ false };
	ThrowIfFailed(m_SwapChain->GetFullscreenState(&bFullScreenNow, nullptr));
	ThrowIfFailed(m_SwapChain->SetFullscreenState(!bFullScreenNow, nullptr));

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width		= m_ClientWidth;
	dxgiTargetParameters.Height		= m_ClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator		= RFR;
	dxgiTargetParameters.RefreshRate.Denominator	= 1;
	dxgiTargetParameters.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_SwapChain->ResizeTarget(&dxgiTargetParameters);

	for (auto& RT : m_RenderTargetBuffers) RT.Reset();
	
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_SwapChain->GetDesc(&dxgiSwapChainDesc);
	m_SwapChain->ResizeBuffers(
		  SwapChainCount
		, m_ClientWidth
		, m_ClientHeight
		, dxgiSwapChainDesc.BufferDesc.Format
		, dxgiSwapChainDesc.Flags);
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	
	CreateRenderTargetViews();
}

LRESULT CALLBACK GameFramework::MsgProc(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam)
{
	switch (messageID)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, messageID, wParam, lParam);
		return 0;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		OnProcessingMouseMessage(hWnd, messageID, wParam, lParam);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 100;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 100;
		return 0;
	case WM_PAINT:
		if (READY) FrameAdvance();
		return 0;
	case WM_MOVE:
		assert(GetWindowRect(m_hWnd, &m_WndRect));
		return 0;
	}
	return DefWindowProc(hWnd, messageID, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GameFramework::AnimateObjects()
{
	if (m_Scene)m_Scene->AnimateObjects(m_GameTimer->GetTimeElapsedSec());
}

void GameFramework::PopulateCommandList()
{
	ThrowIfFailed(m_CommandAllocator->Reset());
	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

	D3D12_RESOURCE_BARRIER RB{ CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargetBuffers.at(m_FrameIndex).Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET) };
	m_CommandList->ResourceBarrier(1, &RB);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvCPUDescH{ m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		static_cast<INT>(m_FrameIndex), m_RtvDescriptorIncrementSize };
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescH{ m_DsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	m_CommandList->ClearRenderTargetView(rtvCPUDescH, Colors::DarkGray, 0, nullptr);
	m_CommandList->ClearDepthStencilView(dsvCPUDescH, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	m_CommandList->OMSetRenderTargets(1, &rtvCPUDescH, true, &dsvCPUDescH);
	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	

	OnPopulateCommandList();
		
	RB.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	RB.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_CommandList->ResourceBarrier(1, &RB);

	ThrowIfFailed(m_CommandList->Close());
}

void GameFramework::WaitForGpuComplete()
{
	const UINT64 Fence{ ++m_FenceValues.at(m_FrameIndex) };
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), Fence));
	if (m_Fence->GetCompletedValue() < Fence)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(Fence, m_hFenceEvent));
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void GameFramework::MoveToNextFrame()
{
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	const UINT64 Fence{ ++m_FenceValues.at(m_FrameIndex) };
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), Fence));

	if (m_Fence->GetCompletedValue() < Fence)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(Fence, m_hFenceEvent));
		WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void GameFramework::ExecuteComandLists()
{
	ID3D12CommandList* comD3dCommandLists[]{ m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(comD3dCommandLists), comD3dCommandLists);
}

void GameFramework::ShowFPS()
{
	m_title = _T("Sunkue D3D12 (");
	m_title += to_tstring(m_GameTimer->GetFrameRate());
	m_title += _T(")");
	SetWindowText(m_hWnd, m_title.data());
}

pair<XMVECTOR, XMVECTOR> GameFramework::MouseRay()
{
	auto viewport(m_Camera->GetViewPort());
	XMMATRIX VM{ m_Camera->GetViewMatrix() };
	XMMATRIX PM{ m_Camera->GetProjectionMatrix() };

	GetWindowRect(m_hWnd, &m_WndRect);
	LONG width{ m_WndRect.right - m_WndRect.left };
	LONG height{ m_WndRect.bottom - m_WndRect.top };

	float x{ (((2.0f * (float)m_OldCusorPos.x) / viewport.Width) - 1.0f) / PM.r[0].m128_f32[0] };
	float y{ (((-2.0f * (float)m_OldCusorPos.y) / viewport.Height) + 1.0f) / PM.r[1].m128_f32[1] };
	XMVECTOR origin{ 0.0f, 0.0f, 0.0f, 1.0f };
	XMVECTOR dirPoint{ x, y, 1.0f, 0.0f };
	
	XMMATRIX IVM{ XMMatrixInverse(nullptr, VM) };
	origin = XMVector3Transform(origin, IVM);
	dirPoint = XMVector3Transform(dirPoint, IVM);
	auto cameraPos{ m_Camera->GetPosition() };
	XMVECTOR dir{ XMVector3Normalize(dirPoint - origin) };

	return make_pair<>(origin, dir);
}

/// <summary>
/// ///////////////////////////////////////////////////////////////
/// </summary>

void GameFramework::OnPopulateCommandList()
{
	if (m_Scene)m_Scene->Render(m_CommandList.Get(), m_Camera.get());
}

void GameFramework::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	if (m_Effect)m_Effect->UpdateShaderVariables(commandList);
}

void GameFramework::FrameAdvance()
{
	/* º® ¶Õ ¹æÁö */
	constexpr int TIMES{ 1 };
	for (int i = 0; i < TIMES; ++i) {
		m_GameTimer->Tick();
		//cout << "TICK:" << mGameTimer.GetTimeElapsed() << "\n";
		ProcessInput();
		AnimateObjects();
	}

	PopulateCommandList();
	ExecuteComandLists();
	WaitForGpuComplete();
#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters{};
	ThrowIfFailed(mcomDxgiSwapChain->Present1(0, 0, &dxgiPresentParameters));
#else
	m_SwapChain->Present(1, 0);
#endif // _WITH_PRESENT_PARAMETERS
	MoveToNextFrame();

	ShowFPS();

	assert(m_FrameIndex < 3);
}