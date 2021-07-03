#include "stdafx.h"

#include "Light.h"



LightObj::LightObj(ID3D12Device* device, ID3D12RootSignature* rootSignature, shared_ptr<SR_LIGHT_INFO> light, int meshes)
	: GameObject{ meshes }
	, m_light{ light }
{}

//////////////////////////////////

LightShader::LightShader(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	CreateShaderVariables(device, commandList);
}

void LightShader::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	const UINT buffeSize{ static_cast<UINT>(sizeof(SR_LIGHT_INFO) * MaxLights) };
	m_RSRC = CreateBufferResource(device, commandList, nullptr, buffeSize
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	m_RSRC->Map(0, nullptr, (void**)&m_Mapped_RSRC);
}

void LightShader::ReleaseShaderVariables()
{
	if (m_RSRC) {
		m_RSRC->Unmap(0, nullptr);
		m_RSRC.Reset();
	}
}

void LightShader::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetGraphicsRootShaderResourceView(m_RootParamIndex, m_RSRC->GetGPUVirtualAddress());
	for (int i = 0; i < MaxLights; ++i) {
		if (static_cast<size_t>(i) < m_Lights.size())m_Mapped_RSRC[i] = *m_Lights[i];
		else m_Mapped_RSRC[i] = SR_LIGHT_INFO();
	}
}

void LightShader::SortByDist(XMVECTOR point)
{
	sort(ALLOF(m_Lights), [point](const auto& a, const auto& b) {
		float aDist{ XMVectorGetX(XMVector3LengthEst(point - Load(a->m_position))) };
		float bDist{ XMVectorGetX(XMVector3LengthEst(point - Load(b->m_position))) };
		return aDist < bDist;
		});	
}