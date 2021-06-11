#pragma once

#include "Mesh.h"

class Shader;
class Camera;

struct Meterial
{
	XMFLOAT4A m_ambient{ 0.3f,0.4f,0.4f,0.0f };
	XMFLOAT4A m_diffuse{ 0.7f,0.3f,0.3f,0.0f };
	XMFLOAT4A m_specular{ 1.0f,1.0f,1.0f,0.0f };
	XMFLOAT4A m_emessive{ 0.0f,0.0f,0.0f,0.0f };
	float m_specualrPower{ 0.0f };
};

class GameObject
{
public:
	GameObject(int meshes = 1);
	virtual ~GameObject();

public:
	void AddRef()	{ ++mReferences; assert(0 < mReferences); }
	void Release() { assert(0 < mReferences); /* if (--mReferences == 0); /*delete this*/; }

private:
	int mReferences;

public:
	void ReleaseUploadBuffers();
	
	void SetMesh(int index, Mesh* mesh);
	void SetShader(Shader* shader);

	virtual void Animate(const milliseconds timeElapsed);

	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera);
	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera, UINT instanceCount);
	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera
		, UINT instanceCount, D3D12_VERTEX_BUFFER_VIEW instancingBufferView);

protected:
	virtual void PrepareRender();
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);
	virtual void ReleaseShaderVariables();

public:
	vector<Mesh*> const& GetMesh() { return mMesh; }
	XMVECTOR XM_CALLCONV GetPosition()	const;
	XMVECTOR XM_CALLCONV GetLook() const;
	XMVECTOR XM_CALLCONV GetUp() const;
	XMVECTOR XM_CALLCONV GetRight() const;
	XMVECTOR XM_CALLCONV GetScale() const { return XMLoadFloat3A(&mScale); }
	void XM_CALLCONV SetScale(FXMVECTOR scale) { XMStoreFloat3A(&mScale, scale); }

	XMMATRIX XM_CALLCONV GetWM()const { return XMMatrixScalingFromVector(GetScale())*XMLoadFloat4x4A(&mWorldMat); }

	void SetPosition(float x, float y, float z);
	void XM_CALLCONV SetPosition(FXMVECTOR position);

	void MoveRight(float distance = 1.0f);
	void MoveUp(float distance = 1.0f);
	void MoveFoward(float distance = 1.0f);
	void XM_CALLCONV Move(FXMVECTOR vel);

	void XM_CALLCONV RotateByAxis(const FXMVECTOR xmf3Axis, const float angle);
	void RotateByPYR(float pitch = 10.0f, float yaw = 10.0f, float roll = 10.0f);
	void UpdateBoundingBox();

public:
	void SetMeterial(XMFLOAT4A a, XMFLOAT4A d, float sp = 0.0f, XMFLOAT4A s = { 1.0f,1.0f,1.0f,0.0f }) {
		m_meterial.m_ambient = a;
		m_meterial.m_diffuse = d;
		m_meterial.m_specular = s;
		m_meterial.m_specualrPower = sp;
	}
	void SetEmessive(XMFLOAT4A e) { m_meterial.m_emessive = e; }
	Meterial GetMeterial() { return m_meterial; }
protected:
	Meterial m_meterial;

public:
	BoundingOrientedBox const& GetOOBB()const { return mOOBB; }
	void SetOOBB(BoundingOrientedBox&& OOBB) { mOOBB = OOBB; }

protected:
	BoundingOrientedBox mOOBB;

public:
	virtual bool const IsVisible(Camera const* const camera = nullptr);

protected:
	XMFLOAT4X4A	mWorldMat;
	vector<Mesh*>	mMesh;
	Shader*	mShader;
	XMFLOAT3A mScale;

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
	/*
	 void f(int a, int b) {
        cout << x + a + b << "!\n";
        ForFamily([](A& obj, int AAA, int BBB) { obj.f(AAA, BBB); }, a, b);
	 }

	*/
protected:
	GameObject* m_brother;
	GameObject* m_child;
};

class EnemyObject : public GameObject
{
public:
	static constexpr float RESETZPOSITION{ 1500.0f };

public:
	EnemyObject(int meshes = 1);
	virtual ~EnemyObject();

	
	enum class TEAM { RED, GREEN, BLUE, YELLOW, ENDCOUNT };
	
private:
	XMFLOAT3A mRotationAxis;
	float mRotationSpeed;
	XMFLOAT3A mDir;
	float mSpeed;
	TEAM m_team{ EnemyObject::TEAM::ENDCOUNT };
	bool m_able;
public:
	bool const IsAble()const { return m_able; }
	void SetAbleState(bool able) { m_able = able; }
	TEAM const GetTeam()const { return m_team; }
	void SetTeam(TEAM t) { m_team = t; }

	void SetRotationSpeed(float rotationSpeed) { mRotationSpeed = rotationSpeed; }
	void XM_CALLCONV SetRotationAxis(FXMVECTOR rotationAxis) { XMStoreFloat3A(&mRotationAxis, rotationAxis); }

	XMVECTOR XM_CALLCONV GetDir() { return XMLoadFloat3A(&mDir); }
	void XM_CALLCONV SetDir(FXMVECTOR dir) { XMStoreFloat3A(&mDir, dir); }
	void SetRotateSpeed(float speed) { mRotationSpeed = speed; };
	virtual void Animate(milliseconds timeElapsed);
};

EnemyObject::TEAM& operator++(EnemyObject::TEAM& t);
EnemyObject::TEAM operator++(EnemyObject::TEAM& t, int);

class HeightMapTerrain : public GameObject
{
public:
	HeightMapTerrain(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* rootSignature, string_view fileName
		, int width, int length
		, int blockWidth, int blockLength
		, XMFLOAT3A scale, XMVECTORF32 color);
	virtual ~HeightMapTerrain();

public:
	float GetHeight(float x, float z)const { return mHeightMapImage->GetHeight(x / mScale.x, z / mScale.z) * mScale.y; }
	XMVECTOR XM_CALLCONV GetNormal(float x, float z)const { return mHeightMapImage->GetNormal(int(x / mScale.x), int(z / mScale.z)); }
	
	int GetHeightMapWidth()const { return mHeightMapImage->GetWidth(); }
	int GetHeightMapLength()const { return mHeightMapImage->GetLength(); }
	
	XMFLOAT3A GetScale()const { return mScale; }
	float GetWidth()const { return mWidth * mScale.x; }
	float GetLength()const { return mLength * mScale.z; }
	
	virtual bool const IsVisible(Camera const* const camera = nullptr)final;

private:
	unique_ptr<HeightMapImage> mHeightMapImage;
	int mWidth;
	int mLength;
	XMFLOAT3A mScale;

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