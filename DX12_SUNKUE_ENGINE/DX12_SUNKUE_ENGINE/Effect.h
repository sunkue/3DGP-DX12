#pragma once


struct SR_EFFECT
{
	XMFLOAT3 m_Position{ 0,0,0 };
	float m_EmitTime{ 0 };
	float m_LifeTime{ 0 };
};

class Effect abstract : public IShaderResourceHelper
{
public:
	Effect(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, size_t n);
	virtual ~Effect() = default;

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) final;
	virtual void ReleaseShaderVariables() final;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList) final;
	virtual void OnUpdateShaderVariables(ID3D12GraphicsCommandList* commandList) abstract = 0;

protected:
	ComPtr<ID3D12Resource> m_RSRC;
	SR_EFFECT* m_Mapped_RSRC;
	UINT m_RootParamIndex;
	size_t size;
};