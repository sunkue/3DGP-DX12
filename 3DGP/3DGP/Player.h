#pragma once

#include "GameObject.h"
#include "Camera.h"

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT		0x04
#define DIR_RIGHT		0x08
#define DIR_UP			0x10
#define DIR_DOWN		0x20
#define DIR_JUMP		0x40

class Effect;

class Player : public GameObject
{
public:
	static Player* PLAYER;
public:
	static void SetStaticPlayer(Player* p);

public:
	Player(int meshes = 1);
	virtual ~Player();

	XMVECTOR XM_CALLCONV GetLookVector()const { return XMLoadFloat3A(&mLookV); }
	XMVECTOR XM_CALLCONV GetUpVector()const { return XMLoadFloat3A(&mUpV); }
	XMVECTOR XM_CALLCONV GetRightVector()const { return XMLoadFloat3A(&mRightV); }

	void SetFriction(float friction) { mFriction = friction; }
	void XM_CALLCONV SetGravity(FXMVECTOR gravity) { XMStoreFloat3A(&mGravity, gravity); }
	XMVECTOR XM_CALLCONV GetGravity()const { return XMLoadFloat3A(&mGravity); }
	void SetMaxVelocityXZ(float maxVelocity) { mMaxVelocityXZ = maxVelocity; }
	void SetMaxVelocityY(float maxVelocity) { mMaxVelocityY = maxVelocity; }
	void XM_CALLCONV SetVelocity(FXMVECTOR velocity) { XMStoreFloat3A(&mVelocity, velocity); }
	XMVECTOR XM_CALLCONV GetVelocity()const { return XMLoadFloat3A(&mVelocity); }
	void XM_CALLCONV SetPosition(FXMVECTOR position) { Move(position - GetPosition(), false); }	
	XMVECTOR XM_CALLCONV GetPosition()const { return XMLoadFloat3A(&mPosition); }

	float GetPitch()const { return mPitch; }
	float GetYaw()const { return mYaw; }
	float GetRoll()const { return mRoll; }

	Camera* GetCamera()const { return mCamera; }
	void SetCamera(Camera* camera) { mCamera = camera; }

	void Move(BYTE direction, float distance, bool updateVelocity = false);
	void XM_CALLCONV Move(FXMVECTOR shift, bool updateVelocity = false);
	void Jump(float height);
	void Jump(milliseconds flightTime);
	void Move(float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f);

	void Rotate(float x, float y, float z);
	void Update(milliseconds timeElapsed);

	virtual void PlayerUpdateCallback(milliseconds timeElapsed) { assert(false); }
	void SetPlayerUpdateContext(void* context) { mPlayerUpdateContext = context; }
	virtual void CameraUpdateCallback(milliseconds timeElapsed) { assert(false); }
	void SetCameraUpdateContext(void* context) { mCameraUpdateContext = context; }

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables()override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

	Camera* ChangeCamera(CAMERA_MODE newCameraMode, CAMERA_MODE currentCameraMode);
	virtual Camera* ChangeCamera(CAMERA_MODE newCameraMode, milliseconds timeElapsed) { return nullptr; }

	virtual void PrepareRender()override;
	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera = nullptr)override;

	bool Collable() { return mInvincible | mStealth; }
	void SetEffect(Effect* eff) { mEffect = eff; }
	void AddScore(int score = 1);
	void Crash();

protected:
	XMFLOAT3A mPosition;
	XMFLOAT3A mRightV;
	XMFLOAT3A mUpV;
	XMFLOAT3A mLookV;

	float mPitch;
	float mYaw;
	float mRoll;

	float m_score;

	XMFLOAT3A mVelocity;
	XMFLOAT3A mGravity;

	float mMaxVelocityXZ;
	float mMaxVelocityY;
	float mFriction;

	void* mPlayerUpdateContext;
	void* mCameraUpdateContext;

	Camera* mCamera;
	Effect* mEffect;
	bool mInvincible;
	bool mStealth;
	bool m_flight;
};

class AirPlanePlayer :public Player
{
public:
	AirPlanePlayer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12RootSignature* rootSignature, int meshes = 1);
	virtual ~AirPlanePlayer();

	virtual Camera* ChangeCamera(CAMERA_MODE newCameraMode, milliseconds timeElapsed);
	virtual void PrepareRender();

	
};

class TerrainPlayer : public Player
{
public:
	TerrainPlayer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
		, ID3D12RootSignature* rootSignature, void* context, int meshes = 1);
	virtual ~TerrainPlayer();

	virtual Camera* ChangeCamera(CAMERA_MODE newCameraMode, milliseconds timeElapsed);
	
	virtual void PlayerUpdateCallback(milliseconds timeElapsed);
	virtual void CameraUpdateCallback(milliseconds timeElapsed);
};