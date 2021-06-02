#pragma once

#include "Shader.h"

class GameObject;
class Camera;
class Effect;

class Scene
{
public:
	static Scene* SCENE;

public:
	Scene();
	~Scene();

	void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void ReleaseObjects();

public:
	void Render(ID3D12GraphicsCommandList* commandList, Camera* camera);
	void ReleaseUploadBuffers();
	void AnimateObjects(const milliseconds timeElapsed);
	ID3D12RootSignature* GetGraphicsRootSignature() { return mGraphicsRootSignature.Get(); }
	
protected:
	bool OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam);

	bool ProcessInput();
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* device);

	void CheckCollision(const milliseconds timeElapsed);

public:
	void SetPlayer(Player* player) { mPlayer = player; };
	void AddObject(EnemyObject* obj) { mObjects.push_back(obj); }
	void AddWall(WallObject* obj) { mWalls.push_back(obj); }
	void SetEffect(Effect* eff) { mEffect = eff; }
	HeightMapTerrain* GetTerrain()const { return mTerrain; }

protected:
	vector<InstancingShader>	mShaders;
	ComPtr<ID3D12RootSignature>		mGraphicsRootSignature;
	Player* mPlayer;
	vector<EnemyObject*> mObjects;
	vector<WallObject*> mWalls;
	Effect* mEffect;
	HeightMapTerrain* mTerrain;

};

