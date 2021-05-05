#pragma once

#include "GameTimer.h"
#include "Scene.h"

class GameFramework
{
public:
	GameFramework();
	~GameFramework();
	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

private:
	void CreateDirect3DDevice();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateCommandQueueAndList();

	void CreateRenderTargetViews();
	void CreateDepthStencilView();

	void BuildObjects();
	void ReleaseObjects();

	void ChangeSwapChainState();

public:
	void ProcessInput();
	void AnimateObjects();
	void PopulateCommandList();
	void WaitForGpuComplete();
	void MoveToNextFrame();
	void FrameAdvance();

	void OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);

private:
	HINSTANCE mhInstance;
	HWND mhWnd;

	int mWndClientWidth;
	int mWndClientHeight;

	ComPtr<IDXGIFactory7> mcomDxgiFactory;
	ComPtr<IDXGISwapChain4> mcomDxgiSwapChain;
	ComPtr<ID3D12Device8> mcomD3dDevice;

	bool mbMssa4xEnable{ false };

	UINT mMsaa4xQualityLevels{ 0 };
	static constexpr UINT mSwapChainBuffers{ 2 };
	UINT mSwapChainBufferIndex;

	array<ComPtr<ID3D12Resource>, mSwapChainBuffers> mcomvD3dRenderTargetBuffers;
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
	array<UINT64, mSwapChainBuffers> mFenceValues;
	HANDLE mhFenceEvent;

	D3D12_VIEWPORT mD3dViewport;
	D3D12_RECT mD3dScissorRect;

	shared_ptr<Scene> mScene;

private:
	GameTimer mGameTimer;
	array<wchar_t, 50> mpszFrameRate;
};

