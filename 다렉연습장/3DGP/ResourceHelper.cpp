#include "stdafx.h"
#include "ResourceHelper.h"
#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <fstream>
#include <filesystem>

ID3D12Resource* CreateBufferResource(
	  ID3D12Device* device
	, ID3D12GraphicsCommandList* commandList
	, void* data
	, UINT dataSize
	, D3D12_HEAP_TYPE heapType
	, D3D12_RESOURCE_STATES resourceStates
	, ID3D12Resource** uploadBuffer)
{
	ID3D12Resource* buffer = nullptr;

	CD3DX12_HEAP_PROPERTIES heapPropertiesDesc{ heapType };
	CD3DX12_RESOURCE_DESC resourceDesc{
		  D3D12_RESOURCE_DIMENSION_BUFFER
		, 0
		, dataSize
		, 1
		, 1
		, 1
		, DXGI_FORMAT_UNKNOWN
		, 1
		, 0
		, D3D12_TEXTURE_LAYOUT_ROW_MAJOR
		, D3D12_RESOURCE_FLAG_NONE
	};

	D3D12_RESOURCE_STATES resourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (heapType == D3D12_HEAP_TYPE_UPLOAD)
		resourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (heapType == D3D12_HEAP_TYPE_READBACK)
		resourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	ThrowIfFailed(device->CreateCommittedResource(
		  &heapPropertiesDesc
		, D3D12_HEAP_FLAG_NONE
		, &resourceDesc
		, resourceInitialStates
		, nullptr
		, IID_PPV_ARGS(&buffer)));
	if (data)
	{
		switch (heapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (uploadBuffer)
			{
				//업로드 버퍼를 생성한다. 
				heapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				ThrowIfFailed(device->CreateCommittedResource(
					&heapPropertiesDesc
					, D3D12_HEAP_FLAG_NONE
					, &resourceDesc
					, D3D12_RESOURCE_STATE_GENERIC_READ
					, nullptr
					, IID_PPV_ARGS(uploadBuffer)));
				//업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사한다. 
				D3D12_RANGE d3dReadRange = { 0, 0 };
				UINT8* pBufferDataBegin = nullptr;
				(*uploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
				memcpy(pBufferDataBegin, data, dataSize);
				(*uploadBuffer)->Unmap(0, nullptr);
				//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다. 
				commandList->CopyResource(buffer, *uploadBuffer);
				D3D12_RESOURCE_BARRIER d3dResourceBarrier{
					CD3DX12_RESOURCE_BARRIER::Transition(
					  buffer
					, D3D12_RESOURCE_STATE_COPY_DEST
					, resourceStates)
				};

				commandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = nullptr;
			buffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, data, dataSize);
			buffer->Unmap(0, nullptr);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return buffer;
}




bool ReadObj(std::string_view fileName) 
{
	using namespace std;
	ifstream objFile{ fileName };
	if (!objFile) { cout << "there is no[" << fileName << "]\n"; return false; }
	regex vertexPattern{ R"(^(v|vn|vt)\s(-?\d*.\d*)\s(-?\d*.\d*)\s(-?\d*.\d*)$)" };
	regex fragmentPattern{ R"(^f\s(\d*)/(\d*)/(\d*)\s(\d*)/(\d*)/(\d*)\s(\d*)/(\d*)/(\d*)(\s(\d*)/(\d*)/(\d*))?$)" };
	vector<string> mismatch;
	string line;
	while (getline(objFile, line))
	{
		smatch matches;
		if (regex_match(line, matches, vertexPattern)) {
			/*
			cout << "line: " << matches[0] << "\n";
			cout << "v/vn/vt: " << matches[1] << "\n";
			cout << "1: " << matches[2] << "\n";
			cout << "2: " << matches[3] << "\n";
			cout << "3: " << matches[4] << "\n";
			stof(matches[]);
			*/
		}
		else if (regex_match(line, matches, fragmentPattern))
		{
			/*
			cout << "line: " << matches[0] << "\n";
			cout << "v 1: " << matches[1] << "\n";
			cout << "vt1: " << matches[2] << "\n";
			cout << "vn1: " << matches[3] << "\n";
			cout << "v 2: " << matches[4] << "\n";
			cout << "vt2: " << matches[5] << "\n";
			cout << "vn2: " << matches[6] << "\n";
			cout << "v 3: " << matches[7] << "\n";
			cout << "vt3: " << matches[8] << "\n";
			cout << "vn3: " << matches[9] << "\n";
			// maybe
			cout << "v 4: " << matches[11] << "\n";
			cout << "vt4: " << matches[12] << "\n";
			cout << "vn4: " << matches[13] << "\n";
			stoi(matches[]);
			*/
		}
		else
		{
			mismatch.emplace_back(move(line));
		}
	}
	for (const auto& miss : mismatch)cout << miss << "\n";
	return true;
}