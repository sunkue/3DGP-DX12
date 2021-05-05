#pragma once

class Scene
{
public:
	Scene();
	~Scene();

	void CreateGraphicsRootSignature(ID3D12Device* pD3dDevice);
	void CreateGraphicsPipelineState(ID3D12Device* pD3dDevice);

	void BuildObjects(ID3D12Device* pD3dDevice);
	void ReleaseObjects();

public:
	bool OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);

	bool ProcessInput();
	void AnimateObjects(milliseconds timeElapsed);

	void PrepareRender(ID3D12GraphicsCommandList* pD3dCommandList);
	void Render(ID3D12GraphicsCommandList* pD3dCommandList);

private:
	ComPtr<ID3D12RootSignature> mcomD3dGraphicsRootSignature;
	ComPtr<ID3D12PipelineState> mcomD3dPipelineState;

};

