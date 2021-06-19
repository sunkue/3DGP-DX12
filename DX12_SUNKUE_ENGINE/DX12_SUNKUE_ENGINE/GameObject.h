#pragma once



class Shader;
class Camera;
class HeightMapImage;

class GameObject : public Collideable
{
public:
	GameObject(int meshes = 1);
	virtual ~GameObject();

public:
	void ReleaseUploadBuffers();
	
	void SetMesh(int index, const shared_ptr<Mesh>& mesh) { m_meshes[index] = mesh; };
	void SetShader(const shared_ptr<Shader>& shader) { m_shader = shader; };

	virtual void Animate(const float timeElapsed);

	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera, UINT instanceCount = 1);

protected:
	virtual void PrepareRender();
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);
	virtual void ReleaseShaderVariables();

public:
	vector<shared_ptr<Mesh>> const& GetMesh() { return m_meshes; }
	XMVECTOR XM_CALLCONV GetPosition()	const;
	XMVECTOR XM_CALLCONV GetLook() const;
	XMVECTOR XM_CALLCONV GetUp() const;
	XMVECTOR XM_CALLCONV GetRight() const;
	XMVECTOR XM_CALLCONV GetScale() const { return Load(m_scale); }
	void XM_CALLCONV SetScale(FXMVECTOR scale) { Store(m_scale, scale); }

	XMMATRIX XM_CALLCONV GetWM()const { return XMMatrixScalingFromVector(GetScale())*Load(m_worldMatNoScale); }

	void SetPosition(float x, float y, float z);
	void XM_CALLCONV SetPosition(FXMVECTOR pos);

	void MoveRight(float distance = 1.0f);
	void MoveUp(float distance = 1.0f);
	void MoveFoward(float distance = 1.0f);
	void XM_CALLCONV Move(FXMVECTOR vel);

	void XM_CALLCONV RotateByAxis(const FXMVECTOR xmf3Axis, const float angle);
	void RotateByPYR(float pitch = 10.0f, float yaw = 10.0f, float roll = 10.0f);
	void UpdateBoundingBox();

public:
	virtual bool IsVisible(const Camera* = nullptr);

protected:
	XMFLOAT4X4	m_worldMatNoScale;
	XMFLOAT3 m_scale;
	vector<shared_ptr<Mesh>> m_meshes;
	shared_ptr<Shader>	m_shader;

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
	GameObject* GetBrother()const { return m_brother; }
	GameObject* GetChild()const { return m_child; }
	void SetBrother(GameObject* br) { m_brother = br; }
	void SetChild(GameObject* ch) { m_child = ch; }
protected:
	GameObject* m_brother{ nullptr };
	GameObject* m_child{ nullptr };
};

class InstancingObject : public GameObject
{
public:
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList*)final {}; // do not update constbuffer
};

class HeightMapTerrain : public GameObject
{
public:
	HeightMapTerrain(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* rootSignature, string_view fileName
		, int width, int length
		, int blockWidth, int blockLength
		, XMFLOAT3A scale, XMVECTORF32 color);
	virtual ~HeightMapTerrain();

public:
	float GetHeight(float x, float z)const { return mHeightMapImage->GetHeight(x / m_scale.x, z / m_scale.z) * m_scale.y; }
	XMVECTOR XM_CALLCONV GetNormal(float x, float z)const { return mHeightMapImage->GetNormal(int(x / m_scale.x), int(z / m_scale.z)); }
	
	int GetHeightMapWidth()const { return mHeightMapImage->GetWidth(); }
	int GetHeightMapLength()const { return mHeightMapImage->GetLength(); }
	
	XMFLOAT3A GetScale()const { return m_scale; }
	float GetWidth()const { return m_width * m_scale.x; }
	float GetLength()const { return m_length * m_scale.z; }
	
	virtual bool const IsVisible(Camera const* const camera = nullptr)final;

private:
	unique_ptr<HeightMapImage> mHeightMapImage;
	int m_width;
	int m_length;
	XMFLOAT3A m_scale;

};

class UIObject : public GameObject
{
public:
	UIObject(int meshes = 1);
	virtual ~UIObject();

	virtual void Animate(const milliseconds timeElapsed)override;

protected:

	virtual bool const IsVisible(Camera const* const camera = nullptr)final;

};