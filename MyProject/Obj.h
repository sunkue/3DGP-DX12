#pragma once

#include<vector>
#include<chrono>

using clk = std::chrono::high_resolution_clock;
typedef unsigned int index;

using std::vector;

struct Vertex 
{
	float pos[3];
	float col[3];
	float nor[3];
	float tex[3];
};

class Particle
{
public:
	float lifeTime() { return static_cast<std::chrono::duration<float, std::milli>>(clk::now() - m_emitTime).count(); }
private:
	vector<Vertex>	m_vertices;
	float m_velocity[3];
	std::chrono::steady_clock::time_point m_emitTime;
};

class Particles
{
public:
	size_t GetNumOfParticles(){ return num; }
private:
	size_t num;
	vector<Particle> m_particles;
	float m_translateMat4x4[4][4];
	float m_roatateMat4x4[4][4];
	float m_scaleMat4x4[4][4];
};

class ObjPart
{
public:
	//float* m_worldMat4x4() { return m_scaleMat4x4 * m_roatateMat4x4 * m_translateMat4x4; }
	//다렉 매쓰 매트릭스로 바꿀 것임.
private:
	vector<Vertex>	m_vertices;
	float m_velocity[3];

	float m_translateMat4x4[4][4];
	float m_roatateMat4x4[4][4];
	float m_scaleMat4x4[4][4];

	index m_texture;
};

class Obj
{
public:
	//float* m_worldMat4x4(); //part 전부에 적용
private:
	vector<ObjPart>	m_objPart;
	float m_velocity[3];

	float m_translateMat4x4[4][4];
	float m_roatateMat4x4[4][4];
	float m_scaleMat4x4[4][4];
};
