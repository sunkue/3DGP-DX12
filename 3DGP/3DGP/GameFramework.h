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
	void OnSizeChange(UINT width, UINT height, bool minimized);
	void UpdateForSizeChange(UINT clientWidth, UINT clientHeight);
	void LoadSizeDependentResources();
	void LoadSceneResolutionDependentResources();
	void SetWindowBounds(int left, int top, int right, int bottom);
protected:
	HWND		mhWnd;
	HINSTANCE	mhInstance;

	UINT		mWndClientWidth;
	UINT		mWndClientHeight;
	float		mAspecRatio;
	RECT		mWindowBounds;
	ComPtr<IDXGIFactory7>	mcomDxgiFactory;
	ComPtr<IDXGISwapChain4>	mcomDxgiSwapChain;
	ComPtr<ID3D12Device8>	mcomD3dDevice;

	bool		mbMssa4xEnable;
	bool		mbFullScreen;

	UINT		mMsaa4xQualityLevels;
	UINT		mFrameIndex;

	static constexpr UINT						FrameCount{ 2 };

	array<ComPtr<ID3D12Resource>, FrameCount>	mcomvD3dRenderTargetBuffers;
	ComPtr<ID3D12DescriptorHeap>				mcomD3dRtvDescriptorHeap;
	UINT										mRtvDescriptorIncrementSize;

	ComPtr<ID3D12Resource>						mcomD3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap>				mcomD3dDsvDescriptorHeap;
	UINT										mDsvDescriptorIncrementSize;

	ComPtr<ID3D12CommandQueue>					mcomD3dCommandQueue;
	ComPtr<ID3D12CommandAllocator>				mcomD3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>			mcomD3dCommandList;

	ComPtr<ID3D12PipelineState>					mcomD3dPipelineState;

	ComPtr<ID3D12Fence>							mcomD3dFence;
	array<UINT64, FrameCount>					mFenceValues;
	HANDLE										mhFenceEvent;

	D3D12_VIEWPORT								mD3dViewport;
	D3D12_RECT									mD3dScissorRect;


protected:
	GameTimer					mGameTimer;
	shared_ptr<Scene>			mScene;
	array<wchar_t, 50>			mStrFrameRate;

};

