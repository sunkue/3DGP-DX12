#include "stdafx.h"
#include "Mesh.h"

////////////////////////////////////////////////

MeshUnit::MeshUnit(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, Mesh_Info& meshInfo
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


Mesh::Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,vector<Mesh_Info>& meshInfo)
{
	m_vertexBufferView.reserve(meshInfo.size());
	for (auto& mesh : meshInfo)
	{
		m_meshes.push_back(make_shared<MeshUnit>(device, commandList, mesh, m_vertexBufferView));
	}
}

void Mesh::Render(ID3D12GraphicsCommandList* commandList, UINT instanceCount)
{	
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(remove_pointer_t<decltype(m_meterial)>) / sizeof(__int32), &m_meterial, 16);// 각 mesh 별로 셋 하고 싶으면 아래로 내리셈
	commandList->IASetVertexBuffers(m_slot, m_vertexBufferView.size(), m_vertexBufferView.data());
	UINT offset{ 0 };
	UINT verticesCount;
	for (const auto& mesh : m_meshes) {
		verticesCount = mesh->GetVerticesCount();
		commandList->DrawInstanced(verticesCount, instanceCount, offset, 0);
		offset += verticesCount;
	}
}

/////////////////////////////////////////////////////
//