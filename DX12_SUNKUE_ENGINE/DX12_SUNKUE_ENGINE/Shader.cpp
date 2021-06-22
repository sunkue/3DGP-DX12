#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"



//////////////////////////////////////

D3D12_RASTERIZER_DESC ShaderCompiler::CreateRasterizerState()
{
	CD3DX12_RASTERIZER_DESC RET{ CD3DX12_DEFAULT {} };
	RET.CullMode = D3D12_CULL_MODE_BACK;
	return RET;
}

D3D12_DEPTH_STENCIL_DESC ShaderCompiler::CreateDepthStencilState()
{
	CD3DX12_DEPTH_STENCIL_DESC DSD{ CD3DX12_DEFAULT {} };
	DSD.StencilReadMask		= 0x00;
	DSD.StencilWriteMask	= 0x00;
	DSD.FrontFace.StencilFunc	= D3D12_COMPARISON_FUNC_NEVER;
	DSD.BackFace.StencilFunc	= D3D12_COMPARISON_FUNC_NEVER;
	return DSD;
}

D3D12_BLEND_DESC ShaderCompiler::CreateBlendState()
{
	return CD3DX12_BLEND_DESC{ CD3DX12_DEFAULT {} };
}

D3D12_SHADER_BYTECODE ShaderCompiler::CompileShaderFromFile(
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

/// /////////////////////////////////////////////////

void Shader::CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature)
{
	ComPtr<ID3DBlob> pd3dVertexShaderBlob;
	ComPtr<ID3DBlob> pd3dPixelShaderBlob;
	
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PSDesc{};
	PSDesc.pRootSignature = rootSignature;
	PSDesc.VS = CreateVertexShader(pd3dVertexShaderBlob.GetAddressOf());
	PSDesc.PS = CreatePixelShader(pd3dPixelShaderBlob.GetAddressOf());	
	PSDesc.RasterizerState		= CreateRasterizerState();
	PSDesc.BlendState			= CreateBlendState();
	PSDesc.DepthStencilState	= CreateDepthStencilState();
	PSDesc.InputLayout			= CreateInputLayout();
	PSDesc.SampleMask			= UINT_MAX;
	PSDesc.PrimitiveTopologyType= D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PSDesc.NumRenderTargets		= 1;
	PSDesc.RTVFormats[0]		= DXGI_FORMAT_R8G8B8A8_UNORM;
	PSDesc.DSVFormat			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	PSDesc.SampleDesc.Count		= 1;
	m_PipelineStates.emplace_back();
	ThrowIfFailed(device->CreateGraphicsPipelineState(
		  &PSDesc
		, IID_PPV_ARGS(m_PipelineStates.at(0).GetAddressOf())));
	if (PSDesc.InputLayout.pInputElementDescs)delete[]PSDesc.InputLayout.pInputElementDescs;
}

void Shader::SetPSO(ID3D12GraphicsCommandList* commandList, size_t PSO_index)
{
	commandList->SetPipelineState(m_PipelineStates.at(PSO_index).Get());
}

///////////////////////////////////////////////////

void ObjectsShader::AnimateObjects(float timeElapsed)
{
	for (const auto& obj : m_Objects)obj->Animate(timeElapsed);
}

void ObjectsShader::OnRender(ID3D12GraphicsCommandList* commandList, Camera* camera)
{
	for (const auto& obj : m_Objects)obj->Render(commandList, camera);
}

////////////////////////////////////