#include "stdafx.h"
#include "Shader.h"
#include "GameFramework.h"
#include "Scene.h"
#include "Player.h"

Player* Player::PLAYER = nullptr;
void Player::SetStaticPlayer(Player* p) {
	PLAYER = p;
	Scene::SCENE->SetPlayer(p);
}
Player::Player(int meshes)
	: GameObject{ meshes }
	, mPosition{ 0.0f,0.0f,0.0f }
	, mRightV{ 1.0f,0.0f,0.0 }
	, mUpV{ 0.0f,1.0f,0.0f }
	, mLookV{ 0.0f,0.0f,1.0f }
	, mVelocity{ 0.0f,0.0f,0.0f }
	, mGravity{ 0.0f,0.0f,0.0f }
	, mMaxVelocityXZ{ 0.0f }
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
	, mEffect{ nullptr }
	, m_flight{ false }
{
	SetStaticPlayer(this);
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
	if (direction & DIR_FORWARD	) shift += GetLookVector();
	if (direction & DIR_BACKWARD) shift -= GetLookVector();
	if (direction & DIR_RIGHT	) shift += GetRightVector();
	if (direction & DIR_LEFT	) shift -= GetRightVector();
	if (direction & DIR_UP		) shift += GetUpVector();
	if (direction & DIR_DOWN	) shift -= GetUpVector();
	if (direction & DIR_JUMP	) shift += yAxis;
	Move(XMVector3Normalize(shift) * distance, updateVelocity);
}

void Player::Jump(float height) {
	if (true == m_flight)return;
	float grav = XMVectorGetY(GetGravity());
	SetVelocity(XMVectorSetY(GetVelocity(), 0.0f));
	Move(DIR_JUMP, sqrtf(height * 2 * -grav), true);
}

void Player::Jump(milliseconds flightTime) {
	if (true == m_flight)return;
	float grav = XMVectorGetY(GetGravity());
	SetVelocity(XMVectorSetY(GetVelocity(), 0.0f));
	float timeE{ ToSec(flightTime) };
	Move(DIR_JUMP, timeE * -grav, true);
}

void Player::Move(FXMVECTOR shift, bool updateVelocity)
{
	if (updateVelocity)
	{
		SetVelocity(GetVelocity() + shift);
	}
	else
	{
		//cout << XMVectorGetY(GetPosition()) << " ";
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
		constexpr float PitchLimit{ 89.9f };
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
		if (y != 0.0f) {
			XMMATRIX rotate{ XMMatrixRotationAxis(GetUpVector(), XMConvertToRadians(y)) };
			XMStoreFloat3A(&mLookV, XMVector3TransformNormal(GetLookVector(), rotate));
			XMStoreFloat3A(&mRightV, XMVector3TransformNormal(GetRightVector(), rotate));
		}
		if (x != 0.0f) {
			XMMATRIX rotate{ XMMatrixRotationAxis(GetRightVector(),XMConvertToRadians(x)) };
			XMStoreFloat3A(&mLookV, XMVector3TransformNormal(GetLookVector(), rotate));
			//XMStoreFloat3A(&mUpV, XMVector3TransformNormal(GetUpVector(), rotate));
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
	//XMStoreFloat3A(&mUpV, up);
	XMStoreFloat3A(&mRightV, right);
}

void Player::Update(const milliseconds timeElapsed)
{
	assert(mPlayerUpdateContext);
	assert(mCameraUpdateContext);

	/* updateByVelocity */
	const float timeE{ timeElapsed.count() / 1000.0f };
	//XMVECTOR Gravity(XMLoadFloat3A(&mGravity));
	//SetGravity(XMVectorAdd(Gravity, Gravity * 0.15f * timeE));
	SetVelocity(XMVectorAdd(GetVelocity(), XMLoadFloat3A(&mGravity) * timeE));
	float length{ sqrtf(static_cast<float>(std::pow(mVelocity.x,2) + std::pow(mVelocity.z,2))) };
	if (mMaxVelocityXZ < length)
	{
		mVelocity.x *= mMaxVelocityXZ / length;
		mVelocity.z *= mMaxVelocityXZ / length;
	}
	length = sqrtf(mVelocity.y * mVelocity.y);
	if (mMaxVelocityY < length)
	{
		mVelocity.y *= mMaxVelocityY / length;
	}
	
	XMVECTOR vel{ GetVelocity() };
	Move(vel * timeE);
	if (mPlayerUpdateContext)PlayerUpdateCallback(timeElapsed);

	/* friction */
	float deceleration{ mFriction * timeE };
	//cout << deceleration<<" ";
	SetVelocity(vel - (vel * deceleration));

	/* camera */
	CAMERA_MODE camMode{ mCamera->GetMode() };
	if (camMode == CAMERA_MODE::THIRD_PERSON) { mCamera->Update(GetPosition(), timeElapsed); }
	if (mCameraUpdateContext)CameraUpdateCallback(timeElapsed);
	XMVECTOR add{ XMVectorZero() };
	if (IsRButtonDown())add = GetLookVector() * 100.0f;
	if (camMode == CAMERA_MODE::THIRD_PERSON) { mCamera->SetLookAt(GetPosition() + add); }
	mCamera->RegenerateViewMatrix();


	/*stealth*/
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
	UpdateBoundingBox();
}

void Player::Crash()
{
	//SetVelocity({ 0.0f,0.0f,0.0f });
}
void Player::AddScore(int score)
{
	float rtio{ 1.02f };
	rtio *= sqrtf((float)score);
	m_score += score;
	if(10.0f< XMVectorGetX(GetScale()))return;
	SetScale(GetScale() * rtio);
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
	XMVECTOR look{ XMVector3Normalize(GetLookVector()) };
	XMVECTOR up{ XMVector3Normalize(GetUpVector()) };
	XMVECTOR right{ XMVector3Normalize(XMVector3Cross(up, look)) };
	up = XMVector3Normalize(XMVector3Cross(look, right));
	XMVECTORF32 u;
	u.v = up;
	
	mWorldMat =
	{
		mRightV.x, mRightV.y, mRightV.z , mWorldMat._14,
		u.f[0]	 , u.f[1]	, u.f[2]	, mWorldMat._24,
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

AirPlanePlayer::AirPlanePlayer(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, ID3D12RootSignature* rootSignature
	, int meshes
)
	: Player{ meshes }
{
	Mesh* airplaneMesh{ new PlayerMeshDiffused(device,commandList) };
	SetMesh(0, airplaneMesh);
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

	return mCamera;
}


////////////////////////////////

TerrainPlayer::TerrainPlayer(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, ID3D12RootSignature* rootSignature
	, void* context
	, int meshes
)
	: Player{ meshes }
{
	mCamera = ChangeCamera(CAMERA_MODE::THIRD_PERSON, milliseconds::zero());
	PlayerMeshDiffused* cube{ new PlayerMeshDiffused(device,commandList,4.0f,4.0f,4.0f) };
	SetMesh(0, cube);
	SetStaticPlayer(this);

	const HeightMapTerrain* const terrain{ reinterpret_cast<HeightMapTerrain*>(context) };
	float const xCenter{ terrain->GetWidth() * 0.5f };
	float const zCenter{ terrain->GetLength() * 0.5f };
	float const height{ terrain->GetHeight(xCenter,zCenter) };
	SetPosition({ xCenter,height + 1500.0f,zCenter });	

	SetPlayerUpdateContext(context);
	SetCameraUpdateContext(context);

	PlayerShader* shader{ new PlayerShader() };
	shader->CreateShader(device, rootSignature);
	SetShader(shader);
	CreateShaderVariables(device, commandList);
}

TerrainPlayer::~TerrainPlayer()
{

}

Camera* TerrainPlayer::ChangeCamera(CAMERA_MODE newCameraMode, milliseconds timeElapsed)
{
	CAMERA_MODE currentCameraMode{ (mCamera) ? (mCamera->GetMode()) : (CAMERA_MODE::NO_CAMERA) };
	if (currentCameraMode == newCameraMode)return mCamera;
	GameFramework* app{ GameFramework::GetApp() };
	UINT W{ app->GetWidth() };
	UINT H{ app->GetHeight() };
	switch (newCameraMode)
	{
	case CAMERA_MODE::FIRST_PERSON: {
		SetFriction(2.5f);
		SetGravity({ 0.0f,-1000.0f,0.0f });
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		mCamera = Player::ChangeCamera(CAMERA_MODE::FIRST_PERSON, currentCameraMode);
		mCamera->SetTimeLag(milliseconds::zero());
		mCamera->SetOffset({ 0.0f,0.0f,0.0f });
		mCamera->SetViewport(0, 0, W, H);
		mCamera->SetScissorRect(0, 0, W, H);
		mCamera->GenerateProjectionMatrix(45.0f, app->GetAspectRatio(), 1.01f, 5000.0f);
	}break;
	case CAMERA_MODE::SPACESHIP: {
		assert(0);
	}break;
	case CAMERA_MODE::THIRD_PERSON: {
		SetFriction(2.5f);
		SetGravity({ 0.0f,-1000.0f,0.0f });
		SetMaxVelocityXZ(300.0f);
		SetMaxVelocityY(400.0f);
		mCamera = Player::ChangeCamera(CAMERA_MODE::THIRD_PERSON, currentCameraMode);
		mCamera->SetPosition(GetPosition() + mCamera->GetOffset());
		mCamera->SetTimeLag(250ms);
		mCamera->SetOffset({ 0.0f,20.0f,-50.0f });
		mCamera->SetViewport(0, 0, W, H);
		mCamera->SetScissorRect(0, 0, W, H);
		mCamera->GenerateProjectionMatrix(45.0f, app->GetAspectRatio(), 1.01f, 5000.0f);
	}break;
	}
	mCamera->SetPosition(GetPosition() + mCamera->GetOffset());

	return mCamera;
}

void TerrainPlayer::PlayerUpdateCallback(milliseconds timeElapsed)
{
	XMVECTOR pos{ GetPosition() };
	HeightMapTerrain const* const terrain{ reinterpret_cast<HeightMapTerrain*>(mPlayerUpdateContext) };
	float magicNum{ 2.0f*XMVectorGetY(GetScale()) };
	float const terrainHeight{ terrain->GetHeight(XMVectorGetX(pos),XMVectorGetZ(pos)) + magicNum };
	float const playeHeight{ XMVectorGetY(pos) };

	if (playeHeight < terrainHeight) {
		m_flight = false;
		pos = XMVectorSetY(pos, terrainHeight);
		SetPosition(pos);
	}
	else {
		m_flight = true;
	}
}

void TerrainPlayer::CameraUpdateCallback(milliseconds timeElapsed)
{
	XMVECTOR cameraPos{ mCamera->GetPosition() };
	HeightMapTerrain const* const terrain{ reinterpret_cast<HeightMapTerrain*>(mCameraUpdateContext) };
	constexpr float magicNum{ 5.0f };
	float const height{ terrain->GetHeight(XMVectorGetX(cameraPos),XMVectorGetZ(cameraPos)) + magicNum };
	if (XMVectorGetY(cameraPos) <= height) {
		cameraPos = XMVectorSetY(cameraPos, height);
		mCamera->SetPosition(cameraPos);
		if (CAMERA_MODE::THIRD_PERSON == mCamera->GetMode()) {
			ThirdPersonCamera* cameraHandle{ reinterpret_cast<ThirdPersonCamera*>(mCamera) };
			cameraHandle->SetLookAt(GetPosition());
		}
	}
}

