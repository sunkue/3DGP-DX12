#include "stdafx.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Camera.h"

/// ////////////////////////////////////////

GameObject::GameObject(int meshes)
{
	Store(m_worldMatNoScale, XMMatrixIdentity());
	m_meshes.resize(meshes);
}

void GameObject::ReleaseUploadBuffers()
{
	ForFamily([](auto& obj) {obj->ReleaseUploadBuffers(); });
}

void GameObject::Animate(const float timeElapsed)
{
	ForFamily([=](auto& obj) {obj->Animate(timeElapsed); });
}

void GameObject::PrepareRender()
{

}

void GameObject::Render(ID3D12GraphicsCommandList* commandList, Camera* camera, UINT instanceCount)
{	
	if (false == IsVisible(camera))return;
	PrepareRender();
	for (auto& m : m_meshes)m->Render(commandList, instanceCount);
	ForFamily([=](auto& obj) {obj->Render(commandList, camera, instanceCount); });
}

void GameObject::RotateByAxis(FXMVECTOR axis, float angle)
{
	XMMATRIX rotate{ XMMatrixRotationAxis(axis,XMConvertToRadians(angle)) };
	RotateByMatrix(rotate);
}

void GameObject::RotateByPYR(float pitch, float yaw, float roll)
{
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(pitch), XMConvertToRadians(yaw), XMConvertToRadians(roll)) };
	RotateByMatrix(rotate);
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
	XMFLOAT4X4 world;
	Store(world, XMMatrixTranspose(GetWM()));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &world, 0);
}

void GameObject::ReleaseShaderVariables()
{

}

bool GameObject::IsVisible(const Camera* camera)
{
	if (nullptr == camera)return false;
	return camera->IsInFrustum(GetOOBB_transformed(Load(m_worldMatNoScale)));
}

//////////////////////////////
