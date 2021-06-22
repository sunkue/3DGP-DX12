#pragma once

#include "Shader.h"

class Camera;

class TerrainShader : public Shader
{
public:
	TerrainShader();
	virtual ~TerrainShader();

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature)override;
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera) override;

protected:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob) override;

};