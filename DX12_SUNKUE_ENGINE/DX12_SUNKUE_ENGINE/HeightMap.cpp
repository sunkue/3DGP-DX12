#include "stdafx.h"
#include "HeightMap.h"



HeightMapImage::HeightMapImage(
	string_view fileName
	, int width
	, int length
	, XMFLOAT3 scale
)
	: m_width{ width }
	, m_length{ length }
	, m_scale{ scale }
{
	const int imageSize{ width * length };
	unique_ptr<BYTE[]> HMPixels = make_unique<BYTE[]>(imageSize);
	ifstream image{ fileName , ios::binary };
	image.read(reinterpret_cast<char*>(HMPixels.get()), imageSize);
	m_heightMapPixels = make_unique<BYTE[]>(imageSize);
	for (size_t y = 0; y < length; y++) {
		for (size_t x = 0; x < width; x++) {
			m_heightMapPixels[x + (((size_t)length - 1 - y) * width)]
				= HMPixels[x + ((size_t)y * width)];
		}
	}
}

#define WITH_APPROXIMATE_OPPOSITE_CORNER
float HeightMapImage::GetHeight(float fx, float fz)const
{
	if (OutOfRange(fx, fz))return -50.0f;

	size_t const x = static_cast<size_t>(fx);
	size_t const z = static_cast<size_t>(fz);
	float const xFract = fx - x;
	float const zFract = fz - z;

	float BL{ static_cast<float>(m_heightMapPixels[(x + 0) + ((z + 0) * m_width)]) };
	float BR{ static_cast<float>(m_heightMapPixels[(x + 1) + ((z + 0) * m_width)]) };
	float TL{ static_cast<float>(m_heightMapPixels[(x + 0) + ((z + 1) * m_width)]) };
	float TR{ static_cast<float>(m_heightMapPixels[(x + 1) + ((z + 1) * m_width)]) };
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

XMVECTOR HeightMapImage::GetNormal(int x, int z)const
{
	if (OutOfRange(x, z))return { 0.0f,1.0f,0.0f };

	int const HMIndex{ x + (z * m_width) };
	int const xHMAdd{ (x < m_width - 1) ? (1) : (-1) };
	int const zHMAdd{ (z < m_length - 1) ? (m_width) : (-m_width) };
	float const y1{ static_cast<float>(m_heightMapPixels[static_cast<size_t>(HMIndex)]) * m_scale.y };
	float const y2{ static_cast<float>(m_heightMapPixels[static_cast<size_t>(HMIndex) + xHMAdd] * m_scale.y) };
	float const y3{ static_cast<float>(m_heightMapPixels[static_cast<size_t>(HMIndex) + zHMAdd] * m_scale.y) };
	XMVECTOR const edge1{ 0.0f,y3 - y1,m_scale.z };
	XMVECTOR const edge2{ m_scale.x,y2 - y1,0.0f };
	XMVECTOR const Normal{ XMVector3Normalize(XMVector3Cross(edge1, edge2)) };
	return Normal;
}


/////////////////////////////////////////////


HeighMapGridMesh::HeighMapGridMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList
	, int Xstart, int Zstart, int width, int length
	, XMFLOAT3 scale
	, XMVECTORF32 color
	, void* context
)
	: VertexBufferData{ device,commandList }
	, m_scale{ scale }
	, m_width{ width }
	, m_length{ length }
{
	m_primitiveToplogy = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	m_verticesCount = width * length;
	m_stride = sizeof(Vertex);
	unique_ptr<Vertex[]> vertices = make_unique<Vertex[]>(m_verticesCount);
	HeightMapImage const* const HMImage{ reinterpret_cast<HeightMapImage*>(context) };
	for (int i = 0, z = Zstart; z < (Zstart + length); z++) {
		for (int x = Xstart; x < (Xstart + width); x++, i++) {
			XMVECTOR pos{ scale.x * x,OnGetHeight(x,z,context),scale.z * z };
			XMStoreFloat3(&vertices[i].pos, pos);
			XMStoreFloat3(&vertices[i].nor, HMImage->GetNormal(x, z));
		}
	}

	UINT bufferSize{ m_stride * m_verticesCount };
	m_vertexBuffer = CreateBufferResource(
		device
		, commandList, vertices.get()
		, bufferSize, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, m_vertexUploadBuffer.GetAddressOf());
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = bufferSize;
	m_vertexBufferView.StrideInBytes = m_stride;

	/// index 
	m_indicesCount = (width * 2) * (length - 1) + (length - 1 - 1);
	unique_ptr<UINT[]> indices = make_unique<UINT[]>(m_indicesCount);
	for (int j = 0, z = 0; z < length - 1; z++) {
		if ((z % 2) == 0) {
			for (int x = 0; x < width; x++) {
				const UINT index{ static_cast<UINT>(x) + (z * width) };
				if ((x == 0) && (0 < z))indices[j++] = index;
				indices[j++] = index;
				indices[j++] = index + width;
			}
		}
		else {
			for (int x = width - 1; 0 <= x; x--) {
				const UINT index{ static_cast<UINT>(x) + (z * width) };
				if (x == (width - 1))indices[j++] = index;
				indices[j++] = index;
				indices[j++] = index + width;
			}
		}
	}
	bufferSize = sizeof(UINT) * m_indicesCount;
	m_indexBuffer = CreateBufferResource(
		device
		, commandList
		, indices.get()
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_INDEX_BUFFER
		, m_indexUploadBuffer.GetAddressOf());
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = bufferSize;

}


float const HeighMapGridMesh::OnGetHeight(int x, int z, void* context)const
{
	HeightMapImage const* const HMImage{ reinterpret_cast<HeightMapImage*>(context) };
	BYTE const* const HMPixels{ HMImage->GetPixels() };
	const auto scale{ HMImage->GetScale() };
	const int width{ HMImage->GetWidth() };
	const float height{ HMPixels[x + (z * width)] * scale.y };
	return height;
}

///////////////////////////////