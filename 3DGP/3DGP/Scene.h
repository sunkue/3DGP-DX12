#pragma once

class GameObject;
class Camera;

class Scene
{
public:
	Scene();
	~Scene();

	void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void ReleaseObjects();

public:
	bool OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);

	bool ProcessInput();
	void AnimateObjects(milliseconds timeElapsed);
	void Render(ID3D12GraphicsCommandList* commandList, Camera* camera);
	
	void ReleaseUploadBuffers();

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* device);
	ID3D12RootSignature* GetGraphicsRootSignature() { return mGraphicsRootSignature.Get(); }

protected:
	vector<GameObject*>					mObjects;
	ComPtr<ID3D12RootSignature>		mGraphicsRootSignature;

};

