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

class MeshUnit : public Collideable
{
public:
	MeshUnit(ID3D12Device*, ID3D12GraphicsCommandList*, Mesh_Info, vector<D3D12_VERTEX_BUFFER_VIEW>& out);

public:
	UINT GetVerticesCount()const { return m_verticesCount; }

protected:
	ComPtr<ID3D12Resource>	m_vertexBuffer;
	ComPtr<ID3D12Resource>	m_vertexUploadBuffer;
	UINT m_verticesCount{ 0 };
};

class Mesh : public Collideable
{
public:
	Mesh(ID3D12Device*, ID3D12GraphicsCommandList*, const vector<Mesh_Info>&);

	virtual void Render(ID3D12GraphicsCommandList*, UINT instanceCount = 1);

public:
	void SetMeterial(XMFLOAT3 a, XMFLOAT3 d, float sp = 600.0f, XMFLOAT3 s = { 0.0f,0.0f,0.0f }) {
		m_meterial.ambient = a;	m_meterial.diffuse = d; m_meterial.specular = s; m_meterial.specualrPower = sp;
	}
	void SetEmessive(XMFLOAT3 e) { m_meterial.emessive = e; }
	Meterial GetMeterial() { return m_meterial; }

protected:
	vector<shared_ptr<MeshUnit>> m_meshes;
	vector<D3D12_VERTEX_BUFFER_VIEW> m_vertexBufferView;
	UINT m_slot{ 0 };
	Meterial m_meterial{};
};