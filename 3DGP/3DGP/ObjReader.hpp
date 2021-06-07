#pragma once
#include<string_view>
#include<fstream>
#include<iostream>
#include<regex>
#include<vector>
#include<filesystem>
#include<DirectXMath.h>
#include"Mesh.h"

using namespace DirectX;
using namespace std::literals::string_view_literals;

//∨∨∨ 수동 템플릿 ㅋㅋ
using STRUCT = DiffusedVertex;
#define POS mPosition 
#define NOR mDiffuse
#define TEX mTexture
//∧∧∧
template<typename VERTEX = STRUCT>
std::vector<VERTEX> LoadObj(std::string_view fileName, size_t reserve_hint = 10'0000)
{	
	using namespace std;
	vector<VERTEX> ret; ret.reserve(reserve_hint);
	vector<decltype(VERTEX::POS)>  v_list(1);  v_list.reserve(reserve_hint);
	vector<decltype(VERTEX::TEX)> vt_list(1); vt_list.reserve(reserve_hint);
	vector<decltype(VERTEX::NOR)> vn_list(1); vn_list.reserve(reserve_hint);

	ifstream objFile{ fileName };
	if (objFile.fail()) { cout << "there is no[" << fileName << "]\n"; return ret; }
	regex const vertexPattern{ R"(^(v|vn|vt)\s+(-?\d*.\d*)\s(-?\d*.\d*)\s?(-?\d*.\d*)?$)" };
	regex const fragmentPattern{ R"(^f\s+(\d*)/(\d*)/(\d*)\s(\d*)/(\d*)/(\d*)\s(\d*)/(\d*)/(\d*)(\s(\d*)/(\d*)/(\d*))?\s*$)" };
	string line;
	while (getline(objFile, line))
	{
		smatch matches;
		if (regex_match(line, matches, vertexPattern)) {
			if ("v" == matches[1]) { v_list.emplace_back(stof(matches[2]), stof(matches[3]), stof(matches[4])); }
			else if ("vn" == matches[1]) { vn_list.emplace_back(stof(matches[2]), stof(matches[3]), stof(matches[4]),1.0f); }
			else if ("vt" == matches[1]) { vt_list.emplace_back(stof(matches[2]), stof(matches[3])); }
		}
		else if (regex_match(line, matches, fragmentPattern))
		{
			VERTEX vertex;
			size_t const size[3]{ v_list.size(), vt_list.size(), vn_list.size() };
			for (int i = 0; i < 9;) {
				if (matches[++i] != "") assert(stoi(matches[i]) < size[0]), vertex.POS = v_list[stoi(matches[i])];
				if (matches[++i] != "") assert(stoi(matches[i]) < size[1]), vertex.TEX = vt_list[stoi(matches[i])];
				if (matches[++i] != "") assert(stoi(matches[i]) < size[2]), vertex.NOR = vn_list[stoi(matches[i])];
				assert(i < 10);
				ret.push_back(vertex);
			}
			if (false != matches[11]) { //사각형 123,134
				ret.push_back(*prev(ret.end(), 3));
				ret.push_back(vertex);
				if (matches[11] != "") assert(stoi(matches[11]) < size[0]), vertex.POS = v_list[stoi(matches[11])];
				if (matches[12] != "") assert(stoi(matches[12]) < size[1]), vertex.TEX = vt_list[stoi(matches[12])];
				if (matches[13] != "") assert(stoi(matches[13]) < size[2]), vertex.NOR = vn_list[stoi(matches[13])];
				ret.push_back(vertex);
			}
		}
	}
	ret.shrink_to_fit();
	return ret;
}
#undef POS
#undef NOR
#undef TEX

template<typename VERTEX = STRUCT>
void SaveMeshAsBinary(std::vector<VERTEX>& mesh, std::string_view fileName)
{
	using namespace std;
	ofstream objBinFile{ fileName, ios::binary };
	objBinFile.write(reinterpret_cast<char*>(mesh.data()), mesh.size() * sizeof(VERTEX));
}

template<typename VERTEX = STRUCT>
std::vector<VERTEX> LoadMeshFromBinary(std::string_view fileName)noexcept
{
	using namespace std;
	const size_t fileSize{ filesystem::file_size(fileName) };
	const size_t vertexCount{ fileSize / sizeof(VERTEX) };
	vector<VERTEX> ret(vertexCount);
	ifstream objBinFile{ fileName, ios::binary };
	objBinFile.read(reinterpret_cast<char*>(ret.data()), fileSize);
	return ret;
}

/*
* =======================================================
* matches2Float 	stof(matches[]);
* matches2Int 		stoi(matches[]);
*==================vertexPattern=========================
			cout << "line: " << matches[0] << "\n";
			cout << "v/vn/vt: " << matches[1] << "\n";
			cout << "1: " << matches[2] << "\n";
			cout << "2: " << matches[3] << "\n";
			cout << "3: " << matches[4] << "\n";
			stof(matches[]);
*==================fragmentPattern=========================
*			obj 파일 인덱스는 1부터 시작
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
			// maybe.. direct12에서는 삼각형만 쓰야..
			cout << "v 4: " << matches[11] << "\n";
			cout << "vt4: " << matches[12] << "\n";
			cout << "vn4: " << matches[13] << "\n";
			stoi(matches[]);
* =======================================================
*/