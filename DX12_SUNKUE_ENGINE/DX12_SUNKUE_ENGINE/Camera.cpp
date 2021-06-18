#include "stdafx.h"

#include "Camera.h"

////////////////////////////
Camera::Camera()
	: m_Viewport{ 0,0,0,0.0f,0.0f }
	, m_ScissorRect{ 0,0,0,0 }
	, m_Position{ 0.0f,0.0f,0.0f }
	, m_RightV{ 1.0f,0.0f,0.0f }
	, m_UpV{ 0.0f,1.0f,0.0f }
	, m_LookV{ 0.0f,0.0f,1.0f }
	, m_Pitch{ 0.0f }
	, m_Yaw{ 0.0f }
	, m_Roll{ 0.0f }
	, m_Offset{ 0.0f,0.0f,0.0f }
	, m_TimeLag{ milliseconds::zero() }
	, m_LookAt{ 0.0f,0.0f,0.0f }
	, m_Mode{ CAMERA_MODE::observer }
{
	XMStoreFloat4x4(&m_Info.viewMat, XMMatrixIdentity());
	XMStoreFloat4x4(&m_Info.projectionMat, XMMatrixIdentity());
}

Camera::Camera(Camera* camera)
{
	if (camera)
	{
		*this = *camera;
	}
	else
	{
		*this = Camera();
	}
}

Camera::~Camera()
{

}

////////////////////////////

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
	XMStoreFloat3A(&m_UpV, XMVector3Normalize(up));
}

void Camera::RegenerateViewMatrix()
{
	XMVECTOR look{ XMVector3Normalize(GetLookVector()) };
	XMVECTOR up{ XMVector3Normalize(GetUpVector()) };
	XMVECTOR right{ XMVector3Normalize(XMVector3Cross(up, look)) };
	up = XMVector3Normalize(XMVector3Cross(look, right));
	XMStoreFloat3A(&m_LookV, look);
	XMStoreFloat3A(&m_RightV, right);
	XMStoreFloat3A(&m_UpV, up);
	XMVECTOR pos{ XMLoadFloat3A(&m_Position) };
	m_Info.viewMat =
	{
		m_RightV.x, m_UpV.x, m_LookV.x, m_Info.viewMat._14,
		m_RightV.y, m_UpV.y, m_LookV.y, m_Info.viewMat._24,
		m_RightV.z, m_UpV.z, m_LookV.z, m_Info.viewMat._34,
		
		-XMVectorGetX(XMVector3Dot(pos, right)),
		-XMVectorGetX(XMVector3Dot(pos, up)),
		-XMVectorGetX(XMVector3Dot(pos, look)),
		m_Info.viewMat._44
	};
	GenerateFrustum();
}

void Camera::GenerateProjectionMatrix(float fov, float aspect, float n, float f)
{
	XMStoreFloat4x4(&m_Info.projectionMat, XMMatrixPerspectiveFovLH(fov, aspect, n, f));
	m_n = n;
	m_f = f;
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMMATRIX VP = XMMatrixTranspose(GetViewMatrix() * GetProjectionMatrix());
	XMFLOAT4X4A viewProj;
	XMStoreFloat4x4A(&viewProj, VP);
	commandList->SetGraphicsRoot32BitConstants(1, 16, &viewProj, 0);
	commandList->SetGraphicsRoot32BitConstants(1, 3, &m_Position, 16);
	float v[2]{ m_Viewport.Width,m_Viewport.Height };
	//commandList->SetGraphicsRoot32BitConstants(1, 2, &v, 16);
}

void Camera::ReleaseShaderVariables()
{

}

void Camera::GenerateFrustum()
{
	m_frustum.CreateFromMatrix(m_frustum, GetProjectionMatrix());
	XMMATRIX inversView{ XMMatrixInverse(nullptr, GetViewMatrix()) };
	m_frustum.Transform(m_frustum, inversView);
}
