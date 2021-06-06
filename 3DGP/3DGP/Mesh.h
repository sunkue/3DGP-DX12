#pragma once

class Vertex
{
public:
	Vertex() : mPosition{ 0.0f,0.0f,0.0f } {}
	Vertex(float x, float y, float z) : mPosition{ x,y,z } {}
	explicit Vertex(XMFLOAT3A pos) : mPosition{ pos.x,pos.y,pos.z } {}
	explicit Vertex(XMVECTOR pos) { XMStoreFloat3A(&mPosition, pos); }

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
	DiffusedVertex(XMVECTOR pos, XMVECTORF32 diffuse)
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
	virtual void Render(ID3D12GraphicsCommandList* commandList, UINT instanceCount = 1);
	virtual void Render(ID3D12GraphicsCommandList* commandList, UINT instanceCount, D3D12_VERTEX_BUFFER_VIEW instancingBufferView);

public:
	BoundingOrientedBox const& GetOOBB()const { return mOOBB; }
	void SetOOBB(BoundingOrientedBox&& OOBB) { mOOBB = OOBB; }

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

	BoundingOrientedBox mOOBB;
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

class PlayerMeshDiffused : public Mesh
{
public:
	PlayerMeshDiffused(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
		, float width = 4.0f, float height = 4.0f, float depth = 4.0f, XMVECTORF32 color = Colors::BlueViolet);
	virtual ~PlayerMeshDiffused();
};

class HeightMapImage
{
public:
	HeightMapImage(string_view fileName, int width, int length, XMFLOAT3A scale);
	~HeightMapImage();

	float const GetHeight(float x, float z)const;
	XMVECTOR const XM_CALLCONV GetNormal(int x, int z)const;
	
	XMFLOAT3A const GetScale()const { return mScale; }
	const BYTE* const GetPixels()const { return mHeightMapPixels.get(); }
	int const GetWidth()const { return mWidth; }
	int const GetLength()const { return mLength; }

private:
	bool const OutOfRange(int w, int l)const { return (w < 0.0f) || (l < 0.0f) || (mWidth <= w) || (mLength <= l); }
	bool const OutOfRange(float w, float l)const { return (w < 0.0f) || (l < 0.0f) || (mWidth <= w) || (mLength <= l); }

private:
	unique_ptr<BYTE[]> mHeightMapPixels;
	int mWidth;
	int mLength;
	XMFLOAT3A mScale;

};

class HeighMapGridMesh : public Mesh
{
public:
	HeighMapGridMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
		, int Xstart, int Zstart, int width, int length
		, XMFLOAT3A scale = { 1.0f,1.0f,1.0f }
		, XMVECTORF32 color = Colors::Yellow
		, void* context = nullptr);
	virtual ~HeighMapGridMesh();

	XMFLOAT3A const GetScale()const { return mScale; }
	int const GetWidth()const { return mWidth; }
	int const GetLength()const { return mLength; }

	virtual float const OnGetHeight(int x, int z, void* context)const;
	virtual XMVECTORF32 const OnGetColor(int x, int z, void* context)const;

protected:
	int mWidth;
	int mLength;
	XMFLOAT3A mScale;

};

class SquareMesh : public Mesh
{
public:
	SquareMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual ~SquareMesh();
};