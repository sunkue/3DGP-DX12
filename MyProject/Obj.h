#pragma once

#include "pch.h"

/* 버텍스에 담을 정보들.. col 값은 사실상 단색 tex 로 교체 가능함.. */
struct Vertex
{
	XMFLOAT3		pos;
	XMFLOAT3		col;
	XMFLOAT3		nor;
	XMFLOAT3		tex;
};


/* 월드에 존재하는 모든 것들. 월드변환 행렬 */
__interface IWORLDMAT
{
	virtual XMMATRIX		WorldMat() abstract;
};
class WORLDOBJ abstract : public IWORLDMAT
{
private:
	XMFLOAT4X4		m_translateMat{ BASICMAT };
	XMFLOAT4X4		m_roatateMat{ BASICMAT };
	XMFLOAT4X4		m_scaleMat{ BASICMAT };
};




/* 파티클을 이루는 요소한개 */
class Particle
{
public:
	//Particle(vector<Vertex>&& vx, XMFLOAT3 vel = { 0.f,0.f,0.f }) :m_vertices{ vx }, m_velocity{ vel }, m_emitTime{}{}
	//Particle(vector<Vertex>&& vx, FXMVECTOR vel) :m_vertices{ vx }, m_emitTime{}{DirectX::XMStoreFloat3(&m_velocity, vel); }
	Particle(vector<Vertex>& vx, XMFLOAT3 vel = { 0.f,0.f,0.f }) :mp_vertices{ make_shared<vector<Vertex>>(vx) }, m_velocity{ vel }, m_emitTime{}{}
	Particle(vector<Vertex>& vx, FXMVECTOR vel) :mp_vertices{ make_shared<vector<Vertex>>(vx) }, m_emitTime{}{XMStoreFloat3(&m_velocity, vel); }

	float	LifeTime() { return static_cast<std::chrono::duration<float, std::milli>>(clk::now() - m_emitTime).count(); }
	void	born() { m_emitTime = clk::now(); }
private:
	shared_ptr<vector<Vertex>>	mp_vertices;
	XMFLOAT3		m_velocity;
	tp				m_emitTime;
};

/* 파티클 */
class Particles : public WORLDOBJ
{
public:
	Particles(vector<Particle>& ptcl, XMFLOAT3 vel = { 0.f,0.f,0.f }) :mp_particles{ make_shared<vector<Particle>>(ptcl) }, m_velocity{ vel }{}
	Particles(vector<Particle>& ptcl, FXMVECTOR vel) :mp_particles{ make_shared<vector<Particle>>(ptcl) } { XMStoreFloat3(&m_velocity, vel); }
	size_t			GetNumOfParticles() { return mp_particles->size(); }

	XMMATRIX		WorldMat() final;
private:
	shared_ptr<vector<Particle>>mp_particles;
	XMFLOAT3		m_velocity;
};




/* 오브젝트 부위 */
class ObjPart : public WORLDOBJ
{
public:
	//ObjPart(vector<Vertex>&& vx, XMFLOAT3 vel = { 0.f,0.f,0.f }) :m_vertices{ vx }, m_velocity{ vel }{}
	//ObjPart(vector<Vertex>&& vx, FXMVECTOR vel) :m_vertices{ vx } { DirectX::XMStoreFloat3(&m_velocity, vel); }
	ObjPart(vector<Vertex>& vx, XMFLOAT3 vel = { 0.f,0.f,0.f }) :mp_vertices{ make_shared<vector<Vertex>>(vx) }, m_velocity{ vel }{}
	ObjPart(vector<Vertex>& vx, FXMVECTOR vel) :mp_vertices{ make_shared<vector<Vertex>>(vx) } { XMStoreFloat3(&m_velocity, vel); }

	XMMATRIX		WorldMat() final;
private:
	shared_ptr<vector<Vertex>>	mp_vertices;
	index			m_texture;
	XMFLOAT3		m_velocity;
};

/* 부위 통합된 오브젝트 */
class Obj : public WORLDOBJ
{
public:
	//Obj(vector<ObjPart>&& objParts, XMFLOAT3 vel = { 0.f,0.f,0.f }) :m_objPart{ (objParts) }, m_velocity{ vel } {}
	//Obj(vector<ObjPart>&& objParts, FXMVECTOR vel) :m_objPart{ make_shared< (objParts) } { DirectX::XMStoreFloat3(&m_velocity, vel); }
	Obj(vector<ObjPart>& objParts, XMFLOAT3 vel = { 0.f,0.f,0.f }) :mp_objPart{ make_shared< vector<ObjPart>>(objParts) }, m_velocity{ vel } {}
	Obj(vector<ObjPart>& objParts, FXMVECTOR vel) :mp_objPart{ make_shared< vector<ObjPart>>(objParts) } { XMStoreFloat3(&m_velocity, vel); }

	XMMATRIX		WorldMat() final;
private:
	shared_ptr<vector<ObjPart>>	mp_objPart;
	XMFLOAT3		m_velocity;
};
