#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"

Scene::Scene()
	: mGraphicsRootSignature{ nullptr }
	, mTerrain{ nullptr }
{}

Scene::~Scene()
{

}

ID3D12RootSignature* Scene::CreateGraphicsRootSignature(ID3D12Device* device)
{
	ID3D12RootSignature* GraphicsRootSignature{ nullptr };
	HRESULT hResult;
	CD3DX12_ROOT_PARAMETER RootParameters[3];
	CD3DX12_ROOT_PARAMETER::InitAsConstants(
		  RootParameters[0]
		, 16
		, 0						//b0 Player
		, 0
		, D3D12_SHADER_VISIBILITY_VERTEX);
	
	CD3DX12_ROOT_PARAMETER::InitAsConstants(
		  RootParameters[1]
		, 32
		, 1						//b1 Camera
		, 0
		, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_ROOT_PARAMETER::InitAsShaderResourceView(
		RootParameters[2]
		, 0						//t0
		, 0
		, D3D12_SHADER_VISIBILITY_VERTEX);
	
	D3D12_ROOT_SIGNATURE_FLAGS RSFlags{
		  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT 
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
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
	XMFLOAT4A color{ 0.0f,0.2f,0.0f,0.0f };

#ifdef WITH_TERRAIN_PARITION
	mTerrain = new HeightMapTerrain{
		  device
		, commandList
		, mGraphicsRootSignature.Get()
		, _T("../Assets/Image/Terrain/HeightMap.raw")
		, 257, 257, 17, 17, scale, color };
#else
	mTerrain = new HeightMapTerrain{
		  device
		, commandList
		, mGraphicsRootSignature.Get()
		, _T("1.raw")
		, 257, 257, 257, 257, scale, color };
#endif
	assert(mShaders.empty());
	mShaders.emplace_back();
	mShaders[0].CreateShader(device, mGraphicsRootSignature.Get());
	mShaders[0].BuildObjects(device, commandList, mTerrain);
};

void Scene::ReleaseObjects()
{
	mGraphicsRootSignature.Reset();
	for (auto& shader : mShaders)
	{
		shader.ReleaseShaderVariables();
		shader.ReleaseObjects();
	}
	delete mTerrain;
};

void Scene::ReleaseUploadBuffers()
{
	for (auto& shader : mShaders)shader.ReleaseUploadBuffers();
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
	for (auto& shader : mShaders)shader.AnimateObjects(timeElapsed);
};

void Scene::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	assert(camera);
	camera->RSSetViewportScissorRect(commandList);
	commandList->SetGraphicsRootSignature(mGraphicsRootSignature.Get());
	camera->UpdateShaderVariables(commandList);
	if (mTerrain)mTerrain->Render(commandList, camera);
	for (auto& shader : mShaders)shader.Render(commandList, camera);
};


