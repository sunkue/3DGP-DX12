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
	, mReferences{ 0 }
	, mPrimitiveToplogy{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST }
	, mVertexBuffer{ nullptr }
	, mVertexUploadBuffer{ nullptr }
	, mVertexBufferView{}
{

}

Mesh::~Mesh()
{

}

void Mesh::ReleaseUploadBuffers()
{
	mVertexUploadBuffer.Reset();
}

////////////////////////////////////////////////

void Mesh::Render(ID3D12GraphicsCommandList* commandList)
{
	commandList->IASetPrimitiveTopology(mPrimitiveToplogy);
	commandList->IASetVertexBuffers(mSlot, 1, &mVertexBufferView);
	commandList->DrawInstanced(mVerticesCount, 1, mOffset, 0);
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
	: Mesh(device, commandList)
{
	mVerticesCount = 36;
	mStride = sizeof(DiffusedVertex);
	float x = width  * 0.5f;
	float y = height * 0.5f;
	float z = depth	 * 0.5f;
	// 개노가다 우씨,, 오브젝트 리더 만들자.
	DiffusedVertex vertices[36]{
		// f1
		 {-x, +y ,-z, RandomColor()}
		,{+x, +y ,-z, RandomColor()}
		,{+x, -y ,-z, RandomColor()}
		// f2
		,{-x, +y ,-z, RandomColor()}
		,{+x, -y ,-z, RandomColor()}
		,{-x, -y ,-z, RandomColor()}
		// u1
		,{-x, +y ,+z, RandomColor()}
		,{+x, +y ,+z, RandomColor()}
		,{+x, +y ,-z, RandomColor()}
		// u2
		,{-x, +y ,+z, RandomColor()}
		,{+x, +y ,-z, RandomColor()}
		,{-x, +y ,-z, RandomColor()}
		// b1
		,{-x, -y ,+z, RandomColor()}
		,{+x, -y ,+z, RandomColor()}
		,{+x, +y ,+z, RandomColor()}
		// b2
		,{-x, -y ,+z, RandomColor()}
		,{+x, +y ,+z, RandomColor()}
		,{-x, +y ,+z, RandomColor()}
		// d1
		,{-x, -y ,-z, RandomColor()}
		,{+x, -y ,-z, RandomColor()}
		,{+x, -y ,+z, RandomColor()}
		// d2
		,{-x, -y ,-z, RandomColor()}
		,{+x, -y ,+z, RandomColor()}
		,{-x, -y ,+z, RandomColor()}
		// l1
		,{-x, +y ,+z, RandomColor()}
		,{-x, +y ,-z, RandomColor()}
		,{-x, -y ,-z, RandomColor()}
		// l2
		,{-x, +y ,+z, RandomColor()}
		,{-x, -y ,-z, RandomColor()}
		,{-x, -y ,+z, RandomColor()}
		// r1
		,{+x, +y ,-z, RandomColor()}
		,{+x, +y ,+z, RandomColor()}
		,{+x, -y ,+z, RandomColor()}
		// r2
		,{+x, +y ,-z, RandomColor()}
		,{+x, -y ,+z, RandomColor()}
		,{+x, -y ,-z, RandomColor()}
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
}

CubeMeshDiffused::~CubeMeshDiffused()
{

}