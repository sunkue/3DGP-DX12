#include "stdafx.h"
#include "Mesh.h"

////////////////////////////////////////////////

VertexBufferData::VertexBufferData(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, Mesh_Info meshInfo
	, vector<D3D12_VERTEX_BUFFER_VIEW>& out
)
{
	m_verticesCount = static_cast<UINT>(meshInfo.v.size());
	UINT stride = sizeof(decltype(meshInfo.v)::value_type);
	D3D12_VERTEX_BUFFER_VIEW VBV;
	UINT bufferSize{ stride * m_verticesCount };
	m_vertexBuffer = CreateBufferResource(
		device
		, commandList
		, meshInfo.v.data()
		, bufferSize
		, D3D12_HEAP_TYPE_DEFAULT
		, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
		, m_vertexUploadBuffer.GetAddressOf());
	VBV.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	VBV.StrideInBytes = stride;
	VBV.SizeInBytes = bufferSize;
	out.push_back(VBV);
	m_OOBB = { meshInfo.center,meshInfo.extents,{0.0f,0.0f,0.0f,1.0f} };
}


////////////////////////////////////////////////


Mesh::Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, vector<Mesh_Info> meshInfo, BoundingOrientedBox OOBB)
	: m_Big_OOBB{ OOBB }
{
	m_vertexBufferView.reserve(meshInfo.size());
	for (const auto& mesh : meshInfo)
	{
		m_meshes.emplace_back(new VertexBufferData(device, commandList, mesh, m_vertexBufferView));
	}
}

void Mesh::Render(ID3D12GraphicsCommandList* commandList, UINT instanceCount)
{
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(Meterial) / sizeof(float), &m_meterial, 16);//
	commandList->IASetVertexBuffers(m_slot, m_vertexBufferView.size(), m_vertexBufferView.data());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//
	float offset{ 0 };
	UINT verticesCount;
	for (const auto& mesh : m_meshes) {
		verticesCount = mesh->GetVerticesCount();
		commandList->DrawInstanced(verticesCount, instanceCount, offset, 0);
		offset += verticesCount;
	}
}

/////////////////////////////////////////////////////
//