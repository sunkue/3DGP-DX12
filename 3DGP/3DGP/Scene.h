#pragma once

#include "Shader.h"

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
	void Render(ID3D12GraphicsCommandList* commandList, Camera* camera);
	void ReleaseUploadBuffers();
	void AnimateObjects(milliseconds timeElapsed);
	
protected:
	bool OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);

	bool ProcessInput();
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* device);
	ID3D12RootSignature* GetGraphicsRootSignature() { return mGraphicsRootSignature.Get(); }

protected:
	vector<ObjectsShader>			mShaders;
	ComPtr<ID3D12RootSignature>		mGraphicsRootSignature;

};

