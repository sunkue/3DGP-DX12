#include "stdafx.h"
#include "GameObject.h"
#include "Light.h"
#include "GameFramework.h"
#include "Shader.h"
#include "Scene.h"


LightObj::LightObj(ID3D12Device* device, ID3D12RootSignature* rootSignature, shared_ptr<LightInfo> light, int meshes)
	: GameObject{ meshes }
	, m_light{ light }
{
	assert(0 < m_light->m_attenuation.x
		&& 0 < m_light->m_attenuation.y
		&& 0 < m_light->m_attenuation.z);

	PlayerShader* shader{ new PlayerShader() };
	shader->CreateShader(device, rootSignature);
	SetShader(shader);
}

//////////////////////////////////

Light::Light(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
	: m_factorMode{ FACTOR_MODE::DEFAULT }
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
	commandList->SetGraphicsRoot32BitConstants(1, 1, &m_factorMode, 19);
	commandList->SetGraphicsRootShaderResourceView(5, mcbLights->GetGPUVirtualAddress());
	for (int i = 0; i < MaxLights; ++i) {
		if (i < Lights.size())mcbMappedLights[i] = *Lights[i];
		else mcbMappedLights[i] = LightInfo();
	}
	
}


/////////////////////////////////////////
Sun::Sun(ID3D12Device* device, ID3D12RootSignature* rootSignature, shared_ptr<LightInfo> light_info, int meshes, void* context)
	: LightObj{ device, rootSignature, light_info,meshes }
	, mSunUpdateContext{ context }
	, m_IsRevolving{ true }
{
	HeightMapTerrain const* const terrain{ reinterpret_cast<HeightMapTerrain*>(mSunUpdateContext) };
	float const xCenter{ terrain->GetWidth() * 0.5f };
	float const zCenter{ terrain->GetLength() * 0.5f };
	float const height{ terrain->GetHeight(xCenter,zCenter) };
	SetPosition({ xCenter, 2000.0f,zCenter });
}


void Sun::Animate(milliseconds timeElapsed)
{
	if (m_IsHandling)
	{
		const XMVECTOR offset{ 15.0f * GameFramework::GetApp()->GetPlayer()->GetLookVector() };
		SetPosition(GameFramework::GetApp()->GetPlayer()->GetPosition() + offset + XMVECTOR{ 0.0f, 9.0f, 0.0f });

		if (IsLButtonDown()) {
			m_IsHandling = false;
		}
		if (IsRButtonDown()) {
			m_IsHandling = false;
			m_IsRevolving = true;
			HeightMapTerrain const* const terrain{ reinterpret_cast<HeightMapTerrain*>(mSunUpdateContext) };
			float const xCenter{ terrain->GetWidth() * 0.5f };
			float const zCenter{ terrain->GetLength() * 0.5f };
			SetPosition({ xCenter,0.0f,-2248.72f });
			GameFramework::GetApp()->GetPlayer()->SetMaxScale(10.0f);
			GameFramework::GetApp()->GetPlayer()->SetPrevMesh(GameFramework::GetApp()->m_Meshes["cube"]);
			GameFramework::GetApp()->GetPlayer()->SetBrother(nullptr);
			GameFramework::GetApp()->GetPlayer()->SetChild(nullptr);
			GameFramework::GetApp()->GetPlayer()->SetScale({ 1.0f,1.0f,1.0f });
			GameFramework::GetApp()->GetPlayer()->GetCamera()->SetMinMaxOffset(-500.0f, -20.0f);
			SetScale({ 1.0f,1.0f,1.0f });
			Scene::SCENE->Trees();
		}
	}

	// Åº¼º¿îµ¿
	if (m_IsRevolving) {
		Revolve(timeElapsed);
	}


	if (mSunUpdateContext)SunUpdateCallback(timeElapsed);


	UpdateBoundingBox();
	ForFamily([=](auto& obj) {obj.Animate(timeElapsed); });
}

void Sun::Revolve(milliseconds timeElapsed)
{
	HeightMapTerrain const* const terrain{ reinterpret_cast<HeightMapTerrain*>(mSunUpdateContext) };
	float const xCenter{ terrain->GetWidth() * 0.5f };
	const float timeE{ timeElapsed.count() / 1000.0f };
	auto revolve = XMMatrixRotationAxis({ 1.0f,0.0f,0.0f}, XMConvertToRadians(360.0f / 8.0f * timeE));
	XMStoreFloat4x4A(&mWorldMat, XMLoadFloat4x4A(&mWorldMat) * revolve);
}

void Sun::SunUpdateCallback(milliseconds timeElapsed)
{
	XMVECTOR pos{ GetPosition() };
	float magicNum{ 2.0f * XMVectorGetY(GetScale()) };
	HeightMapTerrain const* const terrain{ reinterpret_cast<HeightMapTerrain*>(mSunUpdateContext) };
	float const terrainHeight{ terrain->GetHeight(XMVectorGetX(pos),XMVectorGetZ(pos)) + magicNum };
	float const SunHeight{ XMVectorGetY(pos) };

	if (SunHeight < terrainHeight) {
		if (SunHeight < 1.0f) {
			float const xCenter{ terrain->GetWidth() * 0.5f };
			float const zCenter{ terrain->GetLength() * 0.5f };
			SetPosition({ xCenter,0.0f,-2248.72f });
			m_IsRevolving = true;
		}
	}
}


void Sun::Catch()
{
	if (m_IsHandling)return;
	SetScale({ 1.0f, 1.0f, 1.0f });
	m_IsHandling = true;
	m_IsRevolving = false;
	Beep(500, 50);
}