#pragma once

struct PS_VB_EFFECT
{
	float mTime{ 0.0f };
	float mLifeTime{ 0.0f };
	XMFLOAT3 mPosition{ 0.0f,0.0f,0.0f };
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
	void NewObjEffect(FXMVECTOR pos, float lifeTime);

protected:
	ComPtr<ID3D12Resource>	mcbEffects;
	PS_VB_EFFECT* mcbMappedEffects;
	int size;
};