#include "stdafx.h"
#include "GameFramework.h"
#include "GameObject.h"
#include "Player.h"
#include "Shader.h"


/// ////////////////////////////////////////

GameObject::GameObject()
	: mMesh{ nullptr }
	, mShader{ nullptr }
	, mScale{ 1.0f,1.0f,1.0f }
	, mReferences{ 0 }
	, mOptionColor{ 1.0f }
{
	XMStoreFloat4x4A(&mWorldMat, XMMatrixIdentity());
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
		mShader->UpdateShaderVariable(commandList, &mWorldMat);
		mShader->Render(commandList, camera);
	}
	if (mMesh) mMesh->Render(commandList);
}

void GameObject::Render(ID3D12GraphicsCommandList* commandList, Camera* camera, UINT instanceCount)
{
	PrepareRender();
	if (mMesh)mMesh->Render(commandList, instanceCount);
}


void GameObject::Render(
	  ID3D12GraphicsCommandList* commandList
	, Camera* camera
	, UINT instanceCount
	, D3D12_VERTEX_BUFFER_VIEW instancingBufferView)
{
	PrepareRender();
	assert(mMesh);
	mMesh->Render(commandList, instanceCount, instancingBufferView);
}

void XM_CALLCONV GameObject::RotateByAxis(FXMVECTOR axis, const float angle)
{
	XMMATRIX rotate{ XMMatrixRotationAxis(axis,XMConvertToRadians(angle)) };
	XMStoreFloat4x4A(&mWorldMat, rotate * XMLoadFloat4x4A(&mWorldMat));
}

void GameObject::RotateByPYR(float pitch, float yaw, float roll)
{
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) };
	XMStoreFloat4x4A(&mWorldMat, rotate * XMLoadFloat4x4A(&mWorldMat));
}

void GameObject::SetPosition(float x, float y, float z)
{
	mWorldMat._41 = x;
	mWorldMat._42 = y;
	mWorldMat._43 = z;
}

void GameObject::SetPosition(FXMVECTOR position)
{
	SetPosition(XMVectorGetX(position), XMVectorGetY(position), XMVectorGetZ(position));
}

XMVECTOR GameObject::GetPosition()	const
{
	XMVECTOR pos{ XMVectorSet(mWorldMat._41,mWorldMat._42,mWorldMat._43,1.0f) };
	return pos;
}

XMVECTOR GameObject::GetLook() const
{
	XMVECTOR look{ XMVectorSet(mWorldMat._31,mWorldMat._32,mWorldMat._33,0.0f) };
	return XMVector3Normalize(look);
}

XMVECTOR GameObject::GetUp() const
{
	XMVECTOR up{ XMVectorSet(mWorldMat._21,mWorldMat._22,mWorldMat._23,0.0f) };
	return XMVector3Normalize(up);
}

XMVECTOR GameObject::GetRight() const
{
	XMVECTOR right{ XMVectorSet(mWorldMat._21,mWorldMat._22,mWorldMat._23,0.0f) };
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

void GameObject::Move(FXMVECTOR vel)
{
	XMVECTOR pos = GetPosition();
	pos += vel;
	SetPosition(pos);
}

void GameObject::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{

}

void GameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMFLOAT4X4A world;
	XMStoreFloat4x4A(&world, XMMatrixTranspose(XMLoadFloat4x4A(&mWorldMat)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &world, 0);
	commandList->SetGraphicsRoot32BitConstants(0, 1, &mOptionColor, 16);
}

void GameObject::ReleaseShaderVariables()
{

}

void GameObject::UpdateBoundingBox()
{
	assert(mMesh);
	if (mMesh) {
		mMesh->mOOBB.Transform(mOOBB, GetWM());
		XMStoreFloat4(&mOOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&mOOBB.Orientation)));
	}

}

//////////////////////////////

EnemyObject::EnemyObject()
	: mRotationSpeed{ 90.0f }
	, mRotationAxis{ 0.0f, 1.0f, 0.0f }
	, mSpeed{ Rand() * 10.0f + 10.0f }
{
	XMStoreFloat3A(&mDir, { (1.0f - Rand() * 2.0f) * 20.0f * Rand(),0.0f,-0.1f });
}

void EnemyObject::Reset()
{
	SetPosition(Rand() * 50.0f, 0.0f
		, XMVectorGetZ(Player::PLAYER->GetPosition()) + EnemyObject::RESETZPOSITION);
}

EnemyObject::~EnemyObject()
{

}

void EnemyObject::Animate(milliseconds timeElapsed)
{
	const float timeE{ timeElapsed.count() / 1000.0f };

	RotateByAxis(XMLoadFloat3A(&mRotationAxis), mRotationSpeed * timeE);
	Move(XMLoadFloat3A(&mDir) * mSpeed * timeE);
	
	UpdateBoundingBox();
	
}

////////////////////
WallObject::WallObject()
{
	XMStoreFloat4x4A(&mWorldMat, XMMatrixScaling(0.5f, 2.2f, 500.0f) * XMLoadFloat4x4A(&mWorldMat));
}


void WallObject::Animate(milliseconds timeElapsed) {
	SetPosition(XMVectorGetX(GetPosition()), 0.0f, XMVectorGetZ(Player::PLAYER->GetPosition()));
	
	UpdateBoundingBox();
	
}