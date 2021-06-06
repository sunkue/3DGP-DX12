#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

///////
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


////////
#include <iostream>
#include <cassert>
#include <algorithm>
#include <string>
#include <vector>
#include <chrono>
#include <list>
#include <array>
#include <cassert>
#include <fstream>
#include <random>
#include <thread>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <ranges>
#include "d3dx12.h"
#include "D3DX12Helper.h"
#include "ResourceHelper.h"

using namespace std;
using namespace std::chrono;
using namespace std::literals::string_view_literals;
using TimePoint = steady_clock::time_point;
constexpr UINT RFR = 144;

#undef min
#undef max

class RANDOM
{
public:
	random_device rd;
	default_random_engine dre{ rd() };
	uniform_real_distribution<float> urd{ 0.0f,1.0f };
	float operator()()
	{
		return urd(dre);
	}
};

extern RANDOM Rand;

inline XMVECTORF32 RandomColor()
{
	return { Rand(),Rand(),Rand(),1.0f };
}


constexpr float EPSILON{ 1.0e-10f };

inline bool const IsZero(const float value) { return fabs(value) < EPSILON; }
inline bool const IsZero(const XMVECTOR value) { 
	return IsZero(XMVectorGetX(value)) 
		&& IsZero(XMVectorGetY(value)) 
		&& IsZero(XMVectorGetZ(value));
}
inline bool const IsEqual(const float a, const float b) { return IsZero(a - b); }
inline bool const IsEqual(const XMVECTOR a, const XMVECTOR b){
	XMVECTOR val = a - b;
	return IsZero(XMVectorGetX(val)) 
		&& IsZero(XMVectorGetY(val))
		&& IsZero(XMVectorGetZ(val));
}
extern constexpr XMVECTOR yAxis{ 0.0f,1.0f,0.0f };

inline float ToSec(milliseconds time) {
	return time.count() / 1000.0f;
}

#define _WITH_SWAPCHAIN_FULLSCREEN_STATE
//#define WITH_TERRAIN_PARITION
/*
	목표 일주일 3챕터, 8주

	현재 chapter 6 / 따라하기 15

	06/03
   ~06/21


*/
