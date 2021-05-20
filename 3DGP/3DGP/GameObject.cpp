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

void GameObject::RotateByAxis(const XMFLOAT3A* axis, const float angle)
{
	XMMATRIX rotate{ XMMatrixRotationAxis(XMLoadFloat3A(axis),XMConvertToRadians(angle)) };
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

void GameObject::SetPosition(XMFLOAT3A position)
{
	SetPosition(position.x, position.y, position.z);
}

XMFLOAT3A GameObject::GetPosition()	const
{
	XMFLOAT3A pos{ mxmf44World._41,mxmf44World._42,mxmf44World._43 };
	XMStoreFloat3A(&pos, XMVector3Normalize(XMLoadFloat3A(&pos)));
	return pos;
}

XMFLOAT3A GameObject::GetLook() const
{
	XMFLOAT3A look{ mxmf44World._31,mxmf44World._32,mxmf44World._33 };
	XMStoreFloat3A(&look, XMVector3Normalize(XMLoadFloat3A(&look)));
	return look;
}

XMFLOAT3A GameObject::GetUp() const
{
	XMFLOAT3A up{ mxmf44World._21,mxmf44World._22,mxmf44World._23 };
	XMStoreFloat3A(&up, XMVector3Normalize(XMLoadFloat3A(&up)));
	return up;
}

XMFLOAT3A GameObject::GetRight() const
{
	XMFLOAT3A right{ mxmf44World._21,mxmf44World._22,mxmf44World._23 };
	XMStoreFloat3A(&right, XMVector3Normalize(XMLoadFloat3A(&right)));
	return right;
}

void GameObject::MoveRight(const float distance)
{
	XMFLOAT3A pos = GetPosition();
	XMFLOAT3A right = GetRight();
	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&right) * distance);
	SetPosition(pos);
}

void GameObject::MoveUp(const float distance)
{
	XMFLOAT3A pos = GetPosition();
	XMFLOAT3A up = GetUp();
	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&up) * distance);
	SetPosition(pos);
}

void GameObject::MoveFoward(const float distance)
{
	XMFLOAT3A pos = GetPosition();
	XMFLOAT3A look = GetLook();
	XMStoreFloat3(&pos, XMLoadFloat3(&pos) + XMLoadFloat3(&look) * distance);
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
	mxmf3RotationAxis = XMFLOAT3A(0.0f, 1.0f, 0.0f);
	mRotationSpeed = 90.0f;
}

RotatingObject::~RotatingObject()
{

}

void RotatingObject::Animate(milliseconds timeElapsed)
{
	RotateByAxis(&mxmf3RotationAxis, mRotationSpeed * timeElapsed.count() / 1000.0f);
}

