#pragma once

class Vertex
{
public:
	Vertex() : mxmf3Position{ 0.0f,0.0f,0.0f } {}
	Vertex(float x, float y, float z) : mxmf3Position{ x,y,z } {}
	Vertex(XMFLOAT3 pos) : mxmf3Position{ pos.x,pos.y,pos.z } {}
	Vertex(XMFLOAT3A pos) : mxmf3Position{ pos } {}
	virtual ~Vertex() {}

protected:
	XMFLOAT3A mxmf3Position;
};

class DiffusedVertex : public Vertex
{
public:
	DiffusedVertex() : mxmf4Diffuse{ 0.0f,0.0f,0.0f,0.0f } {}
	DiffusedVertex(float x, float y, float z, XMFLOAT4 diffuse)
		: Vertex{ x,y,z }
		, mxmf4Diffuse{ diffuse.x,diffuse.y,diffuse.z,diffuse.w } {}
	DiffusedVertex(float x, float y, float z, XMFLOAT4A diffuse)
		: Vertex{ x,y,z }
		, mxmf4Diffuse{ diffuse } {}
	DiffusedVertex(float x, float y, float z, float r, float g, float b, float a)
		: Vertex{ x,y,z }
		, mxmf4Diffuse{ r,g,b,a } {}
	DiffusedVertex(XMFLOAT3 pos, XMFLOAT4 diffuse)
		: Vertex{ pos }
		, mxmf4Diffuse{ diffuse.x,diffuse.y,diffuse.z,diffuse.w } {}
	DiffusedVertex(XMFLOAT3 pos, XMFLOAT4A diffuse)
		: Vertex{ pos }
		, mxmf4Diffuse{ diffuse } {}
	~DiffusedVertex() {}

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

protected:
	ComPtr<ID3D12Resource>	mVertexBuffer;
	ComPtr<ID3D12Resource>	mVertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
	D3D12_PRIMITIVE_TOPOLOGY mPrimitiveToplogy;
	UINT mSlot;
	UINT mVerticesCount;
	UINT mStride;
	UINT mOffset;

};

class TriangleMesh : public Mesh
{
public:
	TriangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual ~TriangleMesh();
};
