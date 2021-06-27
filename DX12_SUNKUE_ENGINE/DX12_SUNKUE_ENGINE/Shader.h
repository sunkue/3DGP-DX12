#pragma once

#include "GameObject.h"

class GameObject;
class Camera;

class ShaderCompiler abstract
{
protected:
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR* fileName, LPCSTR shaderName, LPCSTR shaderProfile, ID3DBlob** shaderBlob);
	virtual D3D12_INPUT_LAYOUT_DESC	CreateInputLayout() abstract = 0;
	virtual D3D12_SHADER_BYTECODE	CreatePixelShader(ID3DBlob** ShaderBlob) abstract = 0;
	virtual D3D12_SHADER_BYTECODE	CreateVertexShader(ID3DBlob** ShaderBlob) abstract = 0;
	virtual D3D12_RASTERIZER_DESC	CreateRasterizerState();
	virtual D3D12_BLEND_DESC		CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
};


class Shader abstract : public ShaderCompiler, public IShaderResourceHelper
{
public:
	virtual ~Shader() { ReleaseShaderVariables(); };

public:
	virtual void Render(ID3D12GraphicsCommandList* commandList, Camera* camera) abstract = 0;
	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature);

protected:
	void SetPSO(ID3D12GraphicsCommandList* commandList, size_t PSO_index = 0);

protected:
	vector<ComPtr<ID3D12PipelineState>> m_PipelineStates;
};


class ObjectsShader abstract : public Shader
{
public:
	virtual ~ObjectsShader() = default;

	virtual void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, void* context) abstract = 0;
	virtual void AnimateObjects(float timeElapsed);
	void ReleaseObjects() { m_Objects.clear(); }
	
	void Render(ID3D12GraphicsCommandList* commandList, Camera* camera, size_t PSO_index = 0) { SetPSO(commandList, PSO_index); OnRender(commandList, camera); }
	virtual void OnRender(ID3D12GraphicsCommandList* commandList, Camera* camera);
	
protected:
	vector<shared_ptr<GameObject>> m_Objects;
};


template<class SR> class SRVShader abstract : public ObjectsShader
{
public:
	void SetRootParamIndex(UINT index) { m_RootParamIndex = index; }
	
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) final
	{
		const UINT buffeSize{ static_cast<UINT>(sizeof(SR) * m_Objects.size()) };
		m_RSRC = CreateBufferResource(device, commandList, nullptr, buffeSize
			, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
		m_RSRC->Map(0, nullptr, (void**)&m_Mapped_RSRC);
	}
	
	virtual void ReleaseShaderVariables()final
	{
		if (m_RSRC) {
			m_RSRC->Unmap(0, nullptr);
			m_RSRC.Reset();
		}
	}
	
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList) final
	{
		SetSRV(commandList);
		OnUpdateShaderVariables();
	}
	
	void SetSRV(ID3D12GraphicsCommandList* commandList) 
	{ 
		commandList->SetGraphicsRootShaderResourceView(m_RootParamIndex, m_RSRC->GetGPUVirtualAddress());
	}
protected:
	virtual void OnUpdateShaderVariables() abstract = 0;

protected:
	ComPtr<ID3D12Resource> m_RSRC;
	SR* m_Mapped_RSRC;
	UINT m_RootParamIndex;
};



