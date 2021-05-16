#include "stdafx.h"
#include "ResourceHelper.h"

ID3D12Resource* CreateBufferResource(
	  ID3D12Device* pd3dDevice
	, ID3D12GraphicsCommandList* pd3dCommandList
	, void* pData
	, UINT nBytes
	, D3D12_HEAP_TYPE d3dHeapType
	, D3D12_RESOURCE_STATES d3dResourceStates
	, ID3D12Resource** ppd3dUploadBuffer)
{
ID3D12Resource* pd3dBuffer = nullptr;

CD3DX12_HEAP_PROPERTIES d3dHeapPropertiesDesc{ d3dHeapType };
CD3DX12_RESOURCE_DESC d3dResourceDesc{
	  D3D12_RESOURCE_DIMENSION_BUFFER
	, 0
	, nBytes
	, 1
	, 1
	, 1
	, DXGI_FORMAT_UNKNOWN
	, 1
	, 0
	, D3D12_TEXTURE_LAYOUT_ROW_MAJOR
	, D3D12_RESOURCE_FLAG_NONE
};

D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD)
d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK)
d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
ThrowIfFailed(pd3dDevice->CreateCommittedResource(
	&d3dHeapPropertiesDesc
	, D3D12_HEAP_FLAG_NONE
	, &d3dResourceDesc
	, d3dResourceInitialStates
	, nullptr
	, IID_PPV_ARGS(&pd3dBuffer)));
if (pData)
{
	switch (d3dHeapType)
	{
	case D3D12_HEAP_TYPE_DEFAULT:
	{
		if (ppd3dUploadBuffer)
		{
			//업로드 버퍼를 생성한다. 
			d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
			pd3dDevice->CreateCommittedResource(
				&d3dHeapPropertiesDesc
				, D3D12_HEAP_FLAG_NONE
				, &d3dResourceDesc
				, D3D12_RESOURCE_STATE_GENERIC_READ
				, nullptr
				, IID_PPV_ARGS(ppd3dUploadBuffer));
			//업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사한다. 
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = nullptr;
			(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			(*ppd3dUploadBuffer)->Unmap(0, nullptr);
			//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다. 
			pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);
			CD3DX12_RESOURCE_BARRIER d3dResourceBarrier{
				CD3DX12_RESOURCE_BARRIER::Transition(
				  pd3dBuffer
				, D3D12_RESOURCE_STATE_COPY_DEST
				, d3dResourceStates)
			};

			pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
		}break;
	}
	case D3D12_HEAP_TYPE_UPLOAD:
	{
		D3D12_RANGE d3dReadRange = { 0, 0 };
		UINT8* pBufferDataBegin = nullptr;
		pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
		memcpy(pBufferDataBegin, pData, nBytes);
		pd3dBuffer->Unmap(0, nullptr);
		break;
	}
	case D3D12_HEAP_TYPE_READBACK:
		break;
	}
}
return(pd3dBuffer);
}