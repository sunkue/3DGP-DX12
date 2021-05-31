#include "stdafx.h"
#include "Effect.h"
#include "GameFramework.h"

bool alive(PS_VB_EFFECT* a) { return a->mTime < a->mLifeTime; }
bool dead(PS_VB_EFFECT* a) { return !alive(a); }

Effect::Effect(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int n)
	: mcbMappedEffects{ nullptr }
	, mInstancingBufferView{}
{
	mEffects.resize(n);
	CreateShaderVariables(device, commandList);
}

Effect::~Effect()
{

}

void Effect::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	const UINT buffeSize{ static_cast<UINT>(sizeof(PS_VB_EFFECT) * mEffects.size()) };
	mcbEffects = CreateBufferResource(device, commandList, nullptr, buffeSize
		, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
	mcbEffects->Map(0, nullptr, (void**)&mcbMappedEffects);
}

void Effect::ReleaseShaderVariables()
{
	if (mcbEffects) {
		mcbEffects->Unmap(0, nullptr);
		mcbEffects.Reset();
	}
}

void Effect::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	const float timeE{ GameFramework::GetApp()->GetTimer()->GetTimeElapsed().count() / 1000.0f };
	commandList->SetGraphicsRootShaderResourceView(2, mcbEffects->GetGPUVirtualAddress());
	for (int i = 0; i < mEffects.size(); ++i) {
		mcbMappedEffects[i].mTime += timeE;
	}
}

void Effect::NewWallEffect(FXMVECTOR pos, float lifeTime)
{
	mEffects[0]->mLifeTime = mEffects[0]->mTime + lifeTime;
	XMStoreFloat3A(&mEffects[0]->mPosition, pos);
}
	


///////////////////////////////////////////////////