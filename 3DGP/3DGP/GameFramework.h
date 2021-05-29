#pragma once

#include "GameTimer.h"
#include "Player.h"
#include "3DGP00.h"

class Camera;
class Scene;


class GameFramework
{
public:
	static GameFramework* GetApp()	{ return APP; }

private:
	static GameFramework* APP;
	bool READY;

public:
	GameFramework() = default;
	GameFramework(HINSTANCE hInstance, int showCmd);
	virtual ~GameFramework();
	bool Initialize();
	int Run();
	LRESULT CALLBACK MsgProc(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	float GetAspectRatio()const { return mAspectRatio; }
	UINT GetWidth()const { return mWndClientWidth; }
	UINT GetHeight()const { return mWndClientHeight; }
	GameTimer* GetTimer() { return &mGameTimer; }

protected:
	void OnCreate();
	void OnDestroy();
	bool InitDirect3D();
	bool InitMainWindow();
	void OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	void FrameAdvance();
	ATOM MyRegisterClass(HINSTANCE hInstance);
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

	void BuildObjects();
	void ReleaseObjects();

protected:
	void ChanegeFullScreenMode();

protected:
	HWND		mhWnd;
	HINSTANCE	mhInstance;
	int			mShowCmd;

	UINT			mWndClientWidth;
	UINT			mWndClientHeight;
	float			mAspectRatio;
	

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

protected:
	GameTimer					mGameTimer;
	Scene*						mScene;
	Camera*						mCamera;
	Player*						mPlayer;
	POINT						mOldCusorPos;
	array<wchar_t, 50>			mStrFrameRate;
};

