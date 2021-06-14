#pragma once

class Vertex
{
public:
	Vertex() : mPosition{ 0.0f,0.0f,0.0f } {}
	Vertex(float x, float y, float z) : mPosition{ x,y,z } {}
	explicit Vertex(XMFLOAT3A pos) : mPosition{ pos.x,pos.y,pos.z } {}
	explicit Vertex(XMVECTOR pos) { XMStoreFloat3A(&mPosition, pos); }


	XMFLOAT3A mPosition{};
	XMFLOAT3A mNormal{};
	XMFLOAT2A mTexture{};
};

struct Meterial
{
	XMFLOAT4A m_ambient{};
	XMFLOAT4A m_diffuse{};
	XMFLOAT4A m_specular{};
	XMFLOAT4A m_emessive{};
	float m_specualrPower{};
};

class Mesh
{
public:
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<Vertex>& v);
	virtual ~Mesh();

public:
	void AddRef()	{ ++mReferences; assert(0 < mReferences); }
	void Release()	{ assert(0 < mReferences); /*if (--mReferences == 0)delete this;*/ }

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

public:
	void SetMeterial(XMFLOAT4A a, XMFLOAT4A d, float sp = 600.0f, XMFLOAT4A s = { 0.0f,0.0f,0.0f,0.0f }) {
		m_meterial.m_ambient = a;
		m_meterial.m_diffuse = d;
		m_meterial.m_specular = s;
		m_meterial.m_specualrPower = sp;
	}
	void SetEmessive(XMFLOAT4A e) { m_meterial.m_emessive = e; }
	Meterial GetMeterial() { return m_meterial; }
protected:
	Meterial m_meterial;
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