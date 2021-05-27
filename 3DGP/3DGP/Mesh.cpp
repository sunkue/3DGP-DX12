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
		 {-x, +y ,-z, Colors::Aquamarine + color}
		,{+x, +y ,-z, Colors::Aquamarine + color}
		,{+x, +y ,+z, Colors::Aquamarine + color}
		,{-x, +y ,+z, Colors::Aquamarine + color}
		,{-x, -y ,-z, Colors::Aquamarine + color}
		,{+x, -y ,-z, Colors::Aquamarine + color}
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