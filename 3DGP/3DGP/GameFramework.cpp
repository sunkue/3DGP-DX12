#include "stdafx.h"
#include "Camera.h"
#include "Scene.h"
#include "GameFramework.h"
#include "Effect.h"
#include "ObjReader.hpp"

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return GameFramework::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

/// //////////////////////////////////

GameFramework* GameFramework::APP = nullptr;
///////////////////////////////////////////////////////////////////////////////////////

GameFramework::GameFramework(HINSTANCE hInstance, int showCmd)
	: mhInstance					{ hInstance }
	, mShowCmd						{ showCmd }
	, mhWnd							{ nullptr }
	, mWndClientWidth				{ 0 }
	, mWndClientHeight				{ 0 }
	, mAspectRatio					{ 0.0f }
	, mFactory						{ nullptr }
	, mDevice						{ nullptr }
	, mSwapChain					{ nullptr }
	, mFrameIndex					{ 0 }
	, mRtvDescriptorHeap			{ nullptr }
	, mRtvDescriptorIncrementSize	{ 0 }
	, mDepthStencilBuffer			{ nullptr }
	, mDsvDescriptorHeap			{ nullptr }
	, mDsvDescriptorIncrementSize	{ 0 }
	, mCommandQueue					{ nullptr }
	, mCommandAllocator				{ nullptr }
	, mCommandList					{ nullptr }
	, mPipelineState				{ nullptr }
	, mFence						{ nullptr }
	, mFenceValues					{ 0 }
	, mhFenceEvent					{ nullptr }
	, mRenderTargetBuffers			{ nullptr }
	, mGameTimer					{}
{
	APP = this;
	READY = false;

	
}

GameFramework::~GameFramework()
{

}

int GameFramework::Run()
{
	MSG msg{};
	SetCursorPos(mWndClientWidth / 2, mWndClientHeight / 2);
	GetCursorPos(&mOldCusorPos);
	HACCEL hAccelTable = LoadAccelerators(mhInstance, MAKEINTRESOURCE(IDC_MY3DGP1));

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	OnDestroy();

	return static_cast<int>(msg.wParam);
}

bool GameFramework::Initialize()
{
	if (!InitMainWindow())return false;
	if (!InitDirect3D())return false;
#ifdef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	ChanegeFullScreenMode();
#endif
	READY = true;
	ShowWindow(mhWnd, mShowCmd);
	UpdateWindow(mhWnd);
	return true;
}

ATOM GameFramework::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= MainWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= mhInstance;
	wcex.hIcon			= LoadIcon(0, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground	= static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= L"MainWnd";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

bool GameFramework::InitMainWindow()
{
	// 데스크탑 해상도를 받아온다
	HWND hDesktop = GetDesktopWindow();

	GetWindowRect(hDesktop, &mWndRect);
	mWndClientWidth = mWndRect.right - mWndRect.left;
	mWndClientHeight = mWndRect.bottom - mWndRect.top;
	mAspectRatio = static_cast<float>(mWndClientWidth) / mWndClientHeight;
	if (!MyRegisterClass(mhInstance))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT rc{ 0, 0, static_cast<LONG>(mWndClientWidth), static_cast<LONG>(mWndClientHeight) };
	DWORD dwStyle{ 0
		| WS_OVERLAPPED 
		| WS_SYSMENU 
		| WS_MINIMIZEBOX 
		| WS_CAPTION 
		| WS_BORDER 
	};
	AdjustWindowRectEx(&mWndRect, dwStyle, FALSE, dwStyle);
	
	mhWnd = CreateWindowW(
		  L"MainWnd"
		, L"SUNKUE D3D12 App"
		, dwStyle
		, CW_USEDEFAULT
		, CW_USEDEFAULT
		, mWndClientWidth
		, mWndClientHeight
		, nullptr
		, nullptr
		, mhInstance
		, nullptr);

	if (!mhWnd)
	{
		return false;
	}

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

void GameFramework::OnCreate()
{

}

void GameFramework::OnDestroy()
{
	WaitForGpuComplete();
	ReleaseObjects();

	CloseHandle(mhFenceEvent);

	ThrowIfFailed(mSwapChain->SetFullscreenState(FALSE, NULL));

	APP = nullptr;

#if defined(_DEBUG)
	ComPtr<IDXGIDebug1> comDxgiDebug = NULL;
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(comDxgiDebug.GetAddressOf()));
	ThrowIfFailed(comDxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,
		DXGI_DEBUG_RLO_DETAIL));
#endif
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
	ThrowIfFailed(CreateDXGIFactory2(DXGIFactoryFlags, IID_PPV_ARGS(mFactory.GetAddressOf())));

	ComPtr<IDXGIAdapter1> comD3dAdapter{};
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != mFactory->EnumAdapters1(i, comD3dAdapter.GetAddressOf()); ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		comD3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)continue;
		if (SUCCEEDED(D3D12CreateDevice(comD3dAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(mDevice.GetAddressOf()))))break;
	}

	if (nullptr == comD3dAdapter.Get())
	{
		mFactory->EnumWarpAdapter(IID_PPV_ARGS(comD3dAdapter.GetAddressOf()));
		D3D12CreateDevice(comD3dAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(mDevice.GetAddressOf()));
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels{};
	d3dMsaaQualityLevels.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount		= 4;
	d3dMsaaQualityLevels.Flags				= D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels	= 0;
	mDevice->CheckFeatureSupport(
		  D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS
		, &d3dMsaaQualityLevels
		, sizeof(decltype(d3dMsaaQualityLevels)));
	mMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	mbMssa4xEnable = (1 < mMsaa4xQualityLevels) ? (true) : (false);

	constexpr UINT64 FENCE_INIT_VALUE{ 0 };
	ThrowIfFailed(mDevice->CreateFence(
		  FENCE_INIT_VALUE
		, D3D12_FENCE_FLAG_NONE
		, IID_PPV_ARGS(mFence.GetAddressOf())));
	mFenceValues.fill(FENCE_INIT_VALUE);

	mhFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void GameFramework::CreateCommandQueueAndList()
{
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc{};
	d3dCommandQueueDesc.Flags	= D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type	= D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(mDevice->CreateCommandQueue(
		  &d3dCommandQueueDesc
		, IID_PPV_ARGS(mCommandQueue.GetAddressOf())));

	ThrowIfFailed(mDevice->CreateCommandAllocator(
		  D3D12_COMMAND_LIST_TYPE_DIRECT
		, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())));

	ThrowIfFailed(mDevice->CreateCommandList(
		  0
		, D3D12_COMMAND_LIST_TYPE_DIRECT
		, mCommandAllocator.Get()
		, nullptr
		, IID_PPV_ARGS(mCommandList.GetAddressOf())));

	ThrowIfFailed(mCommandList->Close());
}

void GameFramework::CreateSwapChain()
{
	RECT rcClient;
	GetClientRect(mhWnd, &rcClient);
	mWndClientWidth		= rcClient.right - rcClient.left;
	mWndClientHeight	= rcClient.bottom - rcClient.top;
	mAspectRatio = static_cast<float>(mWndClientWidth) / mWndClientHeight;
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};
	dxgiSwapChainDesc.BufferDesc.Width		= mWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height		= mWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count		= (mbMssa4xEnable) ? (4) : (1);
	dxgiSwapChainDesc.SampleDesc.Quality	= (mbMssa4xEnable) ? (mMsaa4xQualityLevels - 1) : (0);
	dxgiSwapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount			= static_cast<UINT>(mRenderTargetBuffers.size());
	dxgiSwapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.Flags					= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator		= RFR;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainDesc.OutputWindow			= mhWnd;
	dxgiSwapChainDesc.Windowed				= true;

	ThrowIfFailed(mFactory->CreateSwapChain(
		  mCommandQueue.Get()
		, &dxgiSwapChainDesc
		, reinterpret_cast<IDXGISwapChain**>(mSwapChain.GetAddressOf())));
	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

	ThrowIfFailed(mFactory->MakeWindowAssociation(mhWnd, DXGI_MWA_NO_ALT_ENTER));

	CreateRenderTargetViews();
}

void GameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc{};
	d3dDescriptorHeapDesc.NumDescriptors	= static_cast<UINT>(mRenderTargetBuffers.size());
	d3dDescriptorHeapDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags				= D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask			= 0;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		  &d3dDescriptorHeapDesc
		, IID_PPV_ARGS(mRtvDescriptorHeap.GetAddressOf())));
	mRtvDescriptorIncrementSize				= mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors	= 1;
	d3dDescriptorHeapDesc.Type				= D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ThrowIfFailed(mDevice->CreateDescriptorHeap(
		  &d3dDescriptorHeapDesc
		, IID_PPV_ARGS(mDsvDescriptorHeap.GetAddressOf())));
	mDsvDescriptorIncrementSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void GameFramework::CreateRenderTargetViews()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle{ mRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	for (UINT i = 0; i < mRenderTargetBuffers.size(); ++i)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(mRenderTargetBuffers.at(i).GetAddressOf())));
		mDevice->CreateRenderTargetView(
			  mRenderTargetBuffers.at(i).Get()
			, nullptr
			, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.Offset(1, mRtvDescriptorIncrementSize);
	}
}

void GameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc{};
	d3dResourceDesc.DepthOrArraySize	= 1;
	d3dResourceDesc.SampleDesc.Count	= (mbMssa4xEnable) ? (4) : (1);
	d3dResourceDesc.SampleDesc.Quality	= (mbMssa4xEnable) ? (mMsaa4xQualityLevels - 1) : (0);
	d3dResourceDesc.Dimension	= D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment	= 0;
	d3dResourceDesc.Width		= mWndClientWidth;
	d3dResourceDesc.Height		= mWndClientHeight;
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

	ThrowIfFailed(mDevice->CreateCommittedResource(
		  &d3dHeapPropertices
		, D3D12_HEAP_FLAG_NONE
		, &d3dResourceDesc
		, D3D12_RESOURCE_STATE_DEPTH_WRITE
		, &d3dClearValue
		, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle{ mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
	mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, d3dDsvCPUDescriptorHandle);
}

void GameFramework::BuildMeshes()
{
	auto const d{ mDevice.Get() };
	auto const c{ mCommandList.Get() };
	vector<Vertex> v;
	string file;
	string const dir{ "Assets/Model/" };
	string const ext{ ".bin" };
	string const ext2{ ".obj" };

	/*file = "tree"; v = LoadObj<Vertex>(dir + file + ext2);
	SaveMeshAsBinary(v, dir + file + ext);
	file = "cube"; v = LoadObj<Vertex>(dir + file + ext2);
	SaveMeshAsBinary(v, dir + file + ext);
	file = "sphere"; v = LoadObj<Vertex>(dir + file + ext2);
	SaveMeshAsBinary(v, dir + file + ext);
	file = "girl"; v = LoadObj<Vertex>(dir + file + ext2);
	SaveMeshAsBinary(v, dir + file + ext);*/


	file = "tree"; v = LoadMeshFromBinary<Vertex>(dir + file + ext);
	m_Meshes.emplace(file, new Mesh{d,c,v});
	m_Meshes[file]->SetMeterial({ 0.9f,0.5f,0.5f,0.0f }, { 0.9f,0.5f,0.5f,0.0f });
	m_Meshes[file]->SetOOBB({ {0.0f,0.0f,0.0f}, {2.0f,25.0f,2.0f}, {0.0f,0.0f,0.0f,1.0f} });

	file = "cube"; v = LoadMeshFromBinary<Vertex>(dir + file + ext);
	m_Meshes.emplace(file, new Mesh{ d,c,v });
	m_Meshes[file]->SetMeterial({ 0.9f,0.5f,0.5f,0.0f }, { 0.9f,0.5f,0.5f,0.0f });
	m_Meshes[file]->SetOOBB({ {0.0f,0.0f,0.0f}, {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f,1.0f} });

	file = "sphere"; v = LoadMeshFromBinary<Vertex>(dir + file + ext);
	m_Meshes.emplace(file, new Mesh{ d,c,v });
	m_Meshes[file]->SetMeterial({ 0.9f,0.5f,0.5f,0.0f }, { 0.9f,0.5f,0.5f,0.0f });

	file = "girl"; v = LoadMeshFromBinary<Vertex>(dir + file + ext);
	m_Meshes.emplace(file, new Mesh{ d,c,v });
	m_Meshes[file]->SetMeterial({ 0.2f,0.5f,0.6f,0.0f }, { 0.5f,0.7f,0.8f,0.0f });



	//file = "onj";
	//m_Meshes.emplace(file, new Mesh{d,c,v});
}

void GameFramework::BuildObjects()
{
	mGameTimer.Reset();
	mCommandList->Reset(mCommandAllocator.Get(), nullptr);

	BuildMeshes();

	mScene = new Scene();
	mScene->BuildObjects(mDevice.Get(), mCommandList.Get());

	mPlayer = new TerrainPlayer(mDevice.Get(), mCommandList.Get(), mScene->GetGraphicsRootSignature(), mScene->GetTerrain(), 1);
	//mPlayer->SetMesh(0, m_Meshes["tree"]);
	mCamera = mPlayer->GetCamera();

	mEffect = new Effect(mDevice.Get(), mCommandList.Get(), 2);
	mPlayer->SetEffect(mEffect);
	mScene->SetEffect(mEffect);
	
	mCommandList->Close();
	ExecuteComandLists();
	WaitForGpuComplete();

	mScene->ReleaseUploadBuffers();

	mGameTimer.Reset();
}

void GameFramework::ReleaseObjects()
{
	if (mScene)mScene->ReleaseObjects();
	delete mScene;
	
	if (mCamera)mCamera->ReleaseShaderVariables();
	delete mCamera;

	if (mEffect)mEffect->ReleaseShaderVariables();
	delete mEffect;
}

void GameFramework::ChanegeFullScreenMode()
{
	WaitForGpuComplete();

	BOOL bFullScreenNow{ false };
	ThrowIfFailed(mSwapChain->GetFullscreenState(&bFullScreenNow, nullptr));
	ThrowIfFailed(mSwapChain->SetFullscreenState(!bFullScreenNow, nullptr));

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format		= DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width		= mWndClientWidth;
	dxgiTargetParameters.Height		= mWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator		= RFR;
	dxgiTargetParameters.RefreshRate.Denominator	= 1;
	dxgiTargetParameters.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	mSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (auto& RT : mRenderTargetBuffers) RT.Reset();
	
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	mSwapChain->GetDesc(&dxgiSwapChainDesc);
	mSwapChain->ResizeBuffers(
		  FrameCount
		, mWndClientWidth
		, mWndClientHeight
		, dxgiSwapChainDesc.BufferDesc.Format
		, dxgiSwapChainDesc.Flags);
	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();
	
	CreateRenderTargetViews();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void GameFramework::OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam)
{
	switch (messageID)
	{
	case WM_LBUTTONDOWN: {
		//GetCursorPos(&mOldCusorPos);
		auto ray{ MouseRay() };
		auto result{ RayCollapsePos(ray.first, ray.second, FLT_MAX) };
		if (true == result.first) mEffect->NewWallEffect(result.second, 0.25f);
		//SetCapture(hWnd);
		//GetCursorPos(&mOldCusorPos);
	}break;
	case WM_LBUTTONUP:
		//ReleaseCapture();
		break;
	case WM_RBUTTONDOWN: {
		
	}break;
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:

		break;
	case WM_MOUSEWHEEL: {
		XMVECTOR offSet{ mCamera->GetOffset() };
		float Z{ XMVectorGetZ(offSet) - std::copysignf(10.0f, -static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam))) };
		offSet = XMVectorSetZ(offSet, mCamera->ClampOffset(Z));
		mCamera->SetOffset(offSet);
	}break;
	default:break;
	}
}

void GameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam)
{
	switch (messageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		case VK_SPACE:
			//mPlayer->Jump(10.0f);
			mPlayer->Jump(250ms);
			break;
		case VK_RETURN:
			break;
		case VK_TAB:
			if (mPlayer)mCamera = mPlayer->ChangeCamera(static_cast<CAMERA_MODE>(
				mPlayer->GetCamera()->GetMode() == static_cast <CAMERA_MODE>(3) ? (1) : (3)), mGameTimer.GetTimeElapsed());
			break;
		case VK_SHIFT:
			if (mPlayer) { mPlayer->SetEvolve(true); }
			break;
		case VK_F9:
			ChanegeFullScreenMode();
			break;
		default:break;
		}
		break;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SHIFT:
			if (mPlayer) { mPlayer->FinishEvolving(); }
			break;
		default:break;
		}
		break;
	default:break;
	}
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
		assert(GetWindowRect(mhWnd, &mWndRect));
		return 0;
	}
	return DefWindowProc(hWnd, messageID, wParam, lParam);
}

void GameFramework::ProcessInput()
{
	static UCHAR key[256];
	BYTE dir{ 0 };
	
	if (GetKeyboardState(key))
	{
		if (key['W'] & 0xf0)dir |= DIR_FORWARD;
		if (key['S'] & 0xf0)dir |= DIR_BACKWARD;
		if (key['A'] & 0xf0)dir |= DIR_LEFT;
		if (key['D'] & 0xf0)dir |= DIR_RIGHT;

		if (key[VK_UP] & 0xf0)dir |= DIR_FORWARD;
		if (key[VK_DOWN] & 0xf0)dir |= DIR_BACKWARD;
		if (key[VK_LEFT] & 0xf0)dir |= DIR_LEFT;
		if (key[VK_RIGHT] & 0xf0)dir |= DIR_RIGHT;

		if (key[VK_SHIFT] & 0xf0)dir |= DIR_UP;
		if (key[VK_CONTROL] & 0xf0)dir |= DIR_DOWN;


		if (key['1'] & 0xf0)mScene->SetFactorMode(FACTOR_MODE::DEFAULT);
		if (key['2'] & 0xf0)mScene->SetFactorMode(FACTOR_MODE::CATOON);
		if (key['3'] & 0xf0)mScene->SetFactorMode(FACTOR_MODE::SMOOTH);

	}
	float deltaX{ 0.0f };
	float deltaY{ 0.0f };
	POINT cursorPos;
	
	GetCursorPos(&cursorPos);
	deltaX = static_cast<float>((cursorPos.x - mOldCusorPos.x) / 3.0f);
	deltaY = static_cast<float>((cursorPos.y - mOldCusorPos.y) / 3.0f);
	mOldCusorPos = cursorPos;
	//SetCursorPos(mOldCusorPos.x, mOldCusorPos.y);

	if ((dir != 0) || (deltaX != 0.0f) || (deltaY != 0.0f)) {
		if (deltaX || deltaY) {
			//if (key[VK_RBUTTON] & 0xF0)mPlayer->Rotate(deltaY, 0.0f, -deltaX);
			mPlayer->Rotate(deltaY, deltaX, 0.0f);
		}
		if (dir) {
			constexpr float MagicSpeed{ 400.0f };
			mPlayer->Move(dir, MagicSpeed * mGameTimer.GetTimeElapsed().count() / 1000.0f, true);
		}
	}
	mPlayer->Update(mGameTimer.GetTimeElapsed());
}

void GameFramework::AnimateObjects()
{
	if (mScene)mScene->AnimateObjects(mGameTimer.GetTimeElapsed());
}

void GameFramework::PopulateCommandList()
{
	ThrowIfFailed(mCommandAllocator->Reset());
	ThrowIfFailed(mCommandList->Reset(mCommandAllocator.Get(), nullptr));

	D3D12_RESOURCE_BARRIER RB{ CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargetBuffers.at(mFrameIndex).Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET) };
	mCommandList->ResourceBarrier(1, &RB);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvCPUDescH{ mRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		static_cast<INT>(mFrameIndex), mRtvDescriptorIncrementSize };
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescH{ mDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };

	mCommandList->ClearRenderTargetView(rtvCPUDescH, Colors::Black, 0, nullptr);
	mCommandList->ClearDepthStencilView(dsvCPUDescH, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	mCommandList->OMSetRenderTargets(1, &rtvCPUDescH, true, &dsvCPUDescH);

	
	if (mScene)mScene->Render(mCommandList.Get(), mCamera);
#ifdef WITH_PLAYER_TOP
	mCommandList->ClearDepthStencilView(dsvCPUDescH, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
#endif // With_PLAYER_TOP
	if (mPlayer)mPlayer->Render(mCommandList.Get(), mCamera);
		
	RB.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	RB.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCommandList->ResourceBarrier(1, &RB);

	ThrowIfFailed(mCommandList->Close());
}

void GameFramework::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	if (mEffect)mEffect->UpdateShaderVariables(commandList);
}

void GameFramework::WaitForGpuComplete()
{
	const UINT64 Fence{ ++mFenceValues.at(mFrameIndex) };
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), Fence));
	if (mFence->GetCompletedValue() < Fence)
	{
		ThrowIfFailed(mFence->SetEventOnCompletion(Fence, mhFenceEvent));
		WaitForSingleObject(mhFenceEvent, INFINITE);
	}
}

void GameFramework::MoveToNextFrame()
{
	mFrameIndex = mSwapChain->GetCurrentBackBufferIndex();

	const UINT64 Fence{ ++mFenceValues.at(mFrameIndex) };
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), Fence));

	if (mFence->GetCompletedValue() < Fence)
	{
		ThrowIfFailed(mFence->SetEventOnCompletion(Fence, mhFenceEvent));
		WaitForSingleObject(mhFenceEvent, INFINITE);
	}
}
void GameFramework::ExecuteComandLists()
{
	ID3D12CommandList* comD3dCommandLists[]{ mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(comD3dCommandLists), comD3dCommandLists);
}

void GameFramework::ShowFPS()
{
	wcscpy_s(mStrFrameRate.data(), mStrFrameRate.size() - 1, _T("Sunkue D3D12 ("));
	size_t len{ wcslen(mStrFrameRate.data()) };
	mGameTimer.GetFrameRate(mStrFrameRate.data() + len, mStrFrameRate.size() - 1 - len);
	SetWindowText(mhWnd, mStrFrameRate.data());
}

pair<XMVECTOR, XMVECTOR> GameFramework::MouseRay()
{
	auto viewport(mCamera->GetViewPort());
	XMMATRIX VM{ mCamera->GetViewMatrix() };
	XMMATRIX PM{ mCamera->GetProjectionMatrix() };

	GetWindowRect(mhWnd, &mWndRect);
	LONG width{ mWndRect.right - mWndRect.left };
	LONG height{ mWndRect.bottom - mWndRect.top };

	float x{ (((2.0f * (float)mOldCusorPos.x) / viewport.Width) - 1.0f) / PM.r[0].m128_f32[0] };
	float y{ (((-2.0f * (float)mOldCusorPos.y) / viewport.Height) + 1.0f) / PM.r[1].m128_f32[1] };
	XMVECTOR origin{ 0.0f, 0.0f, 0.0f, 1.0f };
	XMVECTOR dirPoint{ x, y, 1.0f, 0.0f };
	
	XMMATRIX IVM{ XMMatrixInverse(nullptr, VM) };
	origin = XMVector3Transform(origin, IVM);
	dirPoint = XMVector3Transform(dirPoint, IVM);
	auto cameraPos{ mCamera->GetPosition() };
	XMVECTOR dir{ XMVector3Normalize(dirPoint - origin) };

	return make_pair<>(origin, dir);
}

pair<bool, XMVECTOR> GameFramework::RayCollapsePos(FXMVECTOR origin, FXMVECTOR direction, float dist)
{
	return mScene->RayCollapsePos(origin, direction, dist);
}


void GameFramework::FrameAdvance()
{
	/* 벽 뚫 방지 */
	
	constexpr int TIMES{ 1 };
	for (int i = 0; i < TIMES; ++i) {
		mGameTimer.Tick();
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
	mSwapChain->Present(1, 0);
#endif // _WITH_PRESENT_PARAMETERS
	MoveToNextFrame();

	ShowFPS();

	assert(mFrameIndex < 3);
}