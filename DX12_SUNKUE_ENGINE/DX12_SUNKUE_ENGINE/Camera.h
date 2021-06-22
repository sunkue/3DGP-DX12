#pragma once

enum class CAMERA_MODE
{
	  observer
	, tomography
};

struct SR_CAMERA_INFO
{
	XMFLOAT4X4 viewMat;
	XMFLOAT4X4 projectionMat;
};

class Camera : public IShaderResourceHelper
{
public:
	Camera();
	virtual ~Camera() = default;

public:
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandlist)override;
	virtual void ReleaseShaderVariables()override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commadList)override;

	void XM_CALLCONV GenerateViewMatrix(FXMVECTOR pos, FXMVECTOR lookAt, FXMVECTOR up);
	void RegenerateViewMatrix();
	void GenerateProjectionMatrix(float fov, float aspect, float n, float f);
	void SetNF(float n, float f) { m_n = n; m_f = f; }
	void SetViewport(float xTopLeft, float yTopLeft, float width, float height, float minZ = 0.0f, float maxZ = 1.0f);
	void SetScissorRect(UINT xLeft, UINT yTop, UINT xRight, UINT yBottom);
	virtual void RSSetViewportScissorRect(ID3D12GraphicsCommandList* commandList);

public:
	void SetMode(CAMERA_MODE mode)	{ m_Mode = mode; }
	CAMERA_MODE GetMode() const		{ return m_Mode; }

	void XM_CALLCONV SetPosition(FXMVECTOR pos);
	XMVECTOR XM_CALLCONV GetPosition() const	{ return Load(m_Position); }

	void XM_CALLCONV SetLookAt(FXMVECTOR lookAt);
	XMVECTOR XM_CALLCONV GetLookAt() const { return Load(m_LookAt); }

	XMVECTOR XM_CALLCONV GetRightVector() const	{ return Load(m_RightV); }
	XMVECTOR XM_CALLCONV GetUpVector() const	{ return Load(m_UpV); }
	XMVECTOR XM_CALLCONV GetLookVector() const	{ return Load(m_LookV); }

	float GetPitch() const	{ return m_Pitch; }
	float GetYaw() const	{ return m_Yaw; }
	float GetRoll()	const	{ return m_Roll; }

	XMMATRIX XM_CALLCONV GetViewMatrix() const { return Load(m_Info.viewMat); }
	XMMATRIX XM_CALLCONV GetProjectionMatrix() const { return Load(m_Info.projectionMat); }
	D3D12_VIEWPORT GetViewPort() const	{ return m_Viewport; }
	D3D12_RECT GetScissorRect() const	{ return m_ScissorRect; }
	
	void XM_CALLCONV Move(FXMVECTOR shift) { Store(m_Position, Load(m_Position) + shift); }
	void Rotate(float pitch, float yaw, float roll) { RotateByMat(XMMatrixRotationRollPitchYaw(pitch, yaw, roll)); }
	void XM_CALLCONV RotateByMat(FXMMATRIX rotateMat);
	
	virtual void Update(float timeElapsed) {};
	
public:
	void GenerateFrustum();
	bool IsInFrustum(const BoundingOrientedBox& OOBB) const { return m_frustum.Intersects(OOBB); }

protected:
	SR_CAMERA_INFO m_Info;

	XMFLOAT3 m_Position{ 0.0f,0.0f,0.0f };

	XMFLOAT3 m_RightV{ xAxis };
	XMFLOAT3 m_UpV{ yAxis };
	XMFLOAT3 m_LookV{ zAxis };

	float	m_Pitch{};
	float	m_Yaw{};
	float	m_Roll{};

	float m_n{ 0 };
	float m_f{ FLT_MAX };

	CAMERA_MODE	m_Mode{ CAMERA_MODE::observer };

	XMFLOAT3 m_LookAt{ zAxis };

	D3D12_VIEWPORT m_Viewport;
	D3D12_RECT m_ScissorRect;

	BoundingFrustum m_frustum;
};



class Player;
class ThirdPersonCamera : public Camera
{
public:
	virtual void Update(float timeElapsed)override;

	void XM_CALLCONV SetOffset(FXMVECTOR offset) { Store(m_Offset, offset); }
	XMVECTOR XM_CALLCONV GetOffset() const { return Load(m_Offset); }
	void SetTimeLag(float timeLag) { assert(0 <= timeLag); m_TimeLag = timeLag; }
	float GetTimeLag() const { return m_TimeLag; }

private:
	float m_TimeLag{ 0.25f };
	XMFLOAT3 m_Offset{ 0,0,0 };
	shared_ptr<Player> m_Player;
};




template<size_t N>requires (1 < N) and (N < 5)
class BezierCamera : public Camera
{
public:
	BezierCamera() = default;
	virtual ~BezierCamera() = default;

	void OnBezier() { m_IsBezierOn = true; }
	void OffBezier() { m_IsBezierOn = false; }

	virtual void Update(float timeElapsed)override
	{
		if (m_IsBezierOn && false == m_BezierCurves.empty())
		{
			SetPosition(CaculateBezierCurves(m_BezierCurves, m_BezierCurves_t));
			m_BezierCurves_t += timeElapsed;
		}
	}

private:
	vector<array<XMFLOAT3, N>> m_BezierCurves;
	bool m_IsBezierOn{ false };
	float m_BezierCurves_t{ 0 };
};