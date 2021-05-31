#pragma once

struct PS_VB_EFFECT
{
	XMFLOAT3A mPosition;
	float mTime{ 0.0f };
	float mLifeTime;

	friend bool alive(PS_VB_EFFECT* a);
	friend bool dead(PS_VB_EFFECT* a);
};
// 0 wall	 1 Obj

__interface IShaderHelper
{
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) = 0;
	virtual void ReleaseShaderVariables() = 0;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList) = 0;
};

class Effect : public IShaderHelper
{
public:
	Effect(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int n);
	virtual ~Effect();

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)override;
	virtual void ReleaseShaderVariables()override;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList)override;

	void NewWallEffect(FXMVECTOR pos, float lifeTime);

protected:
	vector<PS_VB_EFFECT*> mEffects;
	ComPtr<ID3D12Resource>	mcbEffects;
	PS_VB_EFFECT* mcbMappedEffects;

	D3D12_VERTEX_BUFFER_VIEW mInstancingBufferView;
};