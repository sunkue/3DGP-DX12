#pragma once

struct ID3D12Device;
struct ID3D12GraphicsCommandList;

__interface IShaderHelper
{
public:
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) = 0;
	virtual void ReleaseShaderVariables() = 0;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList) = 0;
};

class Reference
{
public:
	Reference() = default;
	virtual ~Reference() = default;
	void AddRef() { ++m_reference; }
	void Release() { if (--m_reference == 0) delete this; }
	size_t GetReference()const { return m_reference; }
private:
	size_t m_reference{ 0 };
};

class Collideable
{
public:
	BoundingOrientedBox GetOOBB_origin()const { return m_OOBB; }
	BoundingOrientedBox GetOOBB_transformed(FXMMATRIX WM)const { BoundingOrientedBox ret; m_OOBB.Transform(ret, WM); return ret; }
	void SetOOBB(const BoundingOrientedBox& OOBB) { m_OOBB = OOBB; }
protected:
	BoundingOrientedBox m_OOBB;
};