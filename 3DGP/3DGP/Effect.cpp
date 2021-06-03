#include "stdafx.h"
#include "Effect.h"
#include "GameFramework.h"

bool alive(PS_VB_EFFECT* a) { return a->mTime < a->mLifeTime; }
bool dead(PS_VB_EFFECT* a) { return !alive(a); }

Effect::Effect(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int n)
	: size{ n }
	, mcbMappedEffects{ new PS_VB_EFFECT[n] }
{

	CreateShaderVariables(device, commandList);
}

Effect::~Effect()
{

}

void Effect::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	const UINT buffeSize{ static_cast<UINT>(sizeof(PS_VB_EFFECT) * size) };
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
	commandList->SetGraphicsRootShaderResourceView(3, mcbEffects->GetGPUVirtualAddress());
	for (int i = 0; i < size; ++i) {
		mcbMappedEffects[i].mTime += timeE;
	}
}

void Effect::NewWallEffect(FXMVECTOR pos, float lifeTime)
{
	if (alive(&mcbMappedEffects[0]))return;
	mcbMappedEffects[0].mLifeTime = mcbMappedEffects[0].mTime + lifeTime;
	XMStoreFloat3(&mcbMappedEffects[0].mPosition, pos);
}
void Effect::NewObjEffect(FXMVECTOR pos, float lifeTime)
{
	mcbMappedEffects[1].mLifeTime = mcbMappedEffects[1].mTime + lifeTime;
	XMStoreFloat3(&mcbMappedEffects[1].mPosition, pos);
}


///////////////////////////////////////////////////