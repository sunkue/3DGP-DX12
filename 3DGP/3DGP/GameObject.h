#pragma once

#include "Mesh.h"

class Shader;
class Camera;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

public:
	void AddRef()	{ ++mReferences; assert(0 < mReferences); }
	void Release()	{ assert(0 < mReferences);  if (--mReferences == 0)delete this; }

private:
	int mReferences;

public:
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(Mesh* mesh);
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
	Mesh*	mMesh;
	Shader*	mShader;
	float mOptionColor;
};

class RotatingObject : public GameObject
{
public:
	RotatingObject();
	virtual ~RotatingObject();

private:
	XMFLOAT3A mxmf3RotationAxis;
	float mRotationSpeed;

public:
	void SetRotationSpeed(float rotationSpeed) { mRotationSpeed = rotationSpeed; }
	void XM_CALLCONV SetRotationAxis(FXMVECTOR rotationAxis) { XMStoreFloat3A(&mxmf3RotationAxis, rotationAxis); }
	
	virtual void Animate(milliseconds timeElapsed);

};