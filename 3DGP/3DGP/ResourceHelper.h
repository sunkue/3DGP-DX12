#pragma once

ID3D12Resource* CreateBufferResource(
	  ID3D12Device* pd3dDevice
	, ID3D12GraphicsCommandList* pd3dCommandList
	, void* pData
	, UINT nBytes
	, D3D12_HEAP_TYPE d3dHeapType
	, D3D12_RESOURCE_STATES d3dResourceStates
	, ID3D12Resource** ppd3dUploadBuffer);


#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <fstream>
#include <string_view>
#include <filesystem>

using namespace std;

bool ReadObj(string_view fileName) {
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