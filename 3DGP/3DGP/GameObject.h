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

	virtual void Animate(milliseconds timeElapsed);

	virtual void PrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera);

public:
	void Rotate(XMFLOAT3A* xmf3Axis, float angle);

protected:
	XMFLOAT4X4A	mxmf44World;
	Mesh*	mMesh;
	Shader*	mShader;

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
	void SetRotationAxis(XMFLOAT3A rotationAxis) { mxmf3RotationAxis = rotationAxis; }
	
	virtual void Animate(milliseconds timeElapsed);

};