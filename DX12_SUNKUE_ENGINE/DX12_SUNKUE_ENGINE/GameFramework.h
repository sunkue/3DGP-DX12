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
	virtual ~GameFramework();

	bool Initialize(HWND, HINSTANCE);
	LRESULT CALLBACK MsgProc(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	void SetHWND(HWND hwnd) { m_hWnd = hwnd; }
	HWND GetHWND() { return m_hWnd; }
	float GetAspectRatio()const { return m_AspectRatio; }
	UINT GetWidth()const { return m_ClientWidth; }
	UINT GetHeight()const { return m_ClientHeight; }
	GameTimer* GetTimer() { return m_GameTimer.get(); }
	RECT GetRECT()const { return m_WndRect; }

protected:
	bool InitDirect3D();
	virtual void OnProcessingMouseMessage(HWND, UINT messageID, WPARAM, LPARAM) abstract = 0;
	virtual void OnProcessingKeyboardMessage(HWND, UINT messageID, WPARAM, LPARAM) abstract = 0;
	virtual void ProcessInput() abstract = 0;
	virtual void OnPopulateCommandList();

	void FrameAdvance();
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
	
	virtual void BuildMeshes() abstract = 0;
	virtual void BuildObjects()abstract = 0;
	void ReleaseObjects();

public:
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);
	pair<XMVECTOR, XMVECTOR> XM_CALLCONV MouseRay();
protected:
	void ChanegeFullScreenMode();

protected:
	HWND		m_hWnd;
	HINSTANCE	m_hInstance;
	RECT			m_WndRect;
	UINT			m_ClientWidth;
	UINT			m_ClientHeight;
	float			m_AspectRatio;
	
	ComPtr<IDXGIFactory7>	m_Factory;
	ComPtr<IDXGISwapChain4>	m_SwapChain;
	ComPtr<ID3D12Device8>	m_Device;

	bool	m_IsMssa4xEnable;
	UINT	m_Msaa4xQualityLevels;

	static constexpr size_t SwapChainCount{ 2 };
	UINT	m_FrameIndex;

	array<ComPtr<ID3D12Resource>, SwapChainCount>	m_RenderTargetBuffers;
	ComPtr<ID3D12DescriptorHeap>		m_RtvDescriptorHeap;
	UINT								m_RtvDescriptorIncrementSize;

	ComPtr<ID3D12Resource>				m_DepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap>		m_DsvDescriptorHeap;
	UINT								m_DsvDescriptorIncrementSize;

	ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	ComPtr<ID3D12CommandAllocator>		m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_CommandList;

	ComPtr<ID3D12PipelineState>			m_PipelineState;

	ComPtr<ID3D12Fence>					m_Fence;
	array<UINT64, SwapChainCount>		m_FenceValues;
	HANDLE								m_hFenceEvent;

protected:
	shared_ptr<GameTimer>		m_GameTimer;
	shared_ptr<Scene>			m_Scene;
	vector<shared_ptr<Scene>>	m_SceneList;
	shared_ptr<Camera>			m_Camera;
	shared_ptr<Effect>			m_Effect;
	POINT						m_OldCusorPos;
	tstring						m_title;
};

class MyApp : public GameFramework
{
protected:
	virtual void OnProcessingMouseMessage(HWND, UINT messageID, WPARAM, LPARAM);
	virtual void OnProcessingKeyboardMessage(HWND, UINT messageID, WPARAM, LPARAM);
	virtual void ProcessInput();
	virtual void BuildMeshes();
	virtual void BuildObjects();
};