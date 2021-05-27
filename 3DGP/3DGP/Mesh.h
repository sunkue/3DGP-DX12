#pragma once

class Vertex
{
public:
	Vertex() : mPosition{ 0.0f,0.0f,0.0f } {}
	Vertex(float x, float y, float z) : mPosition{ x,y,z } {}
	explicit Vertex(FXMVECTOR pos) { XMStoreFloat3A(&mPosition, pos); }
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
	DiffusedVertex(FXMVECTOR pos, FXMVECTOR diffuse)
		: Vertex{ pos }
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

////////////////////////////////////

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
		, float width = 20.0f, float height = 20.0f, float depth = 4.0f, XMVECTORF32 color = Colors::Aquamarine);
	virtual ~AirplaneMeshDiffused();
};


////////////////////////////////////

class HeightMapImage
{
public:
	HeightMapImage(LPCTSTR fileName, int width, int length, XMFLOAT3A scale);
	~HeightMapImage();

	float GetHeight(float x, float z)const;
	XMVECTOR XM_CALLCONV GetHeightNormal(int x, int z)const;
	
	XMVECTOR XM_CALLCONV GetScale()const { return XMLoadFloat3A(&mScale); }
	BYTE* GetPixels()const { return mHeightMapPixels; }
	int GetWidth() { return mWidth; }
	int GetLength() { return mLength; }	

private:
	BYTE*		mHeightMapPixels;
	int			mWidth;
	int			mLength;
	XMFLOAT3A	mScale;

};

class HeightMapGridMesh : public Mesh
{
public:
	HeightMapGridMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
		, int Xstart, int Zstart, int width, int length
		, XMFLOAT3A scale = XMFLOAT3A{ 1.0f,1.0f,1.0f }
		, XMFLOAT4A color = XMFLOAT4A{ 1.0f,1.0f,0.0f,0.0f }, void* context = nullptr);
	virtual ~HeightMapGridMesh();

	XMVECTOR XM_CALLCONV GetScale()const { return XMLoadFloat3A(&mScale); }
	int GetWidth()const { return mWidth; }
	int GetLength()const { return mLength; }

	virtual float GetHeight(int x, int z, void* context)const;
	virtual XMVECTOR XM_CALLCONV GetColor(int x, int z, void* context)const;

protected:
	int mWidth;
	int mLength;
	XMFLOAT3A mScale;

};
