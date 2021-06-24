#include "stdafx.h"
#include "Effect.h"
#include "GameFramework.h"

Effect::Effect(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, size_t n)
	: size{ n }
	, m_Mapped_RSRC{ new SR_EFFECT[n] }
{
	CreateShaderVariables(device, commandList);
}

void Effect::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	const UINT buffeSize{ static_cast<UINT>(sizeof(SR_EFFECT) * size) };
	m_RSRC = CreateBufferResource(device, commandList, nullptr, buffeSize
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	m_RSRC->Map(0, nullptr, (void**)&m_Mapped_RSRC);
}

void Effect::ReleaseShaderVariables()
{
	if (m_RSRC) {
		m_RSRC->Unmap(0, nullptr);
		m_RSRC.Reset();
	}
}

void Effect::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetGraphicsRootShaderResourceView(m_RootParamIndex, m_RSRC->GetGPUVirtualAddress());
	OnUpdateShaderVariables(commandList);
}


///////////////////////////////////////////////////