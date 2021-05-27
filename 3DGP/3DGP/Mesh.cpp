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
		 {-x, +y ,-z, Colors::WhiteSmoke + color}
		,{+x, +y ,-z, Colors::Black + color}
		,{+x, +y ,+z, Colors::Black + color}
		,{-x, +y ,+z, Colors::Black + color}
		,{-x, -y ,-z, Colors::WhiteSmoke + color}
		,{+x, -y ,-z, Colors::Black + color}
		,{+x, -y ,+z, Colors::Black + color}
		,{-x, -y ,+z, Colors::Black + color}
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

/////////////////////////////////


HeightMapImage::HeightMapImage(
	LPCTSTR fileName
	, int width
	, int length
	, XMFLOAT3A scale
)
	: mWidth{ width }
	, mLength{ length }
	, mScale{ scale }
	, mHeightMapPixels{ nullptr }
{
	const int PIXEL_COUNT{ width * length };
	BYTE* heightMapPixels{ new BYTE[PIXEL_COUNT] };
	HANDLE file{ CreateFile(fileName,GENERIC_READ,0,nullptr,OPEN_EXISTING
		,FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY,nullptr) };
	DWORD bytesRead;
	if (!ReadFile(file, heightMapPixels, PIXEL_COUNT, &bytesRead, nullptr)) {
		cout << "NOE";
		assert(0);
		throw exception("noHeightMapImageFile");
	}
	CloseHandle(file);

	mHeightMapPixels = new BYTE[PIXEL_COUNT];
	for (int y = 0; y < length; ++y) {
		for (int x = 0; x < width; ++x) {
			assert(x + ((length - 1 - y) * width) < PIXEL_COUNT);
			mHeightMapPixels[x + ((length - 1 - y) * width)] = heightMapPixels[x + (y * width)];

		}
	}
	delete[] heightMapPixels;
}

HeightMapImage::~HeightMapImage()
{
	delete[] mHeightMapPixels;
	mHeightMapPixels = nullptr;
}

XMVECTOR HeightMapImage::GetHeightNormal(const int x, const int z)const
{
	if ((x < 0.0f) || (z < 0.0f) || (mWidth <= x) || (mLength <= z))
		return { 0.0f,1.0f,0.0f };
	int HMIndex{ x + (z * mWidth) };
	int HMAddX{ (x < (mWidth - 1)) ? (1) : (-1) };
	int HMAddZ{ (z < (mLength - 1)) ? (mWidth) : (-mWidth) };

	float y1{ (float)mHeightMapPixels[HMIndex] * mScale.y };
	float y2{ (float)mHeightMapPixels[HMIndex + HMAddX] * mScale.y };
	float y3{ (float)mHeightMapPixels[HMIndex + HMAddZ] * mScale.y };

	XMVECTOR edge1{ XMVector3Normalize({0.0f, y3 - y1, mScale.z}) };
	XMVECTOR edge2{ XMVector3Normalize({mScale.x, y2 - y1, 0.0f }) };
	
	return XMVector3Normalize(XMVector3Cross(edge1, edge2));
}

#define WITH_APPROXIMATE_OPPOSITE_CORNER
float HeightMapImage::GetHeight(const float fx, const float fz)const
{
	if ((fx < 0.0f) || (fz < 0.0f) || (mWidth <= fx) || (mLength <= fz))
		return 0.0f;
	int x{ static_cast<int>(fx) };
	int z{ static_cast<int>(fz) };
	float xPercent{ fx - x };
	float zPercent{ fz - z };

	float BL{ (float)mHeightMapPixels[x + (z * mWidth)] };
	float BR{ (float)mHeightMapPixels[(x + 1) + (z * mWidth)] };
	float TL{ (float)mHeightMapPixels[x + ((z + 1) * mWidth)] };
	float TR{ (float)mHeightMapPixels[(x + 1) + ((z + 1) * mWidth)] };
#ifdef WITH_APPROXIMATE_OPPOSITE_CORNER
	bool R2L{ (z % 2) != 0 };
	if (R2L) {
		if (xPercent <= zPercent)BR = BL + (TR - TL);
		else TL = TR + (BL - BR);
	}
	else {
		if (zPercent < (1.0f - xPercent))TR = TL + (BR - BL);
		else BL = TL + (BR - TR);
	}
#endif
	assert(lerp(TL, TR, xPercent) == (TL * (1 - xPercent) + TR * xPercent));
	float TH{ lerp(TL,TR,xPercent) };
	float BH{ lerp(BL,BR,xPercent) };
	float Height{ lerp(BH,TH,zPercent) };
	return Height;
}

///////////////////////////////////////

HeightMapGridMesh::HeightMapGridMesh(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, int xStart, int zStart, int width, int length
	, XMFLOAT3A scale, XMFLOAT4A color, void* context
)
	: Mesh{ device,commandList }
	, mWidth{ width }
	, mLength{ length }
	, mScale{ scale }
{
	mVerticesCount = width * length;
	mStride = sizeof(DiffusedVertex);
	mPrimitiveToplogy = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
	DiffusedVertex* vertices{ new DiffusedVertex[mVerticesCount] };
	float H{ 0.0f };
	float minH{ +FLT_MAX };
	float maxH{ -FLT_MAX };
	for (int i = 0, z = zStart; z < (zStart + length); ++z) {
		for (int x = xStart; x < (xStart + width); ++x, ++i)
		{
			assert(i < mVerticesCount);
			XMVECTOR pos{ (x * scale.x), GetHeight(x,z,context), (z * scale.z) };
			XMVECTOR col{ GetColor(x,z,context) + XMLoadFloat4A(&color) };
			vertices[i] = DiffusedVertex{ pos, col };
			minH = min(minH, H);
			maxH = max(maxH, H);
		}
	}
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
	delete[] vertices;

	mIndicesCount = ((width * 2) * (length - 1)) + ((length - 1) - 1);
	UINT* indices{ new UINT[mIndicesCount] };
	for (UINT j = 0, z = 0; z < length - 1; ++z) {
		if (z % 2 == 0) {
			for (int x = 0; x < width; ++x) {
				UINT index{ x + (z * width) };
				if ((x == 0) && (0 < z))indices[j++] = index;
				indices[j++] = index;
				indices[j++] = index + width;
				assert(j < mIndicesCount);
			}
		}
		else {
			for (int x = width - 1; 0 <= x; --x) {
				UINT index{ x + (z * width) };
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
		, indices
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_INDEX_BUFFER
		, mIndexUploadBuffer.GetAddressOf());
	mIndexBufferView.BufferLocation = mIndexBuffer->GetGPUVirtualAddress();
	mIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	mIndexBufferView.SizeInBytes = bufferSize;
	delete[] indices;
}

HeightMapGridMesh::~HeightMapGridMesh()
{

}

float HeightMapGridMesh::GetHeight(int x, int z, void* context)const
{
	HeightMapImage* HM{ reinterpret_cast<HeightMapImage*>(context) };
	BYTE* HMPixels{ HM->GetPixels() };
	XMVECTOR scale{ HM->GetScale() };
	int width{ HM->GetWidth() };
	float H{ HMPixels[x + (z * width)] * XMVectorGetY(scale) };
	return H;
}

XMVECTOR HeightMapGridMesh::GetColor(int x, int z, void* context)const
{
	XMVECTOR lightDir{ -1.0f, 1.0f, 1.0f };
	lightDir = XMVector3Normalize(lightDir);
	HeightMapImage* HM{ reinterpret_cast<HeightMapImage*>(context) };
	XMVECTOR incidentLight{ 0.9f, 0.8f, 0.4f, 1.0f };
	XMVECTOR scaleTemp = XMVector3Dot(HM->GetHeightNormal(x, z), lightDir);
	scaleTemp += XMVector3Dot(HM->GetHeightNormal(x + 1, z), lightDir);
	scaleTemp += XMVector3Dot(HM->GetHeightNormal(x + 1, z + 1), lightDir);
	scaleTemp += XMVector3Dot(HM->GetHeightNormal(x, z + 1), lightDir);
	float scale{ XMVectorGetX(scaleTemp) };
	scale = (scale / 4.0f) + 0.05f;
	scale = clamp(scale, 0.25f, 1.0f);
	//fScale 은 조명 반사 비율.
	XMVECTOR col{ incidentLight * scale };
	return col;
}
