#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
//
#include <wrl.h>
#include <shellapi.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectxMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DXGIDebug.h>
using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

//
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <list>
#include <array>
#include <fstream>
#include <thread>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <ranges>
#include <map>
#include <cassert>
#include <type_traits>
#include <concepts>
//
#include "d3dx12.h"
#include "D3DX12Helper.h"
#include "ResourceHelper.h"
#include "Interface.h"
#include "MyDXHelper.hpp"
#include "SUNKUE.hpp"
using namespace std;
using namespace std::chrono;
using namespace std::literals::string_view_literals;
using namespace SUNKUE;
constexpr UINT RFR = 144;

#undef min
#undef max


constexpr float EPSILON{ 1.0e-10f };

inline bool const IsZero(const float value) { return fabs(value) < EPSILON; }
inline bool const IsZero(const XMVECTOR value) {
	return IsZero(XMVectorGetX(value))
		&& IsZero(XMVectorGetY(value))
		&& IsZero(XMVectorGetZ(value));
}
inline bool const IsEqual(const float a, const float b) { return IsZero(a - b); }
inline bool const IsEqual(const XMVECTOR a, const XMVECTOR b) {
	XMVECTOR val = a - b;
	return IsZero(XMVectorGetX(val))
		&& IsZero(XMVectorGetY(val))
		&& IsZero(XMVectorGetZ(val));
}

