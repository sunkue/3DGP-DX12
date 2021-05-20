#pragma once

#include "Camera.h"
#include "GameObject.h"

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4A mxmf44World;
};

class Shader
{
public:
	Shader() : mReferences{ 0 } {}
	virtual ~Shader();

public:
	void AddRef()	{ ++mReferences; assert(0 < mReferences); }
	void Release()	{ assert(0 < mReferences); if (--mReferences == 0) delete this; }

private:
	int mReferences;

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

class PlayerShader : public Shader
{
public:
	PlayerShader();
	virtual ~PlayerShader();

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature);
protected:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob) override;

};

class ObjectsShader : public Shader
{
public:
	ObjectsShader();
	virtual ~ObjectsShader();

	virtual void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
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