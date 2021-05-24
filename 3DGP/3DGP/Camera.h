#pragma once

class Player;

enum class CAMERA_MODE
{
	  NO_CAMERA
	, FIRST_PERSON
	, SPACESHIP
	, THIRD_PERSON
};

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4A mViewMat;
	XMFLOAT4X4A mProjectionMat;
};

class Camera
{
public:
	Camera();
	Camera(Camera* camera);
	virtual ~Camera();
protected:
	void ProbationPrevCamera(Camera* camera);

public:

	virtual void CreateShaderVariables(
		  ID3D12Device* device
		, ID3D12GraphicsCommandList* commandlist);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commadList);

	void GenerateViewMatrix();
	void XM_CALLCONV GenerateViewMatrix(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR up);
	/* 회전으로인한 누적 실수 연산 오류 제거를 위해 ULR 직교화 작업 */
	void RegenerateViewMatrix();

	void GenerateProjectionMatrix(float fov, float aspect, float n, float f);

	void SetViewport(int xTopLeft, int yTopLeft, int width, int height
		, float minZ = 0.0f, float maxZ = 1.0f);
	void SetScissorRect(UINT xLeft, UINT yTop, UINT xRight, UINT yBottom);
	virtual void RSSetViewportScissorRect(ID3D12GraphicsCommandList* commandList);

public:
	void SetPlayer(Player* player)	{ mPlayer = make_shared<Player>(player); }
	Player* GetPlayer()const { return mPlayer.get(); }

	void SetMode(CAMERA_MODE mode) { mMode = mode; }
	CAMERA_MODE GetMode()const { return mMode; }

	void XM_CALLCONV SetPosition(FXMVECTOR position) { XMStoreFloat3A(&mPosition, position); }
	FXMVECTOR XM_CALLCONV GetPosition()const { return XMLoadFloat3A(&mPosition); }

	virtual void XM_CALLCONV SetLookAt(FXMVECTOR focusAt) { XMStoreFloat3A(&mLookAt, focusAt); }
	FXMVECTOR XM_CALLCONV GetLookAt()const { return XMLoadFloat3A(&mLookAt); }

	FXMVECTOR XM_CALLCONV GetRightVector()const { return XMLoadFloat3A(&mRightV); }
	FXMVECTOR XM_CALLCONV GetUpVector()const { return XMLoadFloat3A(&mUpV); }
	FXMVECTOR XM_CALLCONV GetLookVector()const { return XMLoadFloat3A(&mLookV); }

	float GetPitch()const { return mPitch; }
	float GetYaw()const { return mYaw; }
	float GetRoll()const { return mRoll; }
	
	void XM_CALLCONV SetOffset(FXMVECTOR offset) { XMStoreFloat3A(&mOffset, offset); }
	FXMVECTOR XM_CALLCONV GetOffset()const { return XMLoadFloat3A(&mOffset); }

	void SetTimeLag(milliseconds timeLag) { mTimeLag = timeLag; }
	milliseconds GetTimeLag()const { return mTimeLag; }

	FXMMATRIX XM_CALLCONV GetViewMatrix()const { return XMLoadFloat4x4A(&mViewMat); }
	FXMMATRIX XM_CALLCONV GetProjectionMatrix()const { return XMLoadFloat4x4A(&mProjectionMat); }
	D3D12_VIEWPORT GetViewPort()const { return mViewport; }
	D3D12_RECT GetScissorRect()const { return mScissorRect; }
	
	virtual void XM_CALLCONV Move(FXMVECTOR shift) { XMStoreFloat3A(&mPosition, XMLoadFloat3A(&mPosition) + shift); }
	virtual void Rotate(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f);

	void XM_CALLCONV RotateByMat(FXMMATRIX rotateMat) {
		XMStoreFloat3A(&mRightV, XMVector3TransformNormal(XMLoadFloat3A(&mRightV), rotateMat));
		XMStoreFloat3A(&mUpV, XMVector3TransformNormal(XMLoadFloat3A(&mUpV), rotateMat));
		XMStoreFloat3A(&mLookV, XMVector3TransformNormal(XMLoadFloat3A(&mLookV), rotateMat));
	}

protected:
	void XM_CALLCONV ModulatePosWhileRotate(FXMMATRIX rotateMat);
	
protected:
	XMFLOAT3A mPosition;

	// 축
	XMFLOAT3A mUpV;
	XMFLOAT3A mLookV;
	XMFLOAT3A mRightV;

	// 누적회전각
	float	mPitch;
	float	mYaw;
	float	mRoll;

	CAMERA_MODE	mMode;

	// 3인친용 소스
	XMFLOAT3A mLookAt;
	XMFLOAT3A mOffset;

	// 지연시간
	milliseconds mTimeLag;

	// 변환행렬
	XMFLOAT4X4A mViewMat;
	XMFLOAT4X4A mProjectionMat;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;

	shared_ptr<Player> mPlayer;
};


class SpaceShipCamera :public Camera
{
public:
	SpaceShipCamera(Camera* camera);
	virtual ~SpaceShipCamera(){}

	virtual void Rotate(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) override;

};

class FirstPersonCamera :public Camera
{
public:
	FirstPersonCamera(Camera* camera);
	virtual ~FirstPersonCamera(){}

	virtual void Rotate(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) override;
};

class ThirdPersonCamera :public Camera
{
public:
	ThirdPersonCamera(Camera* camera);
	virtual ~ThirdPersonCamera(){}
	virtual void XM_CALLCONV Update(FXMVECTOR lookAt, milliseconds timeElapsed);
	virtual void XM_CALLCONV SetLookAt(FXMVECTOR lookAt)override;
};