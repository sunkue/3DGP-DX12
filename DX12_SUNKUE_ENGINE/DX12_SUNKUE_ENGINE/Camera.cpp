#include "stdafx.h"
#include "Camera.h"
//#include "Player.h"
////////////////////////////
Camera::Camera()
{
	Store(m_Info.viewMat, XMMatrixIdentity());
	Store(m_Info.projectionMat, XMMatrixIdentity());
	SetViewport(0.0f, 0.0f, 0.0f, 0.0f);
	SetScissorRect(0, 0, 0, 0);
}

void Camera::SetViewport(
	  float xTopLeft
	, float yTopLeft
	, float width
	, float height
	, float minZ
	, float maxZ)
{
	m_Viewport.TopLeftX = xTopLeft;
	m_Viewport.TopLeftY = yTopLeft;
	m_Viewport.Width = width;
	m_Viewport.Height = height;
	m_Viewport.MinDepth = minZ;
	m_Viewport.MaxDepth = maxZ;
}

void Camera::SetScissorRect(UINT xLeft, UINT yTop, UINT xRight, UINT yBottom)
{
	m_ScissorRect.left = xLeft;
	m_ScissorRect.top = yTop;
	m_ScissorRect.right = xRight;
	m_ScissorRect.bottom = yBottom;
}

void Camera::RSSetViewportScissorRect(ID3D12GraphicsCommandList* commandList)
{
	commandList->RSSetViewports(1, &m_Viewport);
	commandList->RSSetScissorRects(1, &m_ScissorRect);
}

void Camera::CreateShaderVariables(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList)
{

}

void Camera::GenerateViewMatrix(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR up)
{
	SetPosition(pos);
	SetLookAt(lookAt);
	Store(m_UpV, XMVector3Normalize(up));
	Store(m_Info.viewMat,XMMatrixLookAtLH(pos, lookAt, up));
}

/* after rotate */
void Camera::RegenerateViewMatrix()
{
	XMVECTOR up{ Load(m_UpV) };
	XMVECTOR look{ XMVector3Normalize(Load(m_LookV)) };
	XMVECTOR right{ XMVector3Normalize(XMVector3Cross(up, look)) };
	up = XMVector3Normalize(XMVector3Cross(look, right));
	Store(m_LookV, look);
	Store(m_RightV, right);
	Store(m_UpV, up);
	XMVECTOR pos{ Load(m_Position) };
	Store(m_Info.viewMat, XMMatrixLookAtLH(pos, pos + look, up));
	GenerateFrustum();
}

void Camera::GenerateProjectionMatrix(float fov, float aspect, float n, float f)
{
	Store(m_Info.projectionMat, XMMatrixPerspectiveFovLH(fov, aspect, n, f));
	m_n = n; m_f = f;
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMMATRIX VP = XMMatrixTranspose(GetViewMatrix() * GetProjectionMatrix());
	XMFLOAT4X4 viewProj; Store(viewProj, VP);
	commandList->SetGraphicsRoot32BitConstants(1, 16, &viewProj, 0);
	commandList->SetGraphicsRoot32BitConstants(1, 3, &m_Position, 16);
	//float v[2]{ m_Viewport.Width,m_Viewport.Height };
	//commandList->SetGraphicsRoot32BitConstants(1, 2, &v, 16);
}

void Camera::ReleaseShaderVariables()
{

}

void Camera::SetLookAt(FXMVECTOR lookAt)
{
	Store(m_LookAt, lookAt);
	Store(m_Info.viewMat, XMMatrixLookAtLH(GetPosition(), lookAt, GetUpVector()));
	m_RightV= { m_Info.viewMat._11,m_Info.viewMat._21,m_Info.viewMat._31 };
	m_UpV	= { m_Info.viewMat._12,m_Info.viewMat._22,m_Info.viewMat._32 };
	m_LookV = { m_Info.viewMat._13,m_Info.viewMat._23,m_Info.viewMat._33 };
	GenerateFrustum();
}
void Camera::SetPosition(FXMVECTOR pos) {
	Store(m_Position, pos); 
	Store(m_Info.viewMat, XMMatrixLookAtLH(pos, GetLookAt(), GetUpVector()));
}

void Camera::RotateByMat(FXMMATRIX rotateMat) {
	Store(m_RightV, XMVector3TransformNormal(Load(m_RightV),rotateMat));
	Store(m_UpV,	XMVector3TransformNormal(Load(m_UpV),	rotateMat));
	Store(m_LookV,	XMVector3TransformNormal(Load(m_LookV), rotateMat));
}

void Camera::GenerateFrustum()
{
	m_frustum.CreateFromMatrix(m_frustum, GetProjectionMatrix());
	XMMATRIX inversView{ XMMatrixInverse(nullptr, GetViewMatrix()) };
	m_frustum.Transform(m_frustum, inversView);
}

/// ////////////////////////////////////////
void ThirdPersonCamera::Update(float timeElapsed)
{
//	assert(m_Player);
//	XMVECTOR right{ m_Player->GetRightVector() };
//	XMVECTOR up{ m_Player->GetUpVector() };
//	XMVECTOR look{ m_Player->GetLookVector() };
//	XMMATRIX rotate{ right,up,look,g_XMIdentityR3 };
//	XMVECTOR offset{ XMVector3TransformCoord(Load(m_Offset), rotate) };
//	XMVECTOR pos{ XMVectorAdd(m_Player->GetPosition(),offset) };
//	XMVECTOR dir{ XMVectorSubtract(pos,this->GetPosition()) };
//	float length{ XMVectorGetX(XMVector3Length(dir)) };
//	dir = XMVector3Normalize(dir);
//
//	float timeLagScale = (0 != m_TimeLag) ? (timeElapsed / m_TimeLag) : (1);
//	float distance{ length * timeLagScale };
//	if (length < 0.01f || 1 < timeLagScale)distance = length;
//	if (0 < distance) {
//		SetPosition(XMVectorAdd(GetPosition(), dir * distance));
//		SetLookAt(look);
//	}
}

/// ////////////////////////////////////////