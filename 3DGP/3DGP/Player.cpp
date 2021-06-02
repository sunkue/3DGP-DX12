#include "stdafx.h"
#include "Shader.h"
#include "GameFramework.h"
#include "Scene.h"
#include "Player.h"

Player* Player::PLAYER = nullptr;

Player::Player()
	: mPosition{ 0.0f,0.0f,0.0f }
	, mRightV{ 1.0f,0.0f,0.0 }
	, mUpV{ 0.0f,1.0f,0.0f }
	, mLookV{ 0.0f,0.0f,1.0f }
	, mVelocity{ 0.0f,0.0f,0.0f }
	, mGravity{ 0.0f,0.0f,0.0f }
	, mMaxVelocityZ{ 0.0f }
	, mMaxVelocityY{ 0.0f }
	, mFriction{ 0.0f }
	, mPitch{ 0.0f }
	, mYaw{ 0.0f }
	, mRoll{ 0.0f }
	, mPlayerUpdateContext{ nullptr }
	, mCameraUpdateContext{ nullptr }
	, mCamera{ nullptr }
	, mInvincible{ false }
	, mStealth{ false }
{
	PLAYER = this;
	Scene::SCENE->SetPlayer(this);
}

Player::~Player()
{
	ReleaseShaderVariables();
	delete mCamera;
}

void Player::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	GameObject::CreateShaderVariables(device, commandList);
	mCamera->CreateShaderVariables(device, commandList);
}

void Player::ReleaseShaderVariables()
{
	GameObject::ReleaseShaderVariables();
	mCamera->ReleaseShaderVariables();
}

void Player::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	GameObject::UpdateShaderVariables(commandList);
}

void Player::Move(BYTE direction, float distance, bool updateVelocity)
{
	if (0x00 == direction)return;
	XMVECTOR shift{ XMVectorZero() };
	constexpr XMVECTOR R{ 0.5f,0.0f,0.0f };
	if (direction & DIR_FORWARD	) shift += GetLookVector() * distance;
	if (direction & DIR_BACKWARD) shift -= GetLookVector() * distance;
	if (direction & DIR_RIGHT	) shift += R * distance;
	if (direction & DIR_LEFT	) shift -= R * distance;
	if (direction & DIR_UP		) shift += GetUpVector() * distance;
	if (direction & DIR_DOWN	) shift -= GetUpVector() * distance;
	Move(shift, updateVelocity);


	constexpr float MAX_ROLL = 30.f;
	constexpr float t = 3.0f;
	if (direction & DIR_RIGHT) {
		if (mRoll < MAX_ROLL) {
			mRoll += t;
			Rotate(0.f, t, 0.0f);
		}
	}
	if (direction & DIR_LEFT) {
		if (-MAX_ROLL < mRoll) {
			mRoll -= t;
			Rotate(0.f, -t, 0.0f);
		}
	}
}

void Player::Move(FXMVECTOR shift, bool updateVelocity)
{
	if (updateVelocity)
	{
		SetVelocity(GetVelocity() + shift);
	}
	else
	{
		XMStoreFloat3A(&mPosition, GetPosition() + shift);
		mCamera->Move(shift);
	}
}

void Player::Rotate(float x, float y, float z)
{
	switch (mCamera->GetMode())
	{
	case CAMERA_MODE::NO_CAMERA:
		break;
	case CAMERA_MODE::FIRST_PERSON: 
	case CAMERA_MODE::THIRD_PERSON: {
		constexpr float PitchLimit{ 89.99f };
		constexpr float YawLimit{ 360.0f };
		constexpr float RollLimit{ 20.0f };
		if (x != 0.0f) {
			mPitch += x;
			if (PitchLimit < mPitch) { x -= mPitch - PitchLimit; mPitch = PitchLimit; }
			else if (mPitch < -PitchLimit) { x -= mPitch + PitchLimit; mPitch = -PitchLimit; }
		}
		if (y != 0.0f) {
			mYaw += y;
			if (YawLimit < mYaw)mYaw -= YawLimit;
			else if (mYaw < 0.0f)mYaw += YawLimit;
		}
		if (z != 0.0f) {
			mRoll += z;
			if (RollLimit < mRoll) { z -= mRoll - RollLimit; mRoll = RollLimit; }
			else if (mRoll < RollLimit) { z -= mRoll + RollLimit; mRoll = -RollLimit; }
		}
		mCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX rotate{ XMMatrixRotationAxis(GetUpVector(), XMConvertToRadians(y)) };
			XMStoreFloat3A(&mLookV, XMVector3TransformNormal(GetLookVector(), rotate));
			XMStoreFloat3A(&mRightV, XMVector3TransformNormal(GetRightVector(), rotate));
		}
	}break;
	case CAMERA_MODE::SPACESHIP: {
		mCamera->Rotate(x, y, z);
		if (x != 0.0f) {
			XMMATRIX rotate{ XMMatrixRotationAxis(GetRightVector(),XMConvertToRadians(x)) };
			XMStoreFloat3A(&mLookV, XMVector3TransformNormal(GetLookVector(), rotate));
			XMStoreFloat3A(&mUpV, XMVector3TransformNormal(GetUpVector(), rotate));
		}
		if (y != 0.0f) {
			XMMATRIX rotate{ XMMatrixRotationAxis(GetUpVector(),XMConvertToRadians(y)) };
			XMStoreFloat3A(&mLookV, XMVector3TransformNormal(GetLookVector(), rotate));
			XMStoreFloat3A(&mRightV, XMVector3TransformNormal(GetRightVector(), rotate));
		}
		if (z != 0.0f) {
			XMMATRIX rotate{ XMMatrixRotationAxis(GetLookVector(),XMConvertToRadians(z)) };
			XMStoreFloat3A(&mUpV, XMVector3TransformNormal(GetUpVector(), rotate));
			XMStoreFloat3A(&mRightV, XMVector3TransformNormal(GetRightVector(), rotate));
		}
	}break;
	}
	XMVECTOR look{ XMVector3Normalize(GetLookVector()) };
	XMVECTOR up{ XMVector3Normalize(GetUpVector()) };
	XMVECTOR right{ XMVector3Normalize(XMVector3Cross(up, look)) };
	up = XMVector3Normalize(XMVector3Cross(look, right));
	XMStoreFloat3A(&mLookV, look);
	XMStoreFloat3A(&mUpV, up);
	XMStoreFloat3A(&mRightV, right);
}

void Player::Update(const milliseconds timeElapsed)
{
	const float timeE{ timeElapsed.count() / 1000.0f };
	XMVECTOR Gravity(XMLoadFloat3A(&mGravity));
	SetGravity(XMVectorAdd(Gravity, Gravity * 0.15f * timeE));
	SetVelocity(XMVectorAdd(GetVelocity(), XMLoadFloat3A(&mGravity)));
	float length{ sqrtf(mVelocity.z * mVelocity.z) };
	if (mMaxVelocityZ < length)
	{
		//mVelocity.x *= mMaxVelocityXZ / length;
		mVelocity.z *= mMaxVelocityZ / length;
	}
	length = sqrtf(mVelocity.y * mVelocity.y);
	if (mMaxVelocityY < length)
	{
		mVelocity.y *= mMaxVelocityY / length;
	}
	XMVECTOR vel{ GetVelocity() };
	Move(vel * timeE);
	if (mPlayerUpdateContext)PlayerUpdateCallback(timeElapsed);

	/* camera */
	CAMERA_MODE camMode{ mCamera->GetMode() };
	if (camMode == CAMERA_MODE::THIRD_PERSON) { mCamera->Update(GetPosition(), timeElapsed); }
	if (mCameraUpdateContext)CameraUpdateCallback(timeElapsed);
	if (camMode == CAMERA_MODE::THIRD_PERSON) { mCamera->SetLookAt(GetPosition()); }
	mCamera->RegenerateViewMatrix();

	length = XMVectorGetX(XMVector3Length(vel));
	float deceleration{ mFriction * timeE };
	if (length < deceleration)deceleration = length;
	SetVelocity(vel + XMVector3Normalize(vel * -deceleration));




	if (mStealth) {
		static float t{ 0.0f };
		constexpr float t2{ 0.01f };
		constexpr float MTP{ 0.5f };
		constexpr float MTP2{ 0.2f };
		static float time{ MTP };
		static float time2{ MTP2 };
		t += timeE;
		if (t < t2)mOptionColor = 0.25f;
		else if (time2 < MTP2/2.0f) {
			mOptionColor = 0.5f;
		}
		else {
			mOptionColor = 0.25f;
		}
		time -= timeE;
		time2 = (time2 < 0.0f) ? MTP2 : time2 - timeE;
		if (time < 0) {
			if (mInvincible) {
				time = MTP;
				return;
			}
			t = 0.0f; mStealth = false; time = MTP; mOptionColor = 1.0f;

		}
	}
	ReRoll(timeElapsed);
	UpdateBoundingBox();
}


void Player::ReRoll(milliseconds timeElapsed)
{
	constexpr float T = 300.0f;
	const float TimeE{ timeElapsed.count() / 1000.0f };
	const float t{ T * TimeE };
	if (mRoll < 0.0f) {
		mRoll += t;
		Rotate(0.0f, t, 0.f);
	}
	else if (0.0f < mRoll) {
		mRoll -= t;
		Rotate(0.0f, -t, 0.f);
	}
	else {
		/*
		constexpr XMVECTOR Z{ 0.0f,0.0f,1.0f };
		XMVECTOR look{ GetLookVector() };
		float D = XMVectorGetX(XMVector3Dot(look, Z) / (XMVector3Length(look) * XMVector3Length(Z)));
		Rotate(0.0f, D, 0.0f);
		*/
	}
}

Camera* Player::ChangeCamera(CAMERA_MODE newCameraMode, CAMERA_MODE currentCameraMode)
{
	Camera* newCamera{ nullptr };
	Camera* oldCamera{ GetCamera() };
	switch (newCameraMode)
	{
	case CAMERA_MODE::NO_CAMERA:
		assert(false);
		break;
	case CAMERA_MODE::FIRST_PERSON:
		newCamera = new FirstPersonCamera(oldCamera);
		break;
	case CAMERA_MODE::SPACESHIP:
		newCamera = new SpaceShipCamera(oldCamera);
		break;
	case CAMERA_MODE::THIRD_PERSON:
		newCamera = new ThirdPersonCamera(oldCamera);
		break;
	}
	if (currentCameraMode == CAMERA_MODE::SPACESHIP) {
		XMStoreFloat3A(&mRightV, XMVector3Normalize({ mRightV.x,0.0f,mRightV.z }));
		XMStoreFloat3A(&mUpV, XMVector3Normalize({ 0.0f,1.0f,0.0f }));
		XMStoreFloat3A(&mLookV, XMVector3Normalize({ mLookV.x,0.0f,mLookV.z }));

		mPitch = 0.0f;
		mRoll = 0.0f;
		mYaw = XMVectorGetX(XMVectorACos(XMVector3AngleBetweenNormals({ 0.0f,0.0f,1.0f }, GetLookVector())));
		if (mLookV.x < 0.0f)mYaw = -mYaw;
	}
	else if ((newCameraMode == CAMERA_MODE::SPACESHIP) && mCamera) {
		XMStoreFloat3A(&mRightV, mCamera->GetRightVector());
		XMStoreFloat3A(&mUpV, mCamera->GetUpVector());
		XMStoreFloat3A(&mLookV, mCamera->GetLookVector());
	}

	if (newCamera) {
		newCamera->SetMode(newCameraMode);
		newCamera->SetPlayer(this);
	}
	delete mCamera;
	return newCamera;
}

void Player::PrepareRender()
{
	mWorldMat =
	{
		mRightV.x, mRightV.y, mRightV.z , mWorldMat._14,
		mUpV.x	 , mUpV.y	, mUpV.z	, mWorldMat._24,
		mLookV.x , mLookV.y , mLookV.z	, mWorldMat._34,
		mPosition.x, mPosition.y, mPosition.z, mWorldMat._44
	};
}

void Player::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	CAMERA_MODE cameraMode{ (camera) ? (camera->GetMode()) : (CAMERA_MODE::NO_CAMERA) };
	if (cameraMode == CAMERA_MODE::THIRD_PERSON) {
		if (mShader)mShader->Render(commandList, camera);
		GameObject::Render(commandList, camera);
	}
}

////////////////////////////////////////////

AirPlanePlayer::AirPlanePlayer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* rootSignature)
{
	Mesh* airplaneMesh{ new AirplaneMeshDiffused(device,commandList) };
	SetMesh(1, airplaneMesh);
	SetCamera(ChangeCamera(CAMERA_MODE::THIRD_PERSON, milliseconds::zero()));
	CreateShaderVariables(device, commandList);
	SetPosition({ 0.0f,0.0f,-50.0f });
	PlayerShader* shader{ new PlayerShader() };
	shader->CreateShader(device, rootSignature);
	SetShader(shader);
	SetScale({ 0.5f,1.0f,1.0f });
}

AirPlanePlayer::~AirPlanePlayer()
{

}

void AirPlanePlayer::PrepareRender()
{
	Player::PrepareRender();
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f),0.0f,0.0f) };
	XMMATRIX scale{ XMMatrixScalingFromVector(GetScale()) };
	XMStoreFloat4x4A(&mWorldMat, scale * rotate * XMLoadFloat4x4A(&mWorldMat));
}

Camera* AirPlanePlayer::ChangeCamera(CAMERA_MODE newCameraMode, milliseconds timeElapsed)
{
	CAMERA_MODE currentCameraMode{ (mCamera) ? (mCamera->GetMode()) : (CAMERA_MODE::NO_CAMERA) };
	if (currentCameraMode == newCameraMode)return mCamera;
	GameFramework* app{ GameFramework::GetApp() };
	UINT W{ app->GetWidth() };
	UINT H{ app->GetHeight() };
	switch (newCameraMode)
	{
	case CAMERA_MODE::FIRST_PERSON: {
		SetFriction(200.0f);
		SetGravity({ 0.0f,0.0f,0.0f });
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		mCamera = Player::ChangeCamera(CAMERA_MODE::FIRST_PERSON, currentCameraMode);
		mCamera->SetTimeLag(milliseconds::zero());
		mCamera->SetOffset({ 0.0f,20.0f,0.0f });
		mCamera->GenerateProjectionMatrix(45.0f, app->GetAspectRatio(), 1.01f, 5000.0f);
		mCamera->SetViewport(0, 0, W, H);
		mCamera->SetScissorRect(0, 0, W, H);
	}break;
	case CAMERA_MODE::SPACESHIP: {
		SetFriction(125.0f);
		SetGravity({ 0.0f,0.0f,0.0f });
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		mCamera = Player::ChangeCamera(CAMERA_MODE::SPACESHIP, currentCameraMode);
		mCamera->SetTimeLag(milliseconds::zero());
		mCamera->SetOffset({ 0.0f,0.0f,0.0f });
		mCamera->GenerateProjectionMatrix(45.0f, app->GetAspectRatio(), 1.01f, 5000.0f);
		mCamera->SetViewport(0, 0, W, H);
		mCamera->SetScissorRect(0, 0, W, H);
	}break;
	case CAMERA_MODE::THIRD_PERSON: {
		SetFriction(250.0f);
		SetGravity({ 0.0f,0.0f,1.0f });
		SetMaxVelocityXZ(500.0f);
		SetMaxVelocityY(0.0f);
		mCamera = Player::ChangeCamera(CAMERA_MODE::THIRD_PERSON, currentCameraMode);
		mCamera->SetTimeLag(250ms);
		mCamera->SetOffset({ 0.0f,20.0f,-50.0f });
		mCamera->GenerateProjectionMatrix(45.0f, app->GetAspectRatio(), 1.01f, 5000.0f);
		mCamera->SetViewport(0, 0, W, H);
		mCamera->SetScissorRect(0, 0, W, H);
	}break;
	}
	mCamera->SetPosition(GetPosition() + mCamera->GetOffset());
	//Update(timeElapsed);

	return mCamera;
}


void AirPlanePlayer::Crash()
{
	SetGravity({ 0.0f,0.0f,1.0f });
	SetVelocity({ 0.0f,0.0f,5.0f });
	mStealth = true;
}

////////////////////////////////

TerrainPlayer::TerrainPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
	, ID3D12RootSignature* rootSignature, void* context, int meshes = 1)
{

}

TerrainPlayer::~TerrainPlayer()
{

}

Camera* TerrainPlayer::ChangeCamera(CAMERA_MODE newCameraMode, milliseconds timeElapsed)
{

}

void TerrainPlayer::PlayerUpdateCallback(milliseconds timeElapsed)
{

}

void TerrainPlayer::CameraUpdateCallback(milliseconds timeElapsed)
{

}
