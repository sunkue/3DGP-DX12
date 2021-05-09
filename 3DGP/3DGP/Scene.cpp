#include "stdafx.h"
#include "Scene.h"

Scene::Scene() :
	mcomD3dGraphicsRootSignature{},
	mcomD3dPipelineState{}
{}

Scene::~Scene()
{

}

void Scene::CreateGraphicsRootSignature(ID3D12Device* pD3dDevice)
{
	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags{
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	};
	CD3DX12_ROOT_SIGNATURE_DESC d3DRootSignatureDesc{ 0,nullptr,0,nullptr,rootSignatureFlags };
	ComPtr<ID3DBlob> comD3dSignatureBlob;
	ComPtr<ID3DBlob> comD3dErrorBlob;
	ThrowIfFailedWithErr(D3D12SerializeRootSignature(&d3DRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		comD3dSignatureBlob.GetAddressOf(), comD3dErrorBlob.GetAddressOf()), comD3dErrorBlob.Get());
	ThrowIfFailed(pD3dDevice->CreateRootSignature(0, comD3dSignatureBlob->GetBufferPointer(),
		comD3dSignatureBlob->GetBufferSize(), IID_PPV_ARGS(mcomD3dGraphicsRootSignature.GetAddressOf())));
}

void Scene::CreateGraphicsPipelineState(ID3D12Device* pD3dDevice)
{
	ComPtr<ID3DBlob> comD3dVertexShaderBlob;
	ComPtr<ID3DBlob> comD3dPixelShaderBlob;
	ComPtr<ID3DBlob> comD3dErrorBlob;

	UINT compileFlag = 0;
#if defined(_DEBUG)
	compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	wstring shaderFile{ L"Shaders.hlsl" };

	ThrowIfFailedWithErr(D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "VSMain", "vs_5_1", compileFlag, 0,
		comD3dVertexShaderBlob.GetAddressOf(), comD3dErrorBlob.GetAddressOf()), comD3dErrorBlob.Get());
	ThrowIfFailedWithErr(D3DCompileFromFile(shaderFile.c_str(), nullptr, nullptr, "PSMain", "ps_5_1", compileFlag, 0,
		comD3dPixelShaderBlob.GetAddressOf(), comD3dErrorBlob.GetAddressOf()), comD3dErrorBlob.Get());

	CD3DX12_RASTERIZER_DESC d3DRasterizerDesc{ CD3DX12_DEFAULT{} };
	CD3DX12_BLEND_DESC d3DBlendDesc{ CD3DX12_DEFAULT{} };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3DPipelineStateDesc{};
	d3DPipelineStateDesc.pRootSignature = mcomD3dGraphicsRootSignature.Get();
	d3DPipelineStateDesc.VS.pShaderBytecode = comD3dVertexShaderBlob->GetBufferPointer();
	d3DPipelineStateDesc.VS.BytecodeLength = comD3dVertexShaderBlob->GetBufferSize();
	d3DPipelineStateDesc.PS.pShaderBytecode = comD3dPixelShaderBlob->GetBufferPointer();
	d3DPipelineStateDesc.PS.BytecodeLength = comD3dPixelShaderBlob->GetBufferSize();
	d3DPipelineStateDesc.RasterizerState = d3DRasterizerDesc;
	d3DPipelineStateDesc.BlendState = d3DBlendDesc;
	d3DPipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	d3DPipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	d3DPipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	d3DPipelineStateDesc.InputLayout.NumElements = 0;
	d3DPipelineStateDesc.SampleMask = UINT_MAX;
	d3DPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3DPipelineStateDesc.NumRenderTargets = 1;
	d3DPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3DPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3DPipelineStateDesc.SampleDesc.Count = 1;
	d3DPipelineStateDesc.SampleDesc.Quality = 0;

	ThrowIfFailed(pD3dDevice->CreateGraphicsPipelineState(&d3DPipelineStateDesc,
		IID_PPV_ARGS(mcomD3dPipelineState.GetAddressOf())));
}

void Scene::BuildObjects(ID3D12Device* pD3dDevice)
{
	CreateGraphicsRootSignature(pD3dDevice);
	CreateGraphicsPipelineState(pD3dDevice);
};

void Scene::ReleaseObjects()
{

};


///////////////////////////////////////////////////

bool Scene::OnProcessingMouseMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam)
{
	return false;
};

bool Scene::OnProcessingKeyboardMessage(HWND hWnd, UINT messageID, WPARAM wParam, LPARAM lParam)
{
	return false;
};

bool Scene::ProcessInput()
{
	return false;
};

void Scene::AnimateObjects(milliseconds timeElapsed)
{

};

void Scene::PrepareRender(ID3D12GraphicsCommandList* pD3dCommandList)
{
	pD3dCommandList->SetGraphicsRootSignature(mcomD3dGraphicsRootSignature.Get());
	pD3dCommandList->SetPipelineState(mcomD3dPipelineState.Get());
	pD3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
};

void Scene::Render(ID3D12GraphicsCommandList* pD3dCommandList)
{
	PrepareRender(pD3dCommandList);
	pD3dCommandList->DrawInstanced(6, 1, 0, 0);
};
