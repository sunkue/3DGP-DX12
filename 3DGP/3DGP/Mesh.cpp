#include "stdafx.h"
#include "Mesh.h"

////////////////////////////////////////////////

Mesh::Mesh(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList)
	: mSlot{ 0 }
	, mOffset{ 0 }
	, mStride{ 0 }
	, mVerticesCount{ 0 }
	, mIndicesCount{ 0 }
	, mStartIndex{ 0 }
	, mBaseVertex{ 0 }
	, mReferences{ 0 }
	, mPrimitiveToplogy{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST }
	, mVertexBuffer{ nullptr }
	, mVertexUploadBuffer{ nullptr }
	, mIndexBuffer{ nullptr }
	, mIndexUploadBuffer{ nullptr}
	, mVertexBufferView{}
	, mIndexBufferView{}
	, mOOBB{}
{

}

Mesh::~Mesh()
{
}

void Mesh::ReleaseUploadBuffers()
{
	mVertexUploadBuffer.Reset();
	mIndexUploadBuffer.Reset();
}

////////////////////////////////////////////////

void Mesh::Render(ID3D12GraphicsCommandList* commandList, UINT instanceCount)
{
	commandList->IASetVertexBuffers(mSlot, 1, &mVertexBufferView);
	commandList->IASetPrimitiveTopology(mPrimitiveToplogy);
	if (mIndexBuffer) {
		commandList->IASetIndexBuffer(&mIndexBufferView);
		commandList->DrawIndexedInstanced(mIndicesCount, instanceCount, 0, 0, 0);
	}
	else commandList->DrawInstanced(mVerticesCount, instanceCount, mOffset, 0);
}

void Mesh::Render(ID3D12GraphicsCommandList* commandList, UINT instanceCount, D3D12_VERTEX_BUFFER_VIEW instancingBufferView)
{
	D3D12_VERTEX_BUFFER_VIEW VBV[]{ mVertexBufferView,instancingBufferView };
	commandList->IASetVertexBuffers(mSlot, _countof(VBV), VBV);
	Render(commandList, instanceCount);
}

/////////////////////////////////////////////////////

TriangleMesh::TriangleMesh(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList)
	: Mesh{ device , commandList }
{
	constexpr UINT VC{ 3 };
	mVerticesCount = VC;
	mStride = sizeof(DiffusedVertex);
	mPrimitiveToplogy = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	
	DiffusedVertex vertices[VC]
	{
		  { XMFLOAT3A{  0.0f, +0.5f, 0.0f }, XMFLOAT4A{ Colors::Red }}
		, { XMFLOAT3A{ +0.5f, -0.5f, 0.0f }, XMFLOAT4A{ Colors::Green }}
		, { XMFLOAT3A{ -0.5f, -0.5f, 0.0f }, XMFLOAT4A{ Colors::Blue }}
	};
	
	UINT bufferSize{ mStride * VC };
	mVertexBuffer = CreateBufferResource(
		  device
		, commandList
		, vertices
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, mVertexUploadBuffer.GetAddressOf()
	);
	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = mStride;
	mVertexBufferView.SizeInBytes = bufferSize;
}

TriangleMesh::~TriangleMesh()
{

}

////////////////////////////////////////

CubeMeshDiffused::CubeMeshDiffused(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, float width, float height, float depth)
	: Mesh{ device, commandList }
{
	// 개노가다 우씨,, 오브젝트 리더 만들자.
	mVerticesCount = 8;
	mStride = sizeof(DiffusedVertex);
	float x = width  * 0.5f;
	float y = height * 0.5f;
	float z = depth	 * 0.5f;
	DiffusedVertex vertices[]{
		 {-x, +y ,-z, Colors::Gray}
		,{+x, +y ,-z, Colors::Red}
		,{+x, +y ,+z, Colors::Red}
		,{-x, +y ,+z, Colors::Blue}
		,{-x, -y ,-z, Colors::ForestGreen}
		,{+x, -y ,-z, Colors::ForestGreen}
		,{+x, -y ,+z, Colors::GreenYellow}
		,{-x, -y ,+z, Colors::GreenYellow}
	};
	UINT bufferSize{ mStride * mVerticesCount };
	mVertexBuffer = CreateBufferResource(
		  device
		, commandList
		, vertices
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, mVertexUploadBuffer.GetAddressOf());
	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = mStride;
	mVertexBufferView.SizeInBytes = bufferSize;

	mOOBB = { {0.0f,0.0f,0.0f},{x,y,z},{0.0f,0.0f,0.0f,1.0f} };
	
	mIndicesCount = 36;
	UINT indices[]{
		 3,1,0
		,2,1,3
		,0,5,4
		,1,5,0
		,3,4,7
		,0,4,3
		,1,6,5
		,2,6,1
		,2,7,6
		,3,7,2
		,6,4,5
		,7,4,6
	};
	bufferSize = sizeof(UINT) * mIndicesCount;
	mIndexBuffer = CreateBufferResource(
		  device
		, commandList
		, indices
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_INDEX_BUFFER
		, mIndexUploadBuffer.GetAddressOf());
	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	mIndexBufferView.SizeInBytes = bufferSize;
}

CubeMeshDiffused::~CubeMeshDiffused()
{

}

/////////////////////////////////////

AirplaneMeshDiffused::AirplaneMeshDiffused(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, float width, float height, float depth
	, XMVECTORF32 color)
	: Mesh{ device,commandList }
{
	mVerticesCount = 8;
	mStride = sizeof(DiffusedVertex);
	float x = width * 0.5f;
	float y = height * 0.5f;
	float z = depth * 0.5f;
	DiffusedVertex vertices[]{
		 {-x, +y ,-z, Colors::Aquamarine}
		,{+x, +y ,-z, Colors::Aquamarine + color}
		,{+x, +y ,+z, Colors::Aquamarine}
		,{-x, +y ,+z, Colors::Aquamarine + color}
		,{-x, -y ,-z, Colors::Aquamarine}
		,{+x, -y ,-z, Colors::Aquamarine}
		,{+x, -y ,+z, Colors::Aquamarine + color}
		,{-x, -y ,+z, Colors::Aquamarine + color}
	};
	UINT bufferSize{ mStride * mVerticesCount };
	mVertexBuffer = CreateBufferResource(
		device
		, commandList
		, vertices
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, mVertexUploadBuffer.GetAddressOf());
	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = mStride;
	mVertexBufferView.SizeInBytes = bufferSize;
	
	mOOBB = { {0.0f,0.0f,0.0f},{x,y,z},{0.0f,0.0f,0.0f,1.0f} };

	mIndicesCount = 36;
	UINT indices[]{
		 3,1,0
		,2,1,3
		,0,5,4
		,1,5,0
		,3,4,7
		,0,4,3
		,1,6,5
		,2,6,1
		,2,7,6
		,3,7,2
		,6,4,5
		,7,4,6
	};
	bufferSize = sizeof(UINT) * mIndicesCount;
	mIndexBuffer = CreateBufferResource(
		device
		, commandList
		, indices
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_INDEX_BUFFER
		, mIndexUploadBuffer.GetAddressOf());
	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	mIndexBufferView.SizeInBytes = bufferSize;
}

AirplaneMeshDiffused::~AirplaneMeshDiffused()
{

}

/////////////////////////////////////////////


HeightMapImage::HeightMapImage(
	  string_view fileName
	, int width
	, int length
	, XMFLOAT3A scale
)
	: mWidth{ width }
	, mLength{ length }
	, mScale{ scale }
{
	const int imageSize{ width * length };
	unique_ptr<BYTE[]> HMPixels = make_unique<BYTE[]>(imageSize);
	ifstream image{ fileName , ios::binary };
	image.read(reinterpret_cast<char*>(HMPixels.get()), imageSize);
	mHeightMapPixels = make_unique<BYTE[]>(imageSize);
	for (size_t y = 0; y < length; y++) {
		for (size_t x = 0; x < width; x++) {
			mHeightMapPixels[x + (((size_t)length - 1 - y) * width)]
				= HMPixels[x + ((size_t)y * width)];
		}
	}
}

HeightMapImage::~HeightMapImage()
{
	
}

#define WITH_APPROXIMATE_OPPOSITE_CORNER
float const HeightMapImage::GetHeight(const float fx,const float fz)const
{
	if (OutOfRange(fx, fz))return 0.0f;

	size_t const x = static_cast<size_t>(fx);
	size_t const z = static_cast<size_t>(fz);
	float const xFract = fx - x;
	float const zFract = fz - z;

	float BL{ static_cast<float>(mHeightMapPixels[(x + 0) + ((z + 0) * mWidth)]) };
	float BR{ static_cast<float>(mHeightMapPixels[(x + 1) + ((z + 0) * mWidth)]) };
	float TL{ static_cast<float>(mHeightMapPixels[(x + 0) + ((z + 1) * mWidth)]) };
	float TR{ static_cast<float>(mHeightMapPixels[(x + 1) + ((z + 1) * mWidth)]) };
#ifdef WITH_APPROXIMATE_OPPOSITE_CORNER
	const bool R2L{ (z % 2) != 0 };
	if (R2L) {
		if (xFract <= zFract)BR = BL + (TR - TL);
		else TL = TR + (BL - BR);
	}
	else {
		if (zFract < (1.0f - xFract))TR = TL + (BR - BL);
		else BL = TL + (BR - TR);
	}
#endif // WITH_APPROXIMATE_OPPOSITE_CORNER
	float const THeight{ TL * (1 - xFract) + TR * xFract };
	float const BHeight{ BL * (1 - xFract) + BR * xFract };
	float const Height{ BHeight * (1 - zFract) + THeight * zFract };
	return Height;
}

XMVECTOR const HeightMapImage::GetNormal(const int x,const int z)const
{
	if (OutOfRange(x, z))return { 0.0f,1.0f,0.0f };

	int const HMIndex{ x + (z * mWidth) };
	int const xHMAdd{ (x < mWidth - 1) ? (1) : (-1) };
	int const zHMAdd{ (z < mLength - 1) ? (mWidth) : (-mWidth) };
	float const y1{ static_cast<float>(mHeightMapPixels[static_cast<size_t>(HMIndex)]) * mScale.y };
	float const y2{ static_cast<float>(mHeightMapPixels[static_cast<size_t>(HMIndex) + xHMAdd] * mScale.y) };
	float const y3{ static_cast<float>(mHeightMapPixels[static_cast<size_t>(HMIndex) + zHMAdd] * mScale.y) };
	XMVECTOR const edge1{ 0.0f,y3 - y1,mScale.z };
	XMVECTOR const edge2{ mScale.x,y2 - y1,0.0f };
	XMVECTOR const Normal{ XMVector3Normalize(XMVector3Cross(edge1, edge2)) };
	return Normal;
}


/////////////////////////////////////////////


HeighMapGridMesh::HeighMapGridMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
	, int Xstart, int Zstart, int width, int length
	, XMFLOAT3A scale
	, XMVECTORF32 color
	, void* context
)
	: Mesh{ device,commandList }
	, mScale{ scale }
	, mWidth{ width }
	, mLength{ length }
{
	mPrimitiveToplogy = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	/* vertex */
	mVerticesCount = width * length;
	mStride = sizeof(DiffusedVertex);
	unique_ptr<DiffusedVertex[]> vertices = make_unique<DiffusedVertex[]>(mVerticesCount);
	float height{ 0.0f }, minH{ +FLT_MAX }, maxH{ -FLT_MAX };
	for (int i = 0, z = Zstart; z < (Zstart + length); z++) {
		for (int x = Xstart; x < (Xstart + width); x++, i++) {
			XMVECTOR pos{ x * scale.x,OnGetHeight(x,z,context),z * scale.z };
			XMVECTORF32 col;
			col.v = { OnGetColor(x,z,context).v + color.v };
			vertices[i] = DiffusedVertex{ pos, col };
			minH = min(minH, height);
			maxH = max(maxH, height);
		}
	}
	UINT bufferSize{ mStride * mVerticesCount };
	mVertexBuffer = CreateBufferResource(
		  device
		, commandList, vertices.get()
		, bufferSize, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, mVertexUploadBuffer.GetAddressOf());
	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.SizeInBytes = bufferSize;
	mVertexBufferView.StrideInBytes = mStride;

	/* index */
	mIndicesCount = (width * 2) * (length - 1) + (length - 1 - 1);
	unique_ptr<UINT[]> indices = make_unique<UINT[]>(mIndicesCount);
	for (int j = 0, z = 0; z < length - 1; z++) {
		if ((z % 2) == 0) {
			for (int x = 0; x < width; x++) {
				UINT const index{ static_cast<UINT>(x) + (z * width) };
				if ((x == 0) && (0 < z))indices[j++] = index;
				indices[j++] = index;
				indices[j++] = index + width;
			}
		}
		else {
			for (int x = width - 1; 0 <= x; x--) {
				UINT const index{ static_cast<UINT>(x) + (z * width) };
				if (x == (width - 1))indices[j++] = index;
				indices[j++] = index;
				indices[j++] = index + width;
			}
		}
	}
	bufferSize = sizeof(UINT) * mIndicesCount;
	mIndexBuffer = CreateBufferResource(
		  device
		, commandList
		, indices.get()
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_INDEX_BUFFER
		, mIndexUploadBuffer.GetAddressOf());
	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	mIndexBufferView.SizeInBytes = bufferSize;
}

HeighMapGridMesh::~HeighMapGridMesh()
{

}


float const HeighMapGridMesh::OnGetHeight(int x, int z, void* context)const
{
	HeightMapImage const* const HMImage{ reinterpret_cast<HeightMapImage*>(context) };
	BYTE const* const HMPixels{ HMImage->GetPixels() };
	XMFLOAT3A scale{ HMImage->GetScale() };
	int width{ HMImage->GetWidth() };
	float height{ HMPixels[x + (z * width)] * scale.y };
	return height;
}

XMVECTORF32 const HeighMapGridMesh::OnGetColor(int x, int z, void* context)const
{
	XMVECTOR lightDir{ -1.0f,1.0f,1.0f };
	lightDir = XMVector3Normalize(lightDir);
	XMVECTORF32 incidentLightColor{ 0.9f,0.8f,0.4f,1.0f };

	HeightMapImage const* const HMImage{ reinterpret_cast<HeightMapImage*>(context) };

	float fScale{ XMVectorGetX(XMVector3Dot(HMImage->GetNormal(x,z),lightDir)) };
	fScale += XMVectorGetX(XMVector3Dot(HMImage->GetNormal(x + 1, z), lightDir));
	fScale += XMVectorGetX(XMVector3Dot(HMImage->GetNormal(x + 1, z + 1), lightDir));
	fScale += XMVectorGetX(XMVector3Dot(HMImage->GetNormal(x, z + 1), lightDir));
	fScale = (fScale / 4.0f) + 0.05f;
	fScale = clamp(fScale, 0.25f, 1.0f);
	
	XMVECTORF32 color;
	color.v = { fScale * incidentLightColor };
	return color;
}
