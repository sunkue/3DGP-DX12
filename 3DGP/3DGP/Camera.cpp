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
	, float minZ
	, float maxZ)
{
	mViewport.TopLeftX = static_cast<float>(xTopLeft);
	mViewport.TopLeftY = static_cast<float>(yTopLeft);
	mViewport.Width = static_cast<float>(width);
	mViewport.Height = static_cast<float>(height);
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

void Camera::RSSetViewportScissorRect(ID3D12GraphicsCommandList* commandList)
{
	commandList->RSSetViewports(1, &mViewport);
	commandList->RSSetScissorRects(1, &mScissorRect);
}

void Camera::CreateShaderVariables(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList)
{

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

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	XMFLOAT4X4A view;
	XMStoreFloat4x4A(&view, XMMatrixTranspose(XMLoadFloat4x4A(&mxmf44View)));
	commandList->SetGraphicsRoot32BitConstants(1, 16, &view, 0);
	XMFLOAT4X4A proj;
	XMStoreFloat4x4A(&proj, XMMatrixTranspose(XMLoadFloat4x4A(&mxmf44Projection)));
	commandList->SetGraphicsRoot32BitConstants(1, 16, &proj, 16);
}

void Camera::ReleaseShaderVariables()
{

}
