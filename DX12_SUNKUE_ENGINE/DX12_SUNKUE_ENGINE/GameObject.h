#pragma once

class Shader;
class Camera;
class HeightMapImage;
class Mesh;

class GameObject : public Collideable , public IShaderResourceHelper
{
public:
	explicit GameObject(int meshes = 1);
	virtual ~GameObject() = default;

public:	
	void SetMesh(int index, const shared_ptr<Mesh> mesh) { m_meshes.at(index) = mesh; };
	void SetShader(const shared_ptr<Shader> shader) { m_shader = shader; };

	void Animate(float timeElapsed);
	virtual void OnAnimate(float timeElapsed) {};

	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera, UINT instanceCount = 1);

protected:
	virtual void PrepareRender(ID3D12GraphicsCommandList* commandList);

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override {};
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables()override {};

public:
	const vector<shared_ptr<Mesh>>& GetMesh()const { return m_meshes; }
	XMVECTOR XM_CALLCONV GetRight()const { return XMVectorSet(m_worldMatNoScale._11, m_worldMatNoScale._12, m_worldMatNoScale._13, 0.0f); };
	XMVECTOR XM_CALLCONV GetUp()const { return XMVectorSet(m_worldMatNoScale._21, m_worldMatNoScale._22, m_worldMatNoScale._23, 0.0f); };
	XMVECTOR XM_CALLCONV GetLook()const { return XMVectorSet(m_worldMatNoScale._31, m_worldMatNoScale._32, m_worldMatNoScale._33, 0.0f); }
	XMVECTOR XM_CALLCONV GetPosition()const { return XMVectorSet(m_worldMatNoScale._41, m_worldMatNoScale._42, m_worldMatNoScale._43, 1.0f); }
	void SetPosition(float x, float y, float z) { m_worldMatNoScale._41 = x; m_worldMatNoScale._42 = y; m_worldMatNoScale._43 = z; }
	void XM_CALLCONV SetPosition(FXMVECTOR pos) { SetPosition(XMVectorGetX(pos), XMVectorGetY(pos), XMVectorGetZ(pos)); };

	XMVECTOR XM_CALLCONV GetScale()const { return Load(m_scale); }
	void XM_CALLCONV SetScale(FXMVECTOR scale) { Store(m_scale, scale); }

	XMMATRIX XM_CALLCONV GetWM()const { return XMMatrixScalingFromVector(GetScale())*Load(m_worldMatNoScale); }

	void MoveRight(float distance = 1.0f);
	void MoveUp(float distance = 1.0f);
	void MoveFoward(float distance = 1.0f);
	void XM_CALLCONV Move(FXMVECTOR vel);

	void XM_CALLCONV RotateByAxis(FXMVECTOR xmf3Axis, float angle);
	void RotateByPYR(float pitch = 10.0f, float yaw = 10.0f, float roll = 10.0f);
	void RotateByMatrix(FXMMATRIX rotate) { Store(m_worldMatNoScale, rotate * Load(m_worldMatNoScale)); }

public:
	virtual bool IsVisible(const Camera* = nullptr);

protected:
	XMFLOAT4X4 m_worldMatNoScale;
	XMFLOAT3 m_scale{ 1.0f,1.0f,1.0f };
	vector<shared_ptr<Mesh>> m_meshes;
	shared_ptr<Shader> m_shader;

public:
	template<typename Pred, typename... Args>
	void ForFamily(Pred funct, Args&&... args)
	{
		if (nullptr != m_child)
		{
			funct(*m_child, std::forward<Args>(args)...);
		}
		if (nullptr != m_brother)
		{
			funct(*m_brother, std::forward<Args>(args)...);
		}
	}
	GameObject* GetBrother()const { return m_brother.get(); }
	GameObject* GetChild()const { return m_child.get(); }
	void SetBrother(const shared_ptr<GameObject> br) { m_brother = br; }
	void SetChild(const shared_ptr<GameObject> ch) { m_child = ch; }
protected:
	shared_ptr<GameObject> m_brother;
	shared_ptr<GameObject> m_child;
};

class InstancingObject : public GameObject
{
public:
	explicit InstancingObject(int meshes = 1) : GameObject(meshes) {};
	virtual ~InstancingObject() = default;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList*)final {}; // do not update constbuffer
};