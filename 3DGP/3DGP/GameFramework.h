#pragma once

#include "GameTimer.h"
#include "Scene.h"
#include "3DGP00.h"

class GameFramework
{
public:
	static GameFramework* GetApp()	{ return APP; }
private:
	static GameFramework* APP;

	struct Resolution {
		UINT Width;
		UINT Height;
	};
	static constexpr array<Resolution, 8> mResolutionOptions{
		  Resolution{800u , 600u}
		, Resolution{1200u, 900u }
		, Resolution{1280u, 720u}
		, Resolution{1920u, 1080u}
		, Resolution{1920u, 1200u}
		, Resolution{2560u, 1440u}
		, Resolution{3440u, 1440u}
		, Resolution{3840u, 2160u}
	};

	UINT mResolutionIndex;
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
	void LoadSceneResolutionDependentResources();

protected:
	HWND		mhWnd;
	HINSTANCE	mhInstance;

	UINT			mWndClientWidth;
	UINT			mWndClientHeight;

	UINT			mWndInitialWidth;
	UINT			mWndInitialHeight;

	UINT			mDesktopWidth;
	UINT			mDesktopHeight;

	ComPtr<IDXGIFactory7>	mFactory;
	ComPtr<IDXGISwapChain4>	mSwapChain;
	ComPtr<ID3D12Device8>	mDevice;

	bool	mbMssa4xEnable;
	UINT	mMsaa4xQualityLevels;

	static size_t constexpr FrameCount{ 2 };
	UINT	mFrameIndex;

	array<ComPtr<ID3D12Resource>, FrameCount>	mRenderTargetBuffers;
	ComPtr<ID3D12DescriptorHeap>		mRtvDescriptorHeap;
	UINT								mRtvDescriptorIncrementSize;

	ComPtr<ID3D12Resource>				mDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap>		mDsvDescriptorHeap;
	UINT								mDsvDescriptorIncrementSize;

	ComPtr<ID3D12CommandQueue>			mCommandQueue;
	ComPtr<ID3D12CommandAllocator>		mCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>	mCommandList;

	ComPtr<ID3D12PipelineState>			mPipelineState;

	ComPtr<ID3D12Fence>					mFence;
	array<UINT64, FrameCount>			mFenceValues;
	HANDLE								mhFenceEvent;

	D3D12_VIEWPORT						mViewport;
	D3D12_RECT							mScissorRect;

protected:
	GameTimer					mGameTimer;
	shared_ptr<Scene>			mScene;
	array<wchar_t, 50>			mStrFrameRate;

};

