#include "stdafx.h"
#include "GameObject.h"
#include "Light.h"


LightObj::LightObj(shared_ptr<Light> light, int meshes)
	: GameObject{ meshes }
	, m_light{ light }
{
	assert(0 < m_light->m_attenuation.x
		&& 0 < m_light->m_attenuation.y
		&& 0 < m_light->m_attenuation.z);
}

void LightObj::Animate(const milliseconds timeElapsed)
{

}

void LightObj::PrepareRender()
{

}

void LightObj::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{

}

void LightObj::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{

}

void LightObj::ReleaseShaderVariables()
{

}

