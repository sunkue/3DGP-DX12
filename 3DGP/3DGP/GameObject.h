#pragma once

#include "Mesh.h"

class Shader;
class Camera;

class GameObject
{
public:
	GameObject(int meshes = 1);
	virtual ~GameObject();

public:
	void AddRef()	{ ++mReferences; assert(0 < mReferences); }
	void Release()	{ assert(0 < mReferences);  if (--mReferences == 0)delete this; }

private:
	int mReferences;

public:
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(int index, Mesh* mesh);
	virtual void SetShader(Shader* shader);

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
	XMVECTOR XM_CALLCONV GetPosition()	const;
	XMVECTOR XM_CALLCONV GetLook() const;
	XMVECTOR XM_CALLCONV GetUp() const;
	XMVECTOR XM_CALLCONV GetRight() const;
	XMVECTOR XM_CALLCONV GetScale() const { return XMLoadFloat3A(&mScale); }
	void XM_CALLCONV SetScale(FXMVECTOR scale) { return XMStoreFloat3A(&mScale, scale); }

	XMMATRIX XM_CALLCONV GetWM()const { return XMLoadFloat4x4A(&mWorldMat); }

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
	BoundingOrientedBox const& GetOOBB()const { return mOOBB; }
	void SetOOBB(BoundingOrientedBox&& OOBB) { mOOBB = OOBB; }

public:
	bool const IsVisible(Camera const* const camera = nullptr);

protected:
	XMFLOAT4X4A	mWorldMat;
	vector<Mesh*>	mMesh;
	Shader*	mShader;
	float mOptionColor;
	XMFLOAT3A mScale;

	BoundingOrientedBox mOOBB;
};


class EnemyObject : public GameObject
{
public:
	static constexpr float RESETZPOSITION{ 1500.0f };

public:
	EnemyObject(int meshes = 1);
	virtual ~EnemyObject();

private:
	XMFLOAT3A mRotationAxis;
	float mRotationSpeed;
	XMFLOAT3A mDir;
	float mSpeed;

public:
	void SetRotationSpeed(float rotationSpeed) { mRotationSpeed = rotationSpeed; }
	void XM_CALLCONV SetRotationAxis(FXMVECTOR rotationAxis) { XMStoreFloat3A(&mRotationAxis, rotationAxis); }

	XMVECTOR XM_CALLCONV GetDir() { return XMLoadFloat3A(&mDir); }
	void XM_CALLCONV SetDir(FXMVECTOR dir) { XMStoreFloat3A(&mDir, dir); }
	virtual void Animate(milliseconds timeElapsed);
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
	float GetHeight(float x, float z)const { return mHeightMapImage->GetHeight(x / mScale.x, z / mScale.z) * mScale.y; }
	XMVECTOR XM_CALLCONV GetNormal(float x, float z)const { return mHeightMapImage->GetNormal(int(x / mScale.x), int(z / mScale.z)); }
	
	int GetHeightMapWidth()const { return mHeightMapImage->GetWidth(); }
	int GetHeightMapLength()const { return mHeightMapImage->GetLength(); }
	
	XMFLOAT3A GetScale()const { return mScale; }
	float GetWidth()const { return mWidth * mScale.x; }
	float GetLength()const { return mLength * mScale.z; }

private:
	unique_ptr<HeightMapImage> mHeightMapImage;
	int mWidth;
	int mLength;
	XMFLOAT3A mScale;

};