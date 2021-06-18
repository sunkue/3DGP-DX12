#pragma once

#include "Mesh.h"

struct SR_INSTANCE_INFO
{
	XMFLOAT4X4 transform;
	Meterial meterial;
};

struct SR_UI_INFO
{
	XMFLOAT4X4 transform;
	XMFLOAT4 color;
};

class Shader : public Reference
{
public:
	explicit Shader() = default;
	virtual ~Shader() = default;

public:
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariable(ID3D12GraphicsCommandList* commandList, XMFLOAT4X4A* world);

	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera);
	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature);

protected:
	virtual D3D12_SHADER_BYTECODE	CreatePixelShader(ID3DBlob** ShaderBlob);
	virtual D3D12_SHADER_BYTECODE	CreateVertexShader(ID3DBlob** ShaderBlob);
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* fileName, LPCSTR shaderName, LPCSTR shaderProfile, ID3DBlob** shaderBlob);
	virtual D3D12_INPUT_LAYOUT_DESC	CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC	CreateRasterizerState();
	virtual D3D12_BLEND_DESC		CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual void PrepareRender(ID3D12GraphicsCommandList* commandList);

protected:
	vector<ComPtr<ID3D12PipelineState>> mPipelineStates;
};

class ObjectsShader : public Shader
{
public:
	explicit ObjectsShader();
	virtual ~ObjectsShader();

	virtual void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, void* context);
	virtual void AnimateObjects(milliseconds timeElapsed);
	virtual void ReleaseObjects();
	virtual void ReleaseUploadBuffers();

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature);
	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera) override;

protected:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob) override;
	
protected:
	vector<shared_ptr<GameObject>> mObjects;
};

class InstancingShader : public ObjectsShader
{
public:
	InstancingShader();
	virtual ~InstancingShader();

	virtual void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, void* context = nullptr)override;
	virtual void ReleaseObjects()override;
	virtual void AnimateObjects(milliseconds timeElapsed)override;

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature)override;
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera) override;

protected:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob) override;

protected:
	ComPtr<ID3D12Resource>	mcbGameObjects;
	SR_INSTANCE_INFO*			mcbMappedGameObjects;
	D3D12_VERTEX_BUFFER_VIEW mInstancingBufferView;
};

class TerrainShader : public Shader
{
public:
	TerrainShader();
	virtual ~TerrainShader();

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature)override;

protected:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob) override;
	
};

class UIShader : public InstancingShader
{
public:
	UIShader();
	virtual ~UIShader();

	virtual void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, void* context = nullptr)override;
	virtual void ReleaseObjects()override;
	virtual void AnimateObjects(milliseconds timeElapsed)override;

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature)override;
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera) override;

protected:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob) override;

protected:
	ComPtr<ID3D12Resource>	mcb_UIs;
	SR_UI_INFO* mcb_MappedUIs;
	D3D12_VERTEX_BUFFER_VIEW m_UIBufferView;
};