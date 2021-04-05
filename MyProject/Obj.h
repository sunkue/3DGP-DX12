#pragma once

#include "pch.h"

/* 버텍스에 담을 정보들.. col 값은 사실상 단색 tex 로 교체 가능함.. */
struct VERTEX
{
	XMFLOAT4A		pos;	/* w = 1 */
	XMCOLOR			col;
	XMFLOAT3A		nor;
	XMFLOAT2A		tex;
};

/* 평면 */
struct PLANE
{
	XMFLOAT3A		nor;
	float			distance;
};

/* 월드에 존재하는 모든 것들. 월드변환 행렬 */
class WORLDOBJ
{
public:
	XMMATRIX	WorldMat() const { return (GetScaleMat() * GetRotateMat() * GetTranslateMat()) * GetParentsWorldMat(); }

	void XM_CALLCONV	SetTranslateMat(const FXMMATRIX mat) { XMStoreFloat4x4A(&m_translateMat, mat); }
	void				SetTranslateMat(const float x, const float y, const float z) { XMStoreFloat4x4A(&m_translateMat, XMMatrixTranslation(x, y, z)); }
	void XM_CALLCONV	SetTranslateMat(const FXMVECTOR vec) { XMStoreFloat4x4A(&m_translateMat, XMMatrixTranslationFromVector(vec)); }
	XMMATRIX			GetTranslateMat() const { return XMLoadFloat4x4A(&m_translateMat); }
	XMVECTOR			GetTranslateVec() const { XMFLOAT3A translate{ m_translateMat._41, m_translateMat._42, m_translateMat._43 }; return XMLoadFloat3A(&translate); }

	void XM_CALLCONV	SetRotateMat(const FXMMATRIX mat) { XMStoreFloat4x4A(&m_roatateMat, mat); }
	void XM_CALLCONV	SetRotateMat(const FXMVECTOR axis, const float angle) { XMStoreFloat4x4A(&m_roatateMat, XMMatrixRotationAxis(axis, angle)); }
	void 				SetRotateMatX(const float angle) { XMStoreFloat4x4A(&m_roatateMat, XMMatrixRotationX(angle)); }
	void				SetRotateMatY(const float angle) { XMStoreFloat4x4A(&m_roatateMat, XMMatrixRotationY(angle)); }
	void				SetRotateMatZ(const float angle) { XMStoreFloat4x4A(&m_roatateMat, XMMatrixRotationZ(angle)); }
	XMMATRIX			GetRotateMat() const { return XMLoadFloat4x4A(&m_roatateMat); };

	void XM_CALLCONV	SetScaleMat(const FXMMATRIX mat) { XMStoreFloat4x4A(&m_scaleMat, mat); }
	void				SetScaleMat(const float x, const float y, const float z) { XMStoreFloat4x4A(&m_scaleMat, XMMatrixScaling(x, y, z)); }
	void XM_CALLCONV	SetScaleMat(const FXMVECTOR vec) { XMStoreFloat4x4A(&m_scaleMat, XMMatrixScalingFromVector(vec)); }
	XMMATRIX			GetScaleMat() const { return XMLoadFloat4x4A(&m_scaleMat); }
	XMVECTOR			GetScaleVec() const { XMFLOAT3A scale{ m_scaleMat._11, m_scaleMat._22, m_scaleMat._33 }; return XMLoadFloat3A(&scale); }

	void XM_CALLCONV	SetParentsWorldMat(const FXMMATRIX mat) { XMStoreFloat4x4A(&m_parentsWorldMat, mat); }
	void XM_CALLCONV	AddParentsWorldMat(const FXMMATRIX mat) { XMStoreFloat4x4A(&m_parentsWorldMat, GetBasicWorldMat() * mat); }
	void				ResetParentsWorldMat() { XMStoreFloat4x4A(&m_parentsWorldMat, XMLoadFloat4x4A(&m_basicWorldMat)); }
	void XM_CALLCONV	SetBasicWorldMat(const FXMMATRIX mat) { XMStoreFloat4x4A(&m_basicWorldMat, mat); }
private:
	XMMATRIX			GetBasicWorldMat()const { return XMLoadFloat4x4A(&m_basicWorldMat); }
	XMMATRIX			GetParentsWorldMat()const { return XMLoadFloat4x4A(&m_parentsWorldMat); }
private:
	XMFLOAT4X4A			m_translateMat{ BASICMAT };
	XMFLOAT4X4A			m_roatateMat{ BASICMAT };
	XMFLOAT4X4A			m_scaleMat{ BASICMAT };
	XMFLOAT4X4A			m_parentsWorldMat{ BASICMAT };		/* 계층 구조에서 부모의 월드변환행렬 */
	XMFLOAT4X4A			m_basicWorldMat{ BASICMAT };		/* 계층 구조에서 한단계 위 부모좌표계로의 변환행렬. ( P좌표 = C좌표 * X ) */
};

/* 속도를 가지는 모든 것들 <=> static */
class MOVEABLE
{
public:
	void XM_CALLCONV	SetVelocity(const FXMVECTOR vel) { XMStoreFloat3A(&m_velocity, vel); }
	XMVECTOR			GetVelocity() const { return XMLoadFloat3A(&m_velocity); }

	void XM_CALLCONV	SetTarget(const FXMVECTOR target) { XMStoreFloat4A(&m_target, target); }
	XMVECTOR			GetTarget() const { return XMLoadFloat4A(&m_target); }
private:
	XMFLOAT3A			m_velocity;
	XMFLOAT4A			m_target;	/* w = 1 */
};



/* 파티클을 이루는 요소한개 */
class PARTICLE : public MOVEABLE
{
public:
	PARTICLE(vector<VERTEX>& vx, FXMVECTOR vel = { XMVectorZero() }) :mp_vertices{ make_shared<vector<VERTEX>>(vx) }, m_emitTime{}{SetVelocity(vel); }

	float				LifeTime() const { return static_cast<std::chrono::duration<float, std::milli>>(clk::now() - m_emitTime).count(); }
	void				born() { m_emitTime = clk::now(); }
private:
	shared_ptr<vector<VERTEX>>	mp_vertices;
	tp							m_emitTime;
};

/* 파티클 */
class PARTICLES : public WORLDOBJ, public MOVEABLE
{
public:
	PARTICLES(vector<PARTICLE>& ptcl, FXMVECTOR vel = { XMVectorZero() }) :mp_particles{ make_shared<vector<PARTICLE>>(ptcl) } { SetVelocity(vel); }

	size_t			GetNumOfParticles() const { return mp_particles->size(); }
private:
	shared_ptr<vector<PARTICLE>>	mp_particles;
};




/* 오브젝트 */
class OBJ : public WORLDOBJ, public MOVEABLE
{
public:
	OBJ(vector<VERTEX>& vx, FXMVECTOR vel = { XMVectorZero() }) :mp_vertices{ make_shared<vector<VERTEX>>(vx) } { SetVelocity(vel); }

	void		CallMeBeforeCaculateChildensWM() { ResetChildrenWM(); Power2ChildrenWM(); }
private:
	void		Power2ChildrenWM();	/* 내 월드변환행렬을 자식새끼덜한테 강요하기 */
	void		ResetChildrenWM();	/* 자식새끼덜 기본좌표계 변환 리셋시키기 */

private:
	shared_ptr<vector<VERTEX>>		mp_vertices;
	index							m_texture{ 0 };

	vector<unique_ptr<OBJ>>			m_childrens;
	vector<unique_ptr<PARTICLES>>	m_particles;
};

