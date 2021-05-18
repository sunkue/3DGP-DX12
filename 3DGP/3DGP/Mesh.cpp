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