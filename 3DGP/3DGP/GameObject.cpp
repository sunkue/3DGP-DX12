#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"


/// ////////////////////////////////////////

GameObject::GameObject()
	: mMesh{ nullptr }
	, mShader{ nullptr }
	, mReferences{ 0 }
{
	XMStoreFloat4x4A(&mxmf44World, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	SAFE_RELEASE(mMesh);
	if (mShader) {
		mShader->ReleaseShaderVariables();
		mShader->Release();
	}
}

/// ////////////////////////////////////////

void GameObject::SetMesh(Mesh* mesh)
{
	SAFE_RELEASE(mMesh);
	mMesh = mesh;
	SAFE_ADDREF(mesh);
}

void GameObject::SetShader(Shader* shader)
{
	SAFE_RELEASE(mShader);
	mShader = shader;
	SAFE_ADDREF(shader);
}

void GameObject::ReleaseUploadBuffers()
{
	if (mMesh)mMesh->ReleaseUploadBuffers();
}


/// ////////////////////////////////////////

void GameObject::Animate(milliseconds timeElapsed)
{

}

void GameObject::PrepareRender()
{

}

void GameObject::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	PrepareRender();
	if (mShader) {
		mShader->UpdateShaderVariables(commandList);
		mShader->Render(commandList, camera);
	}
	if (mMesh)mMesh->Render(commandList);
}

void GameObject::Rotate(XMFLOAT3A* axis, float angle)
{
	XMMATRIX rotate{ XMMatrixRotationAxis(XMLoadFloat3A(axis),XMConvertToRadians(angle)) };
	XMStoreFloat4x4A(&mxmf44World, XMLoadFloat4x4A(&mxmf44World) * rotate);
}

//////////////////////////////

RotatingObject::RotatingObject()
{
	mxmf3RotationAxis = XMFLOAT3A(0.0f, 1.0f, 0.0f);
	mRotationSpeed = 90.0f;
}

RotatingObject::~RotatingObject()
{
}

void RotatingObject::Animate(milliseconds timeElapsed)
{
	GameObject::Rotate(&mxmf3RotationAxis, mRotationSpeed * timeElapsed.count() / 1000.0f);
}

