#pragma once

class GameObject;
class Camera;
class Effect;
class Light;
class Shader;
class Terrain;

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


protected:
	ComPtr<ID3D12RootSignature>		mGraphicsRootSignature;
	
	vector<shared_ptr<Shader>>	mShaders;
	vector<shared_ptr<GameObject>> m_Objects;
	vector<shared_ptr<Terrain>> m_Terrains;

	shared_ptr<Effect> mEffect;
	shared_ptr<Light> m_Light;
};
