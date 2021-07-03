#pragma once

#include "Mesh.h"

class HeightMapImage
{
public:
	HeightMapImage(string_view fileName, int width, int length, XMFLOAT3 scale);

	float GetHeight(float x, float z)const;
	XMVECTOR XM_CALLCONV GetNormal(int x, int z)const;

	auto GetScale()const { return m_scale; }
	const BYTE* GetPixels()const { return m_heightMapPixels.get(); }
	int GetWidth()const { return m_width; }
	int GetLength()const { return m_length; }

private:
	template<typename N>requires is_arithmetic<N>::value
		bool const OutOfRange(N w, N l)const { return (w < 0) || (l < 0) || (m_width <= w) || (m_length <= l); }

private:
	shared_ptr<BYTE[]> m_heightMapPixels;
	int m_width;
	int m_length;
	XMFLOAT3 m_scale;
};

class HeighMapGridMeshUnit
{
public:
	HeighMapGridMeshUnit(ID3D12Device*, ID3D12GraphicsCommandList*, Mesh_Info&
		, vector<D3D12_VERTEX_BUFFER_VIEW>& out1, vector<D3D12_INDEX_BUFFER_VIEW>& out2, void* context = nullptr);

public:
	UINT GetVerticesCount()const { return m_verticesCount; }

protected:
	ComPtr<ID3D12Resource>	m_vertexBuffer;
	ComPtr<ID3D12Resource>	m_vertexUploadBuffer;
	UINT m_verticesCount{ 0 };

	ComPtr<ID3D12Resource>	m_indexBuffer;
	ComPtr<ID3D12Resource>	m_indexUploadBuffer;
	UINT m_indiciesCount{ 0 };
};

class HeighMapGridMesh
{
public:
	HeighMapGridMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
		, int Xstart, int Zstart, int width, int length
		, XMFLOAT3 scale = { 1.0f,1.0f,1.0f }
		, void* context = nullptr);
	virtual ~HeighMapGridMesh() = default;

	virtual void Render(ID3D12GraphicsCommandList*, UINT instanceCount = 1);

public:
	void SetMeterial(XMFLOAT3 a, XMFLOAT3 d, float sp = 600.0f, XMFLOAT3 s = { 0.0f,0.0f,0.0f }) {
		m_meterial.ambient = a;	m_meterial.diffuse = d; m_meterial.specular = s; m_meterial.specualrPower = sp;
	}
	void SetEmessive(XMFLOAT3 e) { m_meterial.emessive = e; }
	Meterial GetMeterial() { return m_meterial; }

public:
	auto GetScale()const { return m_scale; }
	int GetWidth()const { return m_width; }
	int GetLength()const { return m_length; }

	virtual float OnGetHeight(int x, int z, void* context)const;

protected:
	vector<shared_ptr<HeighMapGridMeshUnit>> m_meshes;
	vector<D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferView;
	vector<D3D12_INDEX_BUFFER_VIEW> m_indexBufferView;
	UINT m_slot{ 0 };
	Meterial m_meterial{};
	int m_width;
	int m_length;
	XMFLOAT3 m_scale;
};