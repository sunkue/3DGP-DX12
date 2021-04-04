#pragma once

#include "pch.h"

/* 버텍스에 담을 정보들.. col 값은 사실상 단색 tex 로 교체 가능함.. */
struct Vertex
{
	XMFLOAT4		pos;	/* w = 1 */
	XMFLOAT3		col;
	XMFLOAT3		nor;
	XMFLOAT2		tex;
};


/* 월드에 존재하는 모든 것들. 월드변환 행렬 */
__interface IWORLDMAT
{
};
class WORLDOBJ abstract : public IWORLDMAT
{
public:
	virtual XMMATRIX XM_CALLCONV	WorldMat() const abstract; /* SRT */

	void XM_CALLCONV	SetTranslateMat(const FXMMATRIX mat) { XMStoreFloat4x4(&m_translateMat, mat); }
	void XM_CALLCONV	SetTranslateMat(const float x, const float y, const float z) { XMStoreFloat4x4(&m_translateMat, XMMatrixTranslation(x, y, z)); }
	void XM_CALLCONV	SetTranslateMat(const FXMVECTOR vec) { XMStoreFloat4x4(&m_translateMat, XMMatrixTranslationFromVector(vec)); }
	XMMATRIX			GetTranslateMat() const { return XMLoadFloat4x4(&m_translateMat); }
	XMVECTOR			GetTranslateVec() const { XMFLOAT3 translate{ m_translateMat._41, m_translateMat._42, m_translateMat._43 }; return XMLoadFloat3(&translate); }

	void XM_CALLCONV	SetRotateMat(const FXMMATRIX mat) { XMStoreFloat4x4(&m_roatateMat, mat); }
	void XM_CALLCONV	SetRotateMat(const FXMVECTOR axis, const float angle) { XMStoreFloat4x4(&m_roatateMat, XMMatrixRotationAxis(axis, angle)); }
	void XM_CALLCONV	SetRotateMatX(const float angle) { XMStoreFloat4x4(&m_roatateMat, XMMatrixRotationX(angle)); }
	void XM_CALLCONV	SetRotateMatY(const float angle) { XMStoreFloat4x4(&m_roatateMat, XMMatrixRotationY(angle)); }
	void XM_CALLCONV	SetRotateMatZ(const float angle) { XMStoreFloat4x4(&m_roatateMat, XMMatrixRotationZ(angle)); }
	XMMATRIX			GetRotateMat() const { return XMLoadFloat4x4(&m_roatateMat); };

	void XM_CALLCONV	SetScaleMat(const FXMMATRIX mat) { XMStoreFloat4x4(&m_scaleMat, mat); }
	void XM_CALLCONV	SetScaleMat(const float x, const float y, const float z) { XMStoreFloat4x4(&m_scaleMat, XMMatrixScaling(x, y, z)); }
	void XM_CALLCONV	SetScaleMat(const FXMVECTOR vec) { XMStoreFloat4x4(&m_scaleMat, XMMatrixScalingFromVector(vec)); }
	XMMATRIX			GetScaleMat() const { return XMLoadFloat4x4(&m_scaleMat); }
	XMVECTOR			GetScaleVec() const { XMFLOAT3 scale{ m_scaleMat._11, m_scaleMat._22, m_scaleMat._33 }; return XMLoadFloat3(&scale); }
private:
	XMFLOAT4X4		m_translateMat{ BASICMAT };
	XMFLOAT4X4		m_roatateMat{ BASICMAT };
	XMFLOAT4X4		m_scaleMat{ BASICMAT };
};

/* 속도를 가지는 모든 것들 <=> static */
class MOVEABLE
{
public:
	void XM_CALLCONV	SetVelocity(const FXMVECTOR vel) { XMStoreFloat3(&m_velocity, vel); }
	XMVECTOR			GetVelocity() const { return XMLoadFloat3(&m_velocity); }

	void XM_CALLCONV	SetTarget(const FXMVECTOR target) { XMStoreFloat4(&m_target, target); }
	XMVECTOR			GetTarget() const { return XMLoadFloat4(&m_target); }
private:
	XMFLOAT3		m_velocity;
	XMFLOAT4		m_target;	/* w = 1 */
};



/* 파티클을 이루는 요소한개 */
class Particle : public MOVEABLE
{
public:
	Particle(vector<Vertex>& vx, FXMVECTOR vel = { XMVectorZero() }) :mp_vertices{ make_shared<vector<Vertex>>(vx) }, m_emitTime{}{SetVelocity(vel); }

	float				LifeTime() const { return static_cast<std::chrono::duration<float, std::milli>>(clk::now() - m_emitTime).count(); }
	void				born() { m_emitTime = clk::now(); }
private:
	shared_ptr<vector<Vertex>>	mp_vertices;
	tp				m_emitTime;
};

/* 파티클 */
class Particles : public WORLDOBJ, public MOVEABLE
{
public:
	Particles(vector<Particle>& ptcl, FXMVECTOR vel = { XMVectorZero() }) :mp_particles{ make_shared<vector<Particle>>(ptcl) } { SetVelocity(vel); }

	size_t			GetNumOfParticles() const { return mp_particles->size(); }
	XMMATRIX XM_CALLCONV	WorldMat() const final;
private:
	shared_ptr<vector<Particle>>mp_particles;
};




/* 오브젝트 부위 */
class ObjPart : public WORLDOBJ, public MOVEABLE
{
public:
	ObjPart(vector<Vertex>& vx, FXMVECTOR vel = { XMVectorZero() }) :mp_vertices{ make_shared<vector<Vertex>>(vx) } { SetVelocity(vel); }

	XMMATRIX XM_CALLCONV	WorldMat() const final;
private:
	shared_ptr<vector<Vertex>>	mp_vertices;
	index			m_texture{ 0 };
};

/* 부위 통합된 오브젝트 */
class Obj : public WORLDOBJ, public MOVEABLE
{
public:
	Obj(vector<ObjPart>& objParts, FXMVECTOR vel = { XMVectorZero() }) :mp_objPart{ make_shared< vector<ObjPart>>(objParts) } { SetVelocity(vel); }

	XMMATRIX XM_CALLCONV	WorldMat() const final;
private:
	shared_ptr<vector<ObjPart>>	mp_objPart;
};
