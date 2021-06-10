#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"
#include "GameFramework.h"
#include "Player.h"
#include "Effect.h"

Scene* Scene::SCENE = nullptr;

Scene::Scene()
	: mGraphicsRootSignature{ nullptr }
	, mEffect{ nullptr }
	, mPlayer{ nullptr }
	, mTerrain{ nullptr }
{
	SCENE = this;
}

Scene::~Scene()
{

}

ID3D12RootSignature* Scene::CreateGraphicsRootSignature(ID3D12Device* device)
{
	ID3D12RootSignature* GraphicsRootSignature{ nullptr };
	HRESULT hResult;
	CD3DX12_ROOT_PARAMETER RootParameters[5];
	/* 앞쪽이 접근속도가 빠름 */
	CD3DX12_ROOT_PARAMETER::InitAsConstants(
		  RootParameters[0]
		, 16
		, 0						//b0 Player
		, 0
		, D3D12_SHADER_VISIBILITY_ALL);
	
	CD3DX12_ROOT_PARAMETER::InitAsConstants(
		  RootParameters[1]
		, 34
		, 1						//b1 Camera
		, 0
		, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_ROOT_PARAMETER::InitAsShaderResourceView(
		  RootParameters[2]
		, 0						//t0 instance objescts
		, 0
		, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_ROOT_PARAMETER::InitAsShaderResourceView(
		  RootParameters[3]
		, 1						//t1 Effect
		, 0
		, D3D12_SHADER_VISIBILITY_ALL);
	
	CD3DX12_ROOT_PARAMETER::InitAsShaderResourceView(
		RootParameters[4]
		, 2						//t2 UI
		, 0
		, D3D12_SHADER_VISIBILITY_ALL);

	D3D12_ROOT_SIGNATURE_FLAGS RSFlags{
		  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT 
	};
	CD3DX12_ROOT_SIGNATURE_DESC RSDesc{
		   _countof(RootParameters)
		, RootParameters
		, 0
		, nullptr
		, RSFlags };
	ComPtr<ID3DBlob> SignatureBlob;
	ComPtr<ID3DBlob> ErrorBlob;
	hResult = D3D12SerializeRootSignature(
		  &RSDesc
		, D3D_ROOT_SIGNATURE_VERSION_1
		, SignatureBlob.GetAddressOf()
		, ErrorBlob.GetAddressOf());
	OutputErrorMessage(ErrorBlob.Get(), cout);
	ThrowIfFailed(hResult);

	ThrowIfFailed(device->CreateRootSignature(
	  	  0
		, SignatureBlob->GetBufferPointer()
		, SignatureBlob->GetBufferSize()
		, IID_PPV_ARGS(&GraphicsRootSignature)));
	return GraphicsRootSignature;
}

void Scene::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	mGraphicsRootSignature = CreateGraphicsRootSignature(device);

	XMFLOAT3A scale{ 8.0f,2.0f,8.0f };
	XMVECTORF32 color{ 0.0f,0.2f,0.0f,0.0f };


#ifdef WITH_TERRAIN_PARITION
	mTerrain = new HeightMapTerrain(
		device, commandList
		, mGraphicsRootSignature.Get()
		, "Assets/Image/Terrain/heightMap.raw"sv
		, 257, 257, 17, 17, scale, color);
#else
	mTerrain = new HeightMapTerrain(
		device, commandList
		, mGraphicsRootSignature.Get()
		, "Assets/Image/Terrain/heightMap.raw"sv
		, 257, 257, 257, 257, scale, color);
#endif

	assert(mShaders.empty());
	mShaders.push_back(new InstancingShader);
	assert(mShaders.size() == 1);
	mShaders[0]->CreateShader(device, mGraphicsRootSignature.Get());
	mShaders[0]->BuildObjects(device, commandList, mTerrain);
	
	mShaders.push_back(new UIShader);
	mShaders[1]->CreateShader(device, mGraphicsRootSignature.Get());
	mShaders[1]->BuildObjects(device, commandList, nullptr);

};

void Scene::ReleaseObjects()
{
	mGraphicsRootSignature.Reset();
	for (auto& shader : mShaders)
	{
		shader->ReleaseShaderVariables();
		shader->ReleaseObjects();
	}
	delete mTerrain;
};

void Scene::ReleaseUploadBuffers()
{
	for (auto& shader : mShaders)shader->ReleaseUploadBuffers();
	if (mTerrain)mTerrain->ReleaseUploadBuffers();
}

///////////////////////////////////////////////////

bool Scene::OnProcessingMouseMessage(
	  HWND hWnd
	, UINT messageID
	, WPARAM wParam
	, LPARAM lParam
)
{
	return false;
};

bool Scene::OnProcessingKeyboardMessage(
	  HWND hWnd
	, UINT messageID
	, WPARAM wParam
	, LPARAM lParam
)
{
	return false;
};

bool Scene::ProcessInput()
{
	return false;
};

void Scene::AnimateObjects(milliseconds timeElapsed)
{
	const float timeE{ timeElapsed.count() / 1000.0f };
	for (auto& shader : mShaders)shader->AnimateObjects(timeElapsed);
	CheckCollision(timeElapsed);

	if (mPlayer->IsEvolving()) {
		XMVECTOR playerPos{ mPlayer->GetPosition() };
		float const MaxDist{ 100.0f*sqrtf(mPlayer->GetScale().m128_f32[0]) };
		for (auto& obj : mObjects) {
			XMVECTOR const dir{ playerPos- obj->GetPosition() };
			float distance{ XMVectorGetX(XMVector3Length(dir)) };
			if (distance < MaxDist) {
				obj->Move(dir * (MaxDist / distance/2.0f) * timeE);
			}
		}
	}

};

void Scene::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	assert(camera);
	camera->RSSetViewportScissorRect(commandList);
	commandList->SetGraphicsRootSignature(mGraphicsRootSignature.Get());
	camera->UpdateShaderVariables(commandList);
	GameFramework::GetApp()->UpdateShaderVariables(commandList);

	for (auto& shader : mShaders)shader->Render(commandList, camera);
	//mShaders[0]->Render(commandList, camera);

	assert(mTerrain);
	if (mTerrain)mTerrain->Render(commandList, camera);
};

void Scene::CheckCollision(const milliseconds timeElapsed)
{
	const float timeE{ timeElapsed.count() / 1000.0f };
	// obj player
	if (!mPlayer->Collable()) {
		for (const auto& obj : mObjects) {
			if (false == obj->IsAble())continue;
			if (obj->GetOOBB().Intersects(mPlayer->GetOOBB())) {
				mEffect->NewObjEffect(mPlayer->GetPosition(), 0.5f);
				mPlayer->Crash();
				if (obj->GetTeam() == EnemyObject::TEAM::ENDCOUNT) {
					obj->SetScale(XMVectorZero());
					obj->SetAbleState(false);
					mPlayer->AddScore();

				}
				else if (2.5f < XMVectorGetX(mPlayer->GetScale())) {
					auto rr = reinterpret_cast<EnemyObject*>(obj);
					TeamAddCount(rr->GetTeam(), -1);
					rr->SetTeam(EnemyObject::TEAM::ENDCOUNT);
				}
				else mPlayer->Jump(1500ms);
			}
		}
	}
}

pair<bool,XMVECTOR> Scene::RayCollapsePos(const FXMVECTOR origin, const FXMVECTOR direction, float dist)
{
	// obj player
	vector<GameObject*> temp;
	for (const auto& obj : mObjects) {
		if (false == obj->IsAble())continue;
		if (obj->GetOOBB().Intersects(origin, direction, dist)) {
			temp.push_back(obj);
		}
	}
	if (mPlayer->GetOOBB().Intersects(origin, direction, dist)) {
		//temp.push_back(mPlayer);
	}

	GameObject* ret{ nullptr };
	XMVECTOR pos{};
	float minDistance{ FLT_MAX };
	for (const auto& obj : temp) {
		auto team = reinterpret_cast<EnemyObject*>(obj)->GetTeam();
		if (team == EnemyObject::TEAM::ENDCOUNT)continue;
		XMVECTOR pos{ (obj->GetPosition()) };
		float const distance{ XMVectorGetX(XMVector3Length(obj->GetPosition() - mPlayer->GetCamera()->GetPosition())) };
		if (distance < minDistance) ret = obj;
	}
	if (ret) {
		auto rr = reinterpret_cast<EnemyObject*>(ret);
		TeamAddCount(rr->GetTeam(), -1);
		rr->SetTeam(EnemyObject::TEAM::ENDCOUNT);
		pos = ret->GetPosition();
	}
	return make_pair(!temp.empty(), pos);
}