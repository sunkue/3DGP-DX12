#pragma once

class Shader;

class Scene
{
public:
	Scene();
	~Scene();

	void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void ReleaseObjects();
	void ReleaseUploadBuffers();

public:
	bool OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);

	bool ProcessInput();
	void AnimateObjects(milliseconds timeElapsed);
	void Render(ID3D12GraphicsCommandList* commandList);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* device);
	ID3D12RootSignature* GetGraphicsRootSignature() { return mGraphicsRootSignature.Get(); }

protected:
	vector<Shader*>					mShaders;
	ComPtr<ID3D12RootSignature>		mGraphicsRootSignature;

};

