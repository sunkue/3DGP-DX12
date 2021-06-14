#include "stdafx.h"
#include "GameObject.h"
#include "Light.h"


LightObj::LightObj(shared_ptr<LightInfo> light, int meshes)
	: GameObject{ meshes }
	, m_light{ light }
{
	assert(0 < m_light->m_attenuation.x
		&& 0 < m_light->m_attenuation.y
		&& 0 < m_light->m_attenuation.z);
}

//////////////////////////////////

Light::Light(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	CreateShaderVariables(device, commandList);
}

Light::~Light()
{

}

void Light::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	const UINT buffeSize{ static_cast<UINT>(sizeof(LightInfo) * MaxLights) };
	mcbLights = CreateBufferResource(device, commandList, nullptr, buffeSize
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	mcbLights->Map(0, nullptr, (void**)&mcbMappedLights);
}

void Light::ReleaseShaderVariables()
{
	if (mcbLights) {
		mcbLights->Unmap(0, nullptr);
		mcbLights.Reset();
	}
}

void Light::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetGraphicsRootShaderResourceView(5, mcbLights->GetGPUVirtualAddress());
	for (int i = 0; i < MaxLights; ++i) {
		if (i < Lights.size())mcbMappedLights[i] = *Lights[i];
		else mcbMappedLights[i] = LightInfo();
	}
}
