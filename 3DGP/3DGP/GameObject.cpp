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
	UpdateShaderVariables(commandList);
	if (mShader) {
		mShader->UpdateShaderVariable(commandList, &mxmf44World);
		mShader->Render(commandList, camera);
	}
	if (mMesh)mMesh->Render(commandList);
}

void XM_CALLCONV GameObject::RotateByAxis(FXMVECTOR axis, const float angle)
{
	XMMATRIX rotate{ XMMatrixRotationAxis(axis,XMConvertToRadians(angle)) };
	XMStoreFloat4x4A(&mxmf44World, rotate * XMLoadFloat4x4A(&mxmf44World));
}

void GameObject::RotateByPYR(float pitch, float yaw, float roll)
{
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) };
	XMStoreFloat4x4A(&mxmf44World, rotate * XMLoadFloat4x4A(&mxmf44World));
}

void GameObject::SetPosition(float x, float y, float z)
{
	mxmf44World._41 = x;
	mxmf44World._42 = y;
	mxmf44World._43 = z;
}

void GameObject::SetPosition(FXMVECTOR position)
{
	SetPosition(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
}

XMVECTOR GameObject::GetPosition()	const
{
	XMVECTOR pos{ XMVectorSet(mxmf44World._41,mxmf44World._42,mxmf44World._43,1.0f) };
	return XMVector3Normalize(pos);
}

XMVECTOR GameObject::GetLook() const
{
	XMVECTOR look{ XMVectorSet(mxmf44World._31,mxmf44World._32,mxmf44World._33,0.0f) };
	return XMVector3Normalize(look);
}

XMVECTOR GameObject::GetUp() const
{
	XMVECTOR up{ XMVectorSet(mxmf44World._21,mxmf44World._22,mxmf44World._23,0.0f) };
	return XMVector3Normalize(up);
}

XMVECTOR GameObject::GetRight() const
{
	XMVECTOR right{ XMVectorSet(mxmf44World._21,mxmf44World._22,mxmf44World._23,0.0f) };
	return XMVector3Normalize(right);
}

void GameObject::MoveRight(float distance)
{
	XMVECTOR pos = GetPosition();
	XMVECTOR right = GetRight();
	pos += right * distance;
	SetPosition(pos);
}

void GameObject::MoveUp(float distance)
{
	XMVECTOR pos = GetPosition();
	XMVECTOR up = GetUp();
	pos += up * distance;
	SetPosition(pos);
}

void GameObject::MoveFoward(float distance)
{
	XMVECTOR pos = GetPosition();
	XMVECTOR look = GetLook();
	pos += look * distance;
	SetPosition(pos);
}

void GameObject::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{

}

void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMFLOAT4X4A world;
	XMStoreFloat4x4A(&world, XMMatrixTranspose(XMLoadFloat4x4A(&mxmf44World)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &world, 0);
}

void GameObject::ReleaseShaderVariables()
{

}

//////////////////////////////

RotatingObject::RotatingObject()
{
	mxmf3RotationAxis = XMFLOAT3A{ 0.0f, 1.0f, 0.0f };
	mRotationSpeed = 90.0f;
}

RotatingObject::~RotatingObject()
{

}

void RotatingObject::Animate(milliseconds timeElapsed)
{
	RotateByAxis(XMLoadFloat3A(&mxmf3RotationAxis), mRotationSpeed * timeElapsed.count() / 1000.0f);
}

