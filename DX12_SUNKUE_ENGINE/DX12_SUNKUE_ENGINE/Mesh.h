#pragma once


struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT3 nor;
	XMFLOAT2 tex;
};

struct Meterial
{
	XMFLOAT3 ambient;
	XMFLOAT3 diffuse;
	XMFLOAT3 specular;
	XMFLOAT3 emessive;
	float specualrPower;
};

struct Mesh_Info
{
	vector<Vertex> v;
	XMFLOAT3 center;
	XMFLOAT3 extents;
};

class VertexBufferData
{
public:
	VertexBufferData(ID3D12Device*, ID3D12GraphicsCommandList*, Mesh_Info, vector<D3D12_VERTEX_BUFFER_VIEW>& out);

public:
	UINT GetVerticesCount()const { return m_verticesCount; }
	
public:
	BoundingOrientedBox GetOOBB()const { return m_OOBB; }
	void SetOOBB(BoundingOrientedBox&& OOBB) { m_OOBB = move(OOBB); }
	void SetOOBB(BoundingOrientedBox& OOBB) { m_OOBB = OOBB; }

protected:
	ComPtr<ID3D12Resource>	m_vertexBuffer;
	ComPtr<ID3D12Resource>	m_vertexUploadBuffer;
	UINT m_verticesCount{ 0 };
	BoundingOrientedBox m_OOBB;
};

class Mesh
{
public:
	Mesh(ID3D12Device*, ID3D12GraphicsCommandList*, vector<Mesh_Info>, BoundingOrientedBox);

	virtual void Render(ID3D12GraphicsCommandList*, UINT instanceCount = 1);

public:
	void SetMeterial(XMFLOAT3 a, XMFLOAT3 d, float sp = 600.0f, XMFLOAT3 s = { 0.0f,0.0f,0.0f }) {
		m_meterial.ambient = a;	m_meterial.diffuse = d; m_meterial.specular = s; m_meterial.specualrPower = sp;
	}
	void SetEmessive(XMFLOAT3 e) { m_meterial.emessive = e; }
	Meterial GetMeterial() { return m_meterial; }

public:
	BoundingOrientedBox GetOOBB()const { return m_Big_OOBB; }
	void SetOOBB(BoundingOrientedBox&& OOBB) { m_Big_OOBB = move(OOBB); }
	void SetOOBB(BoundingOrientedBox& OOBB) { m_Big_OOBB = OOBB; }

protected:
	vector<shared_ptr<VertexBufferData>> m_meshes;
	vector<D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferView;
	UINT m_slot{ 0 };
	BoundingOrientedBox m_Big_OOBB;
	Meterial m_meterial{};
};