#pragma once

#include "Mesh.h"

class Shader;
class Camera;

class GameObject
{
public:
	GameObject(int meshCount = 1);
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
	XMMATRIX XM_CALLCONV GetWM()const { return XMLoadFloat4x4A(&mWorldMat); }

	void SetPosition(float x, float y, float z);
	void XM_CALLCONV SetPosition(FXMVECTOR position);

	void MoveRight(float distance = 1.0f);
	void MoveUp(float distance = 1.0f);
	void MoveFoward(float distance = 1.0f);

	void XM_CALLCONV RotateByAxis(const FXMVECTOR xmf3Axis, const float angle);
	void RotateByPYR(float pitch = 10.0f, float yaw = 10.0f, float roll = 10.0f);

protected:
	XMFLOAT4X4A	mWorldMat;
	vector<Mesh*> mMeshes;
	Shader*	mShader;

};

class RotatingObject : public GameObject
{
public:
	RotatingObject(int meshCount = 1);
	virtual ~RotatingObject();

private:
	XMFLOAT3A mxmf3RotationAxis;
	float mRotationSpeed;

public:
	void SetRotationSpeed(float rotationSpeed) { mRotationSpeed = rotationSpeed; }
	void XM_CALLCONV SetRotationAxis(FXMVECTOR rotationAxis) { XMStoreFloat3A(&mxmf3RotationAxis, rotationAxis); }
	
	virtual void Animate(milliseconds timeElapsed);

};

class HeightMapTerrain : public GameObject
{
public:
	HeightMapTerrain(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* rootSignature
		, LPCTSTR fileName, int width, int length, int blockWidth, int blockLength, XMFLOAT3A scale, XMFLOAT4A color);
	virtual ~HeightMapTerrain();

public:
	float GetHeight(float x, float z)const { return mHeightMap->GetHeight((x / mScale.x), (z / mScale.z)) * mScale.y; }
	XMVECTOR XM_CALLCONV GetNormal(float x, float z)const { return mHeightMap->GetHeightNormal(static_cast<int>(x / mScale.x), static_cast<int>(z / mScale.z)); }
	int GetHeightMapWidth()const { return mHeightMap->GetWidth(); }
	int GetHeightMapLength()const { return mHeightMap->GetLength(); }

	XMVECTOR GetScale()const { return XMLoadFloat3A(&mScale); }
	int GetWidth()const { return mWidth * mScale.x; }
	int GetLength()const { return mLength * mScale.z; }	

private:
	HeightMapImage* mHeightMap;
	int mWidth;
	int mLength;
	XMFLOAT3A mScale;

};