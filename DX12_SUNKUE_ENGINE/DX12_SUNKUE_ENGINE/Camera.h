#pragma once

class Player;

enum class CAMERA_MODE
{
	  observer
	, tomography
};

struct VS_CAMERA_INFO
{
	XMFLOAT4X4 viewMat;
	XMFLOAT4X4 projectionMat;
};

class Camera
{
public:
	Camera();
	Camera(Camera* camera);
	virtual ~Camera();

public:

	virtual void CreateShaderVariables(
		  ID3D12Device* device
		, ID3D12GraphicsCommandList* commandlist);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commadList);

	void XM_CALLCONV GenerateViewMatrix(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR up);
	/* 회전으로인한 누적 실수 연산 오류 제거를 위해 ULR 직교화 작업 */
	void RegenerateViewMatrix();
	void GenerateProjectionMatrix(float fov, float aspect, float n, float f);
	void SetNF(float n, float f) { m_n = n; m_f = f; }
	void SetViewport(float xTopLeft, float yTopLeft, float width, float height
		, float minZ = 0.0f, float maxZ = 1.0f);
	void SetScissorRect(UINT xLeft, UINT yTop, UINT xRight, UINT yBottom);
	virtual void RSSetViewportScissorRect(ID3D12GraphicsCommandList* commandList);


public:
	void SetMode(CAMERA_MODE mode) { m_Mode = mode; }
	CAMERA_MODE GetMode()const { return m_Mode; }

	void XM_CALLCONV SetPosition(FXMVECTOR position) { XMStoreFloat3A(&m_Position, position); }
	XMVECTOR XM_CALLCONV GetPosition()const { return XMLoadFloat3A(&m_Position); }

	virtual void XM_CALLCONV SetLookAt(FXMVECTOR lookAt) { XMStoreFloat3A(&m_LookAt, lookAt); }
	XMVECTOR XM_CALLCONV GetLookAt()const { return XMLoadFloat3A(&m_LookAt); }

	XMVECTOR XM_CALLCONV GetRightVector()const { return XMLoadFloat3A(&m_RightV); }
	XMVECTOR XM_CALLCONV GetUpVector()const { return XMLoadFloat3A(&m_UpV); }
	XMVECTOR XM_CALLCONV GetLookVector()const { return XMLoadFloat3A(&m_LookV); }

	float GetPitch()const { return m_Pitch; }
	float GetYaw()const { return m_Yaw; }
	float GetRoll()const { return m_Roll; }
	
	void XM_CALLCONV SetOffset(FXMVECTOR offset) { XMStoreFloat3A(&m_Offset, offset); }
	XMVECTOR XM_CALLCONV GetOffset()const { return XMLoadFloat3A(&m_Offset); }
	void SetTimeLag(milliseconds timeLag) { m_TimeLag = timeLag; }
	milliseconds GetTimeLag()const { return m_TimeLag; }

	XMMATRIX XM_CALLCONV GetViewMatrix()const { return XMLoadFloat4x4(&m_Info.viewMat); }
	XMMATRIX XM_CALLCONV GetProjectionMatrix()const { return XMLoadFloat4x4(&m_Info.projectionMat); }
	D3D12_VIEWPORT GetViewPort()const { return m_Viewport; }
	D3D12_RECT GetScissorRect()const { return m_ScissorRect; }
	
	virtual void XM_CALLCONV Move(FXMVECTOR shift) { XMStoreFloat3A(&m_Position, XMLoadFloat3A(&m_Position) + shift); }
	virtual void Rotate(float pitch = 0.0f, float yaw = 0.0f, float roll = 0.0f) {};
	virtual void XM_CALLCONV Update(XMVECTOR lookAt, milliseconds timeElapsed) {};

	void XM_CALLCONV RotateByMat(FXMMATRIX rotateMat) {
		XMStoreFloat3A(&m_RightV, XMVector3TransformNormal(XMLoadFloat3A(&m_RightV), rotateMat));
		XMStoreFloat3A(&m_UpV, XMVector3TransformNormal(XMLoadFloat3A(&m_UpV), rotateMat));
		XMStoreFloat3A(&m_LookV, XMVector3TransformNormal(XMLoadFloat3A(&m_LookV), rotateMat));
	}
	
public:
	void GenerateFrustum();
	bool IsInFrustum(BoundingOrientedBox const& OOBB)const {
		return m_frustum.Intersects(OOBB);
	}

protected:
	XMFLOAT3A m_Position;

	// 축
	XMFLOAT3A m_UpV;
	XMFLOAT3A m_LookV;
	XMFLOAT3A m_RightV;

	// 누적회전각
	float	m_Pitch;
	float	m_Yaw;
	float	m_Roll;


	float m_n;
	float m_f;

	CAMERA_MODE	m_Mode;

	XMFLOAT3A m_LookAt;
	XMFLOAT3A m_Offset;

	// 지연시간
	milliseconds m_TimeLag;

	// 변환행렬
	VS_CAMERA_INFO m_Info;

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_ScissorRect;

	BoundingFrustum m_frustum;
};