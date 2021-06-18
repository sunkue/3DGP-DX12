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

Mesh::Mesh(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, Mesh_Info meshInfo
)
	: mSlot{ 0 }
	, mOffset{ 0 }
	, mIndicesCount{ 0 }
	, mStartIndex{ 0 }
	, mBaseVertex{ 0 }
	, mPrimitiveToplogy{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST }
	, mIndexBuffer{ nullptr }
	, mIndexUploadBuffer{ nullptr }
	, mIndexBufferView{}
	, mOOBB{}
{
	mVerticesCount = static_cast<UINT>(meshInfo.v.size());
	mStride = sizeof(*meshInfo.v.data());
	UINT bufferSize{ mStride * mVerticesCount };
	mVertexBuffer = CreateBufferResource(
		device
		, commandList
		, meshInfo.v.data()
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, mVertexUploadBuffer.GetAddressOf());
	mVertexBufferView.BufferLocation = mVertexBuffer->GetGPUVirtualAddress();
	mVertexBufferView.StrideInBytes = mStride;
	mVertexBufferView.SizeInBytes = bufferSize;

	mOOBB = { meshInfo.center,meshInfo.extents,{0.0f,0.0f,0.0f,1.0f} };
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
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(Meterial) / sizeof(float), &m_meterial, 16);

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
	if (OutOfRange(fx, fz))return -50.0f;

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
#undef WITH_APPROXIMATE_OPPOSITE_CORNER
#endif // WITH_APPROXIMATE_OPPOSITE_CORNER
	float const THeight{ lerp(TL, TR, xFract) };
	float const BHeight{ lerp(BL, BR, xFract) };
	float const Height{ lerp(BHeight, THeight, zFract) };
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

	mVerticesCount = width * length;
	mStride = sizeof(Vertex);
	unique_ptr<Vertex[]> vertices = make_unique<Vertex[]>(mVerticesCount);
	HeightMapImage const* const HMImage{ reinterpret_cast<HeightMapImage*>(context) };
	for (int i = 0, z = Zstart; z < (Zstart + length); z++) {
		for (int x = Xstart; x < (Xstart + width); x++, i++) {
			XMVECTOR pos{ scale.x * x,OnGetHeight(x,z,context),scale.z * z };
			XMStoreFloat3(&vertices[i].pos, pos);
			XMStoreFloat3(&vertices[i].nor, HMImage->GetNormal(x, z));
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

	/// index 
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

/////////////////////////////////