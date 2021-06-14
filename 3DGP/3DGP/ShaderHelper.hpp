#pragma once

struct ID3D12Device;
struct ID3D12GraphicsCommandList;

__interface IShaderHelper
{
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList) = 0;
	virtual void ReleaseShaderVariables() = 0;
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList) = 0;
};