#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"

Scene::Scene()
	: mGraphicsRootSignature{ nullptr }
{}

Scene::~Scene()
{

}

ID3D12RootSignature* Scene::CreateGraphicsRootSignature(ID3D12Device* device)
{
	ID3D12RootSignature* GraphicsRootSignature{ nullptr };
	HRESULT hResult;
	CD3DX12_ROOT_PARAMETER RootParameters[2];

	CD3DX12_ROOT_PARAMETER::InitAsConstants(
		  RootParameters[0]
		, 16
		, 0
		, 0
		, D3D12_SHADER_VISIBILITY_VERTEX);
	CD3DX12_ROOT_PARAMETER::InitAsConstants(
		RootParameters[1]
		, 32
		, 1
		, 0
		, D3D12_SHADER_VISIBILITY_VERTEX);
	
	D3D12_ROOT_SIGNATURE_FLAGS RSFlags{
		  D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT 
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		| D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS
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
	assert(mObjects.empty());
	TriangleMesh* mesh = new TriangleMesh(device, commandList);
	RotatingObject* RO = new RotatingObject();
	RO->SetMesh(mesh);

	DiffusedShader* shader = new DiffusedShader();
	shader->CreateShader(device, mGraphicsRootSignature.Get());
	shader->CreateShaderVariables(device, commandList);

	RO->SetShader(shader);

	mObjects.push_back(RO);
};

void Scene::ReleaseObjects()
{
	mGraphicsRootSignature.Reset();
	for (auto& obj : mObjects)delete obj;
	mObjects.clear();
};

void Scene::ReleaseUploadBuffers()
{
	for (auto& obj : mObjects)obj->ReleaseUploadBuffers();
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
	for (auto& obj : mObjects)obj->Animate(timeElapsed);
};

void Scene::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	assert(camera);
	camera->SetViewportScissorRect(commandList);
	commandList->SetGraphicsRootSignature(mGraphicsRootSignature.Get());
	camera->UpdateShaderVariables(commandList);
	for (auto& obj : mObjects)obj->Render(commandList, camera);
};


