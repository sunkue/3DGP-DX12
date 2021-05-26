#pragma once

class Vertex
{
public:
	Vertex() : mPosition{ 0.0f,0.0f,0.0f } {}
	Vertex(float x, float y, float z) : mPosition{ x,y,z } {}
	explicit Vertex(XMFLOAT3A pos)	: mPosition{ pos.x,pos.y,pos.z } {}

protected:
	XMFLOAT3A mPosition;
};

class DiffusedVertex : public Vertex
{
public:
	DiffusedVertex() : mxmf4Diffuse{ 0.0f,0.0f,0.0f,0.0f } {}
	DiffusedVertex(float x, float y, float z, XMFLOAT4A diffuse)
		: Vertex{ x,y,z }
		, mxmf4Diffuse{ diffuse } {}
	DiffusedVertex(float x, float y, float z, XMVECTORF32 diffuse)
		: Vertex{ x,y,z }
		, mxmf4Diffuse{ diffuse } {}
	DiffusedVertex(float x, float y, float z, FXMVECTOR diffuse)
		: Vertex{ x,y,z }
	{
		XMStoreFloat4A(&mxmf4Diffuse, diffuse);
	}
	DiffusedVertex(float x, float y, float z, float r, float g, float b, float a)
		: Vertex{ x,y,z }
		, mxmf4Diffuse{ r,g,b,a } {}
	DiffusedVertex(XMFLOAT3A pos, XMFLOAT4A diffuse)
		: Vertex{ pos }
		, mxmf4Diffuse{ diffuse } {}

protected:
	XMFLOAT4A mxmf4Diffuse;
};

class Mesh
{
public:
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual ~Mesh();

public:
	void AddRef()	{ ++mReferences; assert(0 < mReferences); }
	void Release()	{ assert(0 < mReferences); if (--mReferences == 0)delete this; }

private:
	int mReferences;

public:
	void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* commandList);
	virtual void Render(ID3D12GraphicsCommandList* commandList, UINT instanceCount);
	virtual void Render(ID3D12GraphicsCommandList* commandList, UINT instanceCount, D3D12_VERTEX_BUFFER_VIEW instancingBufferView);
	
protected:
	ComPtr<ID3D12Resource>	mVertexBuffer;
	ComPtr<ID3D12Resource>	mVertexUploadBuffer;
	ComPtr<ID3D12Resource>	mIndexBuffer;
	ComPtr<ID3D12Resource>	mIndexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW	 mIndexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY mPrimitiveToplogy;
	UINT mVerticesCount;
	UINT mIndicesCount;
	UINT mStartIndex;
	int  mBaseVertex;
	UINT mSlot;
	UINT mStride;
	UINT mOffset;
};

class TriangleMesh : public Mesh
{
public:
	explicit TriangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual ~TriangleMesh();
};

class CubeMeshDiffused : public Mesh
{
public:
	CubeMeshDiffused(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
		, float width = 2.0f, float height = 2.0f, float depth = 2.0f);
	virtual ~CubeMeshDiffused();
};

class AirplaneMeshDiffused : public Mesh
{
public:
	AirplaneMeshDiffused(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
		, float width = 20.0f, float height = 20.0f, float depth = 4.0f, XMVECTORF32 color = Colors::BlueViolet);
	virtual ~AirplaneMeshDiffused();
};