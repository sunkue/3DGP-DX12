#pragma once

class Camera;
class Scene;
class Effect;
class GameTimer;
class MeshUnit;
class GameObject;

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
	GameTimer* GetTimer() { return mGameTimer.get(); }
	RECT GetRECT()const { return mWndRect; }

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
	
	void BuildMeshes();
	void BuildObjects();
	void ReleaseObjects();

public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);
	pair<XMVECTOR, XMVECTOR> XM_CALLCONV MouseRay();
	pair<bool, XMVECTOR> XM_CALLCONV RayCollapsePos(FXMVECTOR origin, FXMVECTOR direction, float dist);
protected:
	void ChanegeFullScreenMode();

protected:
	HWND		mhWnd;
	HINSTANCE	mhInstance;
	int			mShowCmd;

	RECT			mWndRect;
	UINT			mWndClientWidth;
	UINT			mWndClientHeight;
	float			mAspectRatio;
	
	ComPtr<IDXGIFactory7>	mFactory;
	ComPtr<IDXGISwapChain4>	mSwapChain;
	ComPtr<ID3D12Device8>	mDevice;

	bool	mbMssa4xEnable;
	UINT	mMsaa4xQualityLevels;

	static constexpr size_t FrameCount{ 2 };
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
	shared_ptr<GameTimer>		mGameTimer;
	shared_ptr<Scene>			mScene;
	shared_ptr<Camera>			mCamera;
	shared_ptr<Effect>			mEffect;
	POINT						mOldCusorPos;
	array<TCHAR, 50>			mStrFrameRate;
};

