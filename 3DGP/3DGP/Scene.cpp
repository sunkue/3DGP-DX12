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
	D3D12_ROOT_SIGNATURE_FLAGS RSFlags{
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };
	CD3DX12_ROOT_SIGNATURE_DESC RSDesc{
		  0
		, nullptr
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
	Shader* shader = new Shader;
	shader->CreateShader(device, mGraphicsRootSignature.Get());
	shader->BuildObjects(device, commandList);
	shader->AddRef();
	mShaders.push_back(shader);
};

void Scene::ReleaseObjects()
{
	mGraphicsRootSignature.Reset();
	for (auto& sh : mShaders) {
		sh->ReleaseShaderVariables();
		sh->ReleaseObjects();
		sh->Release();
	}
	mShaders.clear();
};

void Scene::ReleaseUploadBuffers()
{
	for (auto& sh : mShaders)sh->ReleaseUploadBuffers();	
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
	for (auto& sh : mShaders)sh->AnimateObjects(timeElapsed);
};

void Scene::Render(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetGraphicsRootSignature(mGraphicsRootSignature.Get());
	for (auto& sh : mShaders)sh->Render(commandList);
};
