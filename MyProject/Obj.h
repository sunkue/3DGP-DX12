#pragma once

#include "pch.h"


struct Vertex
{
	m_vec3 pos;
	m_vec3 col;
	m_vec3 nor;
	m_vec3 tex;
};

class Particle
{
public:
	Particle(vector<Vertex>&& vx, m_vec3 vel = { 0.f,0.f,0.f }) :m_vertices{ move(vx) }, m_velocity{ vel }, m_emitTime{}{}
	Particle(vector<Vertex>&& vx, Fvec vel) :m_vertices{ move(vx) }, m_emitTime{}{V2M3(&m_velocity, vel); }

	float	lifeTime() { return static_cast<std::chrono::duration<float, std::milli>>(clk::now() - m_emitTime).count(); }
	void	born() { m_emitTime = clk::now(); }
private:
	vector<Vertex>	m_vertices;
	m_vec3			m_velocity;
	tp				m_emitTime;
	int f() {
		int a;
		vector<char>v;
		M2V2(v);
	}
};

class Particles
{
public:
	size_t GetNumOfParticles() { return m_particles.size(); }
private:
	vector<Particle> m_particles;
	float m_translateMat4x4[4][4];
	float m_roatateMat4x4[4][4];
	float m_scaleMat4x4[4][4];
};

class ObjPart
{
public:
	ObjPart(vector<Vertex>&& vx, m_vec3 vel = { 0.f,0.f,0.f }) :m_vertices{ move(vx) }, m_velocity{ vel }{}
	ObjPart(vector<Vertex>&& vx, Fvec vel) :m_vertices{ move(vx) } { V2M3(&m_velocity, vel); }

	//float* m_worldMat4x4() { return m_scaleMat4x4 * m_roatateMat4x4 * m_translateMat4x4; }
	//다렉 매쓰 매트릭스로 바꿀 것임.
private:
	vector<Vertex>	m_vertices;
	m_vec3 m_velocity;

	float m_translateMat4x4[4][4];
	float m_roatateMat4x4[4][4];
	float m_scaleMat4x4[4][4];

	index m_texture;
};

class Obj
{
public:
	Obj(vector<ObjPart>&& objParts, m_vec3 vel = { 0.f,0.f,0.f }) :m_objPart{ move(objParts) }, m_velocity{ vel } {}
	Obj(vector<ObjPart>&& objParts, Fvec vel) :m_objPart{ move(objParts) } { V2M3(&m_velocity, vel); }

	//float* m_worldMat4x4(); //part 전부에 적용
private:
	vector<ObjPart>	m_objPart;
	m_vec3 m_velocity;

	float m_translateMat4x4[4][4];
	float m_roatateMat4x4[4][4];
	float m_scaleMat4x4[4][4];
};
