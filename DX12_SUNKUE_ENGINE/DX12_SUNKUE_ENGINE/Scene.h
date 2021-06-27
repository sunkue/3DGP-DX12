#pragma once

class GameObject;
class Camera;
class Effect;
class LightShader;
class Shader;
class Terrain;

class Scene abstract
{
public:
	Scene() = default;
	virtual ~Scene() = default;

	virtual void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) = 0;
	void ReleaseObjects();

public:
	void Render(ID3D12GraphicsCommandList* commandList, Camera* camera);
	void AnimateObjects(float timeElapsed);
	ID3D12RootSignature* GetGraphicsRootSignature(size_t index = 0) { return m_RootSignatures.at(index).Get(); }
	
protected:
	virtual void PrepareRender(ID3D12GraphicsCommandList* commandList, Camera* camera) abstract = 0;
	
	virtual void OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam) abstract = 0;
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam) abstract = 0;
	virtual void ProcessInput() abstract = 0;

	virtual ID3D12RootSignature* CreateGraphicsRootSignatures(ID3D12Device* device) abstract = 0;
	virtual void CheckCollision(float timeElapsed) abstract = 0;

protected:
	vector<ComPtr<ID3D12RootSignature>>	m_RootSignatures;
	
	vector<shared_ptr<GameObject>> m_Objects;
	vector<shared_ptr<Shader>> m_Shaders;
	vector<shared_ptr<Terrain>> m_Terrains;

	shared_ptr<Effect> m_Effect;
	shared_ptr<LightShader> m_Light;
};
