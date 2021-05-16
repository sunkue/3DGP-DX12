#include "stdafx.h"
#include "Shader.h"

//////////////////////////////////////

Shader::~Shader()
{
	for (auto& ps : mPipelineStates) ps.Reset();
}

//////////////////////////////////////

D3D12_RASTERIZER_DESC Shader::CreateRasterizerState()
{
	return CD3DX12_RASTERIZER_DESC{ CD3DX12_DEFAULT {} };
}

D3D12_DEPTH_STENCIL_DESC Shader::CreateDepthStencilState()
{
	CD3DX12_DEPTH_STENCIL_DESC DSD{ CD3DX12_DEFAULT {} };
	DSD.StencilReadMask		= 0x00;
	DSD.StencilWriteMask	= 0x00;
	DSD.FrontFace.StencilFunc	= D3D12_COMPARISON_FUNC_NEVER;
	DSD.BackFace.StencilFunc	= D3D12_COMPARISON_FUNC_NEVER;
	return DSD;
}

D3D12_BLEND_DESC Shader::CreateBlendState()
{
	return CD3DX12_BLEND_DESC{ CD3DX12_DEFAULT {} };
}

D3D12_INPUT_LAYOUT_DESC Shader::CreateInputLayout()
{
	constexpr UINT INPUT_ELEMENT_DESCS_COUNT{ 2 };
	D3D12_INPUT_ELEMENT_DESC* inputElementDescs = new D3D12_INPUT_ELEMENT_DESC[INPUT_ELEMENT_DESCS_COUNT]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0
		, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(Vertex)
		, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = INPUT_ELEMENT_DESCS_COUNT;
	return inputLayoutDesc;
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3D10Blob** shaderBlob)
{
	return CompileShaderFromFile(const_cast<WCHAR*>(L"Shaders.hlsl"), "VSMain", "vs_5_1", shaderBlob);
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3D10Blob** shaderBlob)
{
	return CompileShaderFromFile(const_cast<WCHAR*>(L"Shaders.hlsl"), "PSMain", "ps_5_1", shaderBlob);
}

D3D12_SHADER_BYTECODE Shader::CompileShaderFromFile(
	  WCHAR*		fileName
	, LPCSTR		shaderName
	, LPCSTR		shaderProfile
	, ID3DBlob**	shaderBlob
)
{
	HRESULT hResult;
	ComPtr<ID3DBlob> comD3dErrorBlob;

	UINT compileFlag = 0
		| D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
	compileFlag = compileFlag
		| D3DCOMPILE_DEBUG
		| D3DCOMPILE_SKIP_OPTIMIZATION
		| D3DCOMPILE_WARNINGS_ARE_ERRORS
		;
#else
	compileFlag = compileFlag
		| D3DCOMPILE_OPTIMIZATION_LEVEL3
		;
#endif
	hResult = D3DCompileFromFile(
		  fileName
		, nullptr
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, shaderName
		, shaderProfile
		, compileFlag
		, 0
		, shaderBlob
		, comD3dErrorBlob.GetAddressOf());
	OutputErrorMessage(comD3dErrorBlob.Get(), cout);
	ThrowIfFailed(hResult);

	CD3DX12_SHADER_BYTECODE d3dShaderByteCode{ *shaderBlob };
	return d3dShaderByteCode;
}

/// //////////////////////////////////

void Shader::CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature)
{
	assert(mPipelineStates.empty());
	mPipelineStates.emplace_back();

	ComPtr<ID3DBlob> pd3dVertexShaderBlob;
	ComPtr<ID3DBlob> pd3dPixelShaderBlob;
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSDesc{};
	PSDesc.pRootSignature = rootSignature;
	PSDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	PSDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);	
	PSDesc.RasterizerState		= CreateRasterizerState();
	PSDesc.BlendState			= CreateBlendState();
	PSDesc.DepthStencilState	= CreateDepthStencilState();
	PSDesc.InputLayout			= CreateInputLayout();
	PSDesc.SampleMask			= UINT_MAX;
	PSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PSDesc.NumRenderTargets		= 1;
	PSDesc.RTVFormats[0]		= DXGI_FORMAT_R8G8B8A8_UNORM;
	PSDesc.DSVFormat			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	PSDesc.SampleDesc.Count		= 1;
	ThrowIfFailed(device->CreateGraphicsPipelineState(
		  &PSDesc
		, IID_PPV_ARGS(mPipelineStates[0].GetAddressOf())));
	
	if (PSDesc.InputLayout.pInputElementDescs)
		delete[]PSDesc.InputLayout.pInputElementDescs;
}

//////////////////////////////////////

void Shader::BuildObjects(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, void* context)
{
	TriangleMesh* triangleMesh{ new TriangleMesh{device,commandList} };
	assert(mObjects.empty());
	mObjects.emplace_back();
	mObjects[0] = new GameObject();
	mObjects[0]->SetMesh(triangleMesh);
}

void Shader::ReleaseObjects()
{
	for (auto& obj : mObjects)delete obj;
	mObjects.clear();
}

void Shader::AnimateObjects(milliseconds timeElapsed)
{
	for (auto& obj : mObjects)obj->Animate(timeElapsed);
}

void Shader::ReleaseUploadBuffers()
{
	for (auto& obj : mObjects)obj->ReleaseUploadBuffers();
}

void Shader::PrepareRender(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetPipelineState(mPipelineStates[0].Get());
}

void Shader::Render(ID3D12GraphicsCommandList* commandList)
{
	PrepareRender(commandList);
	for (auto& obj : mObjects)obj->Render(commandList);
}
