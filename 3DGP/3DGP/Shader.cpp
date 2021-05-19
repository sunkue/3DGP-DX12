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
	CD3DX12_RASTERIZER_DESC RET{ CD3DX12_DEFAULT {} };
	RET.CullMode = D3D12_CULL_MODE_BACK;
	RET.FillMode = D3D12_FILL_MODE_WIREFRAME;
	return RET;
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
	return D3D12_INPUT_LAYOUT_DESC{ nullptr,0 };
}

D3D12_SHADER_BYTECODE Shader::CreateVertexShader(ID3D10Blob** shaderBlob)
{
	return CD3DX12_SHADER_BYTECODE{ nullptr,0 };
}

D3D12_SHADER_BYTECODE Shader::CreatePixelShader(ID3D10Blob** shaderBlob)
{
	return CD3DX12_SHADER_BYTECODE{ nullptr,0 };
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
	assert(!mPipelineStates.empty());


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

void Shader::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{

}

void Shader::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)
{
	assert(false);
}

void Shader::UpdateShaderVariable(ID3D12GraphicsCommandList* commandList, XMFLOAT4X4A* world)
{
	XMFLOAT4X4A w;
	XMStoreFloat4x4A(&w, XMMatrixTranspose(XMLoadFloat4x4A(world)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &w, 0);
	
}

void Shader::ReleaseShaderVariables()
{

}

void Shader::PrepareRender(ID3D12GraphicsCommandList* commandList)
{
	commandList->SetPipelineState(mPipelineStates[0].Get());
}

void Shader::Render(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	PrepareRender(commandList);
}

///////////////////////////////////////////////////

DiffusedShader::DiffusedShader()
{

}

DiffusedShader::~DiffusedShader()
{

}

D3D12_INPUT_LAYOUT_DESC DiffusedShader::CreateInputLayout()
{
	constexpr UINT InputElemDescsCount = 2;
	D3D12_INPUT_ELEMENT_DESC* InputElemDescs = 
		new D3D12_INPUT_ELEMENT_DESC[InputElemDescsCount]
	{
	 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	,{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, sizeof(Vertex), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	D3D12_INPUT_LAYOUT_DESC InputLayoutDesc;
	InputLayoutDesc.pInputElementDescs = InputElemDescs;
	InputLayoutDesc.NumElements = InputElemDescsCount;
	return InputLayoutDesc;
}

D3D12_SHADER_BYTECODE DiffusedShader::CreateVertexShader(ID3DBlob** shaderBlob)
{
	return CompileShaderFromFile(const_cast<WCHAR*>
		(L"Shaders.hlsl"), "VSDiffused", "vs_5_1", shaderBlob);
}

D3D12_SHADER_BYTECODE DiffusedShader::CreatePixelShader(ID3DBlob** shaderBlob)
{
	return CompileShaderFromFile(const_cast<WCHAR*>
		(L"Shaders.hlsl"), "PSDiffused", "ps_5_1", shaderBlob);
}

void DiffusedShader::CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature)
{
	mPipelineStates.emplace_back();
	Shader::CreateShader(device, rootSignature);
}