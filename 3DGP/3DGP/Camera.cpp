#include "stdafx.h"
#include "Camera.h"

////////////////////////////
Camera::Camera()
	: mViewport{ 0,0,222,222,0.0f,1.0f }
	, mScissorRect{ 0,0,222,222 }
{
	XMStoreFloat4x4A(&mxmf44View, XMMatrixIdentity());
	XMStoreFloat4x4A(&mxmf44Projection, XMMatrixIdentity());
}

Camera::~Camera()
{

}

////////////////////////////

void Camera::SetViewport(
	  int xTopLeft
	, int yTopLeft
	, int width
	, int height
	, float minZ = 0.0f
	, float maxZ = 1.0f)
{
	mViewport.TopLeftX = xTopLeft;
	mViewport.TopLeftY = yTopLeft;
	mViewport.Width = width;
	mViewport.Height = height;
	mViewport.MinDepth = minZ;
	mViewport.MaxDepth = maxZ;
}

void Camera::SetScissorRect(UINT xLeft, UINT yTop, UINT xRight, UINT yBottom)
{
	mScissorRect.left = xLeft;
	mScissorRect.top = yTop;
	mScissorRect.right = xRight;
	mScissorRect.bottom = yBottom;
}

void Camera::SetViewportScissorRect(ID3D12GraphicsCommandList* commandList)
{
	commandList->RSSetViewports(1, &mViewport);
	commandList->RSSetScissorRects(1, &mScissorRect);
}

void Camera::CreateShaderVariables(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandlist)
{
	XMFLOAT4X4A view;
	XMStoreFloat4x4A(&view, XMMatrixTranspose(XMLoadFloat4x4A(&mxmf44View)));
	commandlist->SetGraphicsRoot32BitConstants(1, 16, &view, 0);
	XMFLOAT4X4A proj;
	XMStoreFloat4x4A(&proj, XMMatrixTranspose(XMLoadFloat4x4A(&mxmf44Projection)));
	commandlist->SetGraphicsRoot32BitConstants(1, 16, &view, 16);
}

void Camera::GenerateViewMatrix(XMFLOAT3 pos, XMFLOAT3 lookAt, XMFLOAT3 up)
{
	XMStoreFloat4x4A(
		  &mxmf44View
		, XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&lookAt), XMLoadFloat3(&up)));
}

void Camera::GenerateProjectionMatrix(float fov, float aspect, float n, float f)
{
	XMStoreFloat4x4A(&mxmf44Projection, XMMatrixPerspectiveFovLH(fov, aspect, n, f));
}

void Camera::ReleaseShaderVariables()
{

}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* commadList)
{

}

