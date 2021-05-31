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
	CD3DX12_ROOT_PARAMETER RootParameters[4];
	/* 앞쪽이 접근속도가 빠름 */
	CD3DX12_ROOT_PARAMETER::InitAsConstants(
		  RootParameters[0]
		, 17
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

	assert(mShaders.empty());
	mShaders.emplace_back();
	mShaders[0].CreateShader(device, mGraphicsRootSignature.Get());
	mShaders[0].BuildObjects(device, commandList);
};

void Scene::ReleaseObjects()
{
	mGraphicsRootSignature.Reset();
	for (auto& shader : mShaders)
	{
		shader.ReleaseShaderVariables();
		shader.ReleaseObjects();
	}
};

void Scene::ReleaseUploadBuffers()
{
	for (auto& shader : mShaders)shader.ReleaseUploadBuffers();
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
	for (auto& shader : mShaders)shader.AnimateObjects(timeElapsed);
	CheckCollision(timeElapsed);
};

void Scene::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	assert(camera);
	camera->RSSetViewportScissorRect(commandList);
	commandList->SetGraphicsRootSignature(mGraphicsRootSignature.Get());
	camera->UpdateShaderVariables(commandList);
	for (auto& shader : mShaders)shader.Render(commandList, camera);
};

void Scene::CheckCollision(const milliseconds timeElapsed)
{
	const float timeE{ timeElapsed.count() / 1000.0f };
	// obj player
	if (!mPlayer->Collable()) {
		for (const auto& obj : mObjects) {
			if (obj->mOOBB.Intersects(mPlayer->mOOBB)) {
				reinterpret_cast<AirPlanePlayer*>(mPlayer)->Crash();
			}
		}
	}

	// wall obj
	for (const auto& wall : mWalls) {
		for (const auto& obj : mObjects) {
			if (wall->mOOBB.Intersects(obj->mOOBB)) {
				XMVECTOR dir{ obj->GetDir() };
				dir = XMVectorSetX(dir, -XMVectorGetX(dir));
				obj->SetDir(dir);
				assert((void*)obj != (void*)mPlayer);
			}
		}
	}

	// wall player
	for (const auto& wall : mWalls) {
		if (wall->mOOBB.Intersects(mPlayer->mOOBB)) {
			mEffect->NewWallEffect(mPlayer->GetPosition(), 500.0f);

			while (wall->mOOBB.Intersects(mPlayer->mOOBB)) {
				mPlayer->Move((mPlayer->GetPosition() - wall->GetPosition()) * XMVECTOR { 1.0f, 0.0f, 0.0f } *timeE);
				mPlayer->GameObject::SetPosition(mPlayer->GetPosition());
				mPlayer->UpdateBoundingBox();
			}
		}
	}
}