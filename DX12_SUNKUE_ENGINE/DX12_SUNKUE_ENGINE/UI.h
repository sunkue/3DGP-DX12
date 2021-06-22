#pragma once

#include "Shader.h"

class Camera;

struct SR_UI_INFO
{
	XMFLOAT4X4 transform;
	XMFLOAT4 color;
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