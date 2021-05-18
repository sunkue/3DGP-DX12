#pragma once


struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4A mxmf44View;
	XMFLOAT4X4A mxmf44Projection;
};

class Camera
{
public:
	Camera();
	virtual ~Camera();

	virtual void CreateShaderVariables(
		  ID3D12Device* device
		, ID3D12GraphicsCommandList* commandlist);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commadList);

	void GenerateViewMatrix(XMFLOAT3 pos, XMFLOAT3 lookAt, XMFLOAT3 up);
	void GenerateProjectionMatrix(float fov, float aspect, float n, float f);
	
	void SetViewport(int xTopLeft, int yTopLeft, int width, int height
		, float minZ, float maxZ);
	void SetScissorRect(UINT xLeft, UINT yTop, UINT xRight, UINT yBottom);
	virtual void SetViewportScissorRect(ID3D12GraphicsCommandList* commandList);

protected:
	XMFLOAT4X4A mxmf44View;
	XMFLOAT4X4A mxmf44Projection;

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;
};

