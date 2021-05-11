#pragma once

#include "GameTimer.h"
#include "Scene.h"
#include "3DGP00.h"

class GameFramework
{
public:
	GameFramework() = default;
	GameFramework(HINSTANCE hInstance);
	virtual ~GameFramework();
	void OnCreate();
	void OnDestroy();
	bool Initialize();
	bool InitMainWindow();
	bool InitDirect3D();

public:
	int Run();
	void FrameAdvance();
	void OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MsgProc(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	ATOM MyRegisterClass(HINSTANCE hInstance);

	static GameFramework* GetApp() { return APP; }

protected:
	void ProcessInput();
	void AnimateObjects();
	void PopulateCommandList();
	void ExecuteComandLists();
	void WaitForGpuComplete();
	void MoveToNextFrame();
	void ShowFPS();

protected:
	void CreateDirect3DDevice();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();
	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	void SetViewportScissorRect();

	void BuildObjects();
	void ReleaseObjects();

protected:
	void ChanegeFullScreenMode();

protected:
	static GameFramework* APP;

protected:
	HWND mhWnd;
	HINSTANCE mhInstance;

	int mWndClientWidth;
	int mWndClientHeight;

	ComPtr<IDXGIFactory7> mcomDxgiFactory;
	ComPtr<IDXGISwapChain4> mcomDxgiSwapChain;
	ComPtr<ID3D12Device8> mcomD3dDevice;

	bool mbMssa4xEnable{ false };

	UINT mMsaa4xQualityLevels{ 0 };
	static constexpr UINT mcexprSwapChainBuffers{ 2 };
	UINT mSwapChainBufferIndex;

	array<ComPtr<ID3D12Resource>, mcexprSwapChainBuffers> mcomvD3dRenderTargetBuffers;
	ComPtr<ID3D12DescriptorHeap> mcomD3dRtvDescriptorHeap;
	UINT mRtvDescriptorIncrementSize;

	ComPtr<ID3D12Resource> mcomD3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> mcomD3dDsvDescriptorHeap;
	UINT mDsvDescriptorIncrementSize;

	ComPtr<ID3D12CommandQueue> mcomD3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> mcomD3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> mcomD3dCommandList;

	ComPtr<ID3D12PipelineState> mcomD3dPipelineState;

	ComPtr<ID3D12Fence> mcomD3dFence;
	array<UINT64, mcexprSwapChainBuffers> mFenceValues;
	HANDLE mhFenceEvent;

	D3D12_VIEWPORT mD3dViewport;
	D3D12_RECT mD3dScissorRect;

protected:
	GameTimer mGameTimer;
	shared_ptr<Scene> mScene;
	array<wchar_t, 50> mStrFrameRate;

};

