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
	void AnimateObjects(const milliseconds timeElapsed);
	ID3D12RootSignature* GetGraphicsRootSignature()const { return mGraphicsRootSignature.Get(); }
	HeightMapTerrain* GetTerrain()const { return mTerrain; }

protected:
	bool OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);

	bool ProcessInput();
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* device);

protected:
	vector<InstancingShader>		mShaders;
	ComPtr<ID3D12RootSignature>		mGraphicsRootSignature;
	HeightMapTerrain*				mTerrain;

};

