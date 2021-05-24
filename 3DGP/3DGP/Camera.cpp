#include "stdafx.h"
#include "Player.h"
#include "Camera.h"

////////////////////////////
Camera::Camera()
	: mViewport{ 0,0,0,0.0f,0.0f }
	, mScissorRect{ 0,0,0,0 }
	, mPosition{ 0.0f,0.0f,0.0f }
	, mRightV{ 1.0f,0.0f,0.0f }
	, mLookV{ 0.0f,0.0f,1.0f }
	, mUpV{ 0.0f,1.0f,0.0f }
	, mPitch{ 0.0f }
	, mYaw{ 0.0f }
	, mRoll{ 0.0f }
	, mOffset{ 0.0f,0.0f,0.0f }
	, mTimeLag{ milliseconds::zero() }
	, mLookAt{ 0.0f,0.0f,0.0f }
	, mMode{ CAMERA_MODE::NO_CAMERA }
{
	XMStoreFloat4x4A(&mViewMat, XMMatrixIdentity());
	XMStoreFloat4x4A(&mProjectionMat, XMMatrixIdentity());
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

void Camera::ProbationPrevCamera(Camera* camera)
{
	if (nullptr == camera)return;
	if (camera->GetMode() == CAMERA_MODE::SPACESHIP)
	{
		mUpV = { 0.0f,1.0f,0.0f };
		mRightV.y = 0.0f;
		mLookV.y = 0.0f;
		XMStoreFloat3A(&mRightV, XMVector3Normalize(XMLoadFloat3A(&mRightV)));
		XMStoreFloat3A(&mLookV, XMVector3Normalize(XMLoadFloat3A(&mRightV)));
	}
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

void Camera::GenerateViewMatrix()
{
	XMStoreFloat4x4A(
		  &mViewMat
		, XMMatrixLookAtLH(XMLoadFloat3(&mPosition), XMLoadFloat3(&mLookAt), XMLoadFloat3(&mUpV)));
}

void Camera::GenerateViewMatrix(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR up)
{
	SetPosition(pos);
	SetLookAt(lookAt);
	XMStoreFloat3A(&mUpV, XMVector3Normalize(up));
	GenerateViewMatrix();
}

void Camera::RegenerateViewMatrix()
{
	XMVECTOR look{ XMVector3Normalize(GetLookVector()) };
	XMVECTOR up{ XMVector3Normalize(GetUpVector()) };
	XMVECTOR right{ XMVector3Normalize(XMVector3Cross(up, look)) };
	up = XMVector3Normalize(XMVector3Cross(look, right));
	XMStoreFloat3A(&mLookV, look);
	XMStoreFloat3A(&mRightV, right);
	XMStoreFloat3A(&mUpV, up);

	XMVECTOR pos{ XMLoadFloat3A(&mPosition) };
	mViewMat =
	{
		mRightV.x, mUpV.x, mLookV.x, mViewMat._14,
		mRightV.y, mUpV.y, mLookV.y, mViewMat._24,
		mRightV.z, mUpV.z, mLookV.z, mViewMat._34,
		
		-XMVectorGetX(XMVector3Dot(pos, right)),
		-XMVectorGetX(XMVector3Dot(pos, up)),
		-XMVectorGetX(XMVector3Dot(pos, look)),
		mViewMat._44
	};
}

void Camera::GenerateProjectionMatrix(float fov, float aspect, float n, float f)
{
	XMStoreFloat4x4A(&mProjectionMat, XMMatrixPerspectiveFovLH(fov, aspect, n, f));
}

void Camera::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	/* 다렉은 행우선, 셰이더는 열우선 행렬. Transpose해주어야 함. */
	XMFLOAT4X4A view;
	XMStoreFloat4x4A(&view, XMMatrixTranspose(XMLoadFloat4x4A(&mViewMat)));
	commandList->SetGraphicsRoot32BitConstants(1, 16, &view, 0);
	XMFLOAT4X4A proj;
	XMStoreFloat4x4A(&proj, XMMatrixTranspose(XMLoadFloat4x4A(&mProjectionMat)));
	commandList->SetGraphicsRoot32BitConstants(1, 16, &proj, 16);
}

void Camera::ReleaseShaderVariables()
{

}

void Camera::ModulatePosWhileRotate(FXMMATRIX rotateMat)
{
	XMVECTOR pos{ this->GetPosition() };
	XMVECTOR playerPos{ mPlayer->GetPosition() };
	pos = XMVectorSubtract(pos, playerPos);
	pos = XMVector3TransformCoord(pos, rotateMat);
	pos = XMVectorAdd(pos, playerPos);
	XMStoreFloat3A(&mPosition, pos);
}

//////////////////////////////////

SpaceShipCamera::SpaceShipCamera(Camera* camera) :Camera{ camera }
{
	mMode = CAMERA_MODE::SPACESHIP;
}

void SpaceShipCamera::Rotate(float x, float y, float z)
{
	if (!mPlayer)return;
	if (x != 0.0f)
	{
		XMVECTOR right{ mPlayer->GetRightVector() };
		XMMATRIX rotate{ XMMatrixRotationAxis(right,XMConvertToRadians(x)) };

		RotateByMat(rotate);
		ModulatePosWhileRotate(rotate);
	}
	if (y != 0.0f)
	{
		XMVECTOR up{ mPlayer->GetUpVector() };
		XMMATRIX rotate{ XMMatrixRotationAxis(up,XMConvertToRadians(y)) };

		RotateByMat(rotate);
		ModulatePosWhileRotate(rotate);
	}
	if (z != 0.0f)
	{
		XMVECTOR look{ mPlayer->GetLookVector() };
		XMMATRIX rotate{ XMMatrixRotationAxis(look,XMConvertToRadians(z)) };

		RotateByMat(rotate);
		ModulatePosWhileRotate(rotate);
	}
}

////////////////////////////////////////////////////

FirstPersonCamera::FirstPersonCamera(Camera* camera) :Camera{ camera }
{
	mMode = CAMERA_MODE::FIRST_PERSON;
	ProbationPrevCamera(camera);
}

void FirstPersonCamera::Rotate(float x, float y, float z)
{
	if (x != 0.0f)
	{
		XMVECTOR right{ this->GetRightVector() };
		XMMATRIX rotate{ XMMatrixRotationAxis(right,XMConvertToRadians(x)) };
		
		RotateByMat(rotate);
	}
	if (y != 0.0f)
	{
		XMVECTOR up{ mPlayer->GetUpVector() };
		XMMATRIX rotate{ XMMatrixRotationAxis(up,XMConvertToRadians(y)) };

		RotateByMat(rotate);
	}
	if (z != 0.0f)
	{
		XMVECTOR look{ mPlayer->GetLookVector() };
		XMMATRIX rotate{ XMMatrixRotationAxis(look,XMConvertToRadians(z)) };

		ModulatePosWhileRotate(rotate);

		RotateByMat(rotate);
	}
}

////////////////////////////////////////////////

ThirdPersonCamera::ThirdPersonCamera(Camera* camera) :Camera{ camera }
{
	mMode = CAMERA_MODE::THIRD_PERSON;
	ProbationPrevCamera(camera);
}

void ThirdPersonCamera::Update(FXMVECTOR lookAt, milliseconds timeElapsed)
{
	if (nullptr == mPlayer)return;
	XMMATRIX rotate{ XMMatrixIdentity() };
	XMVECTOR right{ mPlayer->GetRightVector() };
	XMVECTOR up{ mPlayer->GetUpVector() };
	XMVECTOR look{ mPlayer->GetLookVector() };
	rotate.r[0] = right;
	rotate.r[1] = up;
	rotate.r[2] = look;

	XMVECTOR offset{ XMVector3TransformCoord(XMLoadFloat3A(&mOffset), rotate) };
	XMVECTOR pos{ XMVectorAdd(mPlayer->GetPosition(),offset) };
	XMVECTOR dir{ XMVectorSubtract(pos,this->GetPosition()) };
	float length{ XMVectorGetX(XMVector3Length(dir)) };
	dir = XMVector3Normalize(dir);

	milliseconds timeLagScale = (mTimeLag == milliseconds::zero()) ? timeElapsed * (1s / mTimeLag) : 1000ms;
	float distance{ length * timeLagScale.count() / 1000 };
	if (length < distance)distance = length;
	if (length < 0.01f)distance = length;
	if (0 < distance) {
		SetPosition(XMVectorAdd(GetPosition(), dir * distance));
		SetLookAt(look);
	}
}

void ThirdPersonCamera::SetLookAt(FXMVECTOR lookAt)
{
	XMFLOAT4X4A lookAtMat;
	XMStoreFloat4x4A(&lookAtMat, XMMatrixLookAtLH(GetPosition(), GetLookAt(), mPlayer->GetUpVector()));
	mRightV = XMFLOAT3A{ lookAtMat._11,lookAtMat._21,lookAtMat._31 };
	mUpV = XMFLOAT3A{ lookAtMat._12,lookAtMat._22,lookAtMat._32 };
	mLookV = XMFLOAT3A{ lookAtMat._13,lookAtMat._23,lookAtMat._33 };
}
