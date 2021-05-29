#pragma once

struct PS_VB_EFFECT
{
	XMFLOAT3A mPosition;
	float mTime;
	float mLifeTime;
};

__interface IShaderHelper
{
public:
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) = 0;
	virtual void ReleaseShaderVariables() = 0;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList) = 0;
};

class EffectShader : IShaderHelper
{
public:
	EffectShader();
	virtual ~EffectShader();

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables()override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

protected:
	vector<PS_VB_EFFECT*> mEffects;
	ComPtr<ID3D12Resource>	mcbEffects;
	PS_VB_EFFECT* mcbMappedEffects;

	D3D12_VERTEX_BUFFER_VIEW mInstancingBufferView;
};