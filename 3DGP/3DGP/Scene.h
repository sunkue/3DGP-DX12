#pragma once

#include "Shader.h"
#include "Light.h"
#include "GameFramework.h"

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
	void AddUI(UIObject* ui) { m_UIs.push_back(ui); }
	void SetEffect(Effect* eff) { mEffect = eff; }
	HeightMapTerrain* GetTerrain()const { return mTerrain; }
	void SetFactorMode(FACTOR_MODE fm) { m_light->SetFactorMode(fm); }
	pair<bool, XMVECTOR> XM_CALLCONV RayCollapsePos(FXMVECTOR origin, FXMVECTOR direction, float dist);
	void Trees() { 
		mObjects[0]->SetMesh(0, GameFramework::GetApp()->m_Meshes["tree"]); 
		for (auto& a : mObjects)a->SetRotateSpeed(0.0f);
	}
public:
	void TeamAddCount(EnemyObject::TEAM team, int count = 1) {
		m_Team[static_cast<int>(team)].m_count += count;
	}
	auto TeamGetCount(EnemyObject::TEAM team)const {
		return m_Team[static_cast<int>(team)].m_count;
	}
	void TeamAddCount(int team, int count = 1) {
		m_Team[team].m_count += count;
	}
	auto TeamGetCount(int team)const {
		return m_Team[team].m_count;
	}
	auto TeamMaxCount()const {
		return TEAM::MAXCOUNT;
	}
	auto TeamSetMaxCount(int max)const {
		TEAM::MAXCOUNT = max;
	}
protected:
	struct TEAM {
		int m_count{ 0 };
		static inline int MAXCOUNT{ 10 };
	}m_Team[static_cast<int>(EnemyObject::TEAM::ENDCOUNT)];

protected:
	vector<InstancingShader*>	mShaders;
	ComPtr<ID3D12RootSignature>		mGraphicsRootSignature;
	Player* mPlayer;
	vector<EnemyObject*> mObjects;

	Effect* mEffect;
	HeightMapTerrain* mTerrain;

	vector<UIObject*> m_UIs;
	vector<shared_ptr<LightObj>> m_lightObjs;

	shared_ptr<Light> m_light;
};
