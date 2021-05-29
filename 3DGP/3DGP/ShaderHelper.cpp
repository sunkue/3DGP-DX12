#include "stdafx.h"
#include "ShaderHelper.h"
#include "GameFramework.h"


EffectShader::EffectShader()
	: mcbMappedEffects{ nullptr }
	, mInstancingBufferView{}
{

}

EffectShader::~EffectShader()
{

}

void EffectShader::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	const UINT buffeSize{ static_cast<UINT>(sizeof(PS_VB_EFFECT) * mEffects.size()) };
	mcbEffects = CreateBufferResource(device, commandList, nullptr, buffeSize
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	mcbEffects->Map(0, nullptr, (void**)&mcbMappedEffects);
}

void EffectShader::ReleaseShaderVariables()
{
	if (mcbEffects) {
		mcbEffects->Unmap(0, nullptr);
		mcbEffects.Reset();
	}
}

void EffectShader::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	const float timeE{ GameFramework::GetApp()->GetTimer()->GetTimeElapsed().count() / 1000.0f };
	commandList->SetGraphicsRootShaderResourceView(2, mcbEffects->GetGPUVirtualAddress());
	for (int i = 0; i < mEffects.size(); ++i) {
		mcbMappedEffects[i].mTime += timeE;
	}
}

///////////////////////////////////////////////////