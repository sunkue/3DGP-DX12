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

//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

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
#include "d3dx12.h"
#include "D3DX12Helper.h"
#include "ResourceHelper.h"

using namespace std;
using namespace std::chrono;
using TimePoint = steady_clock::time_point;
constexpr UINT RFR = 144;
constexpr float EPSILON = 1.0e-10f;

inline bool IsZero(float value) { return(fabsf(value) < EPSILON); }
inline bool IsZero(XMVECTOR value) { return IsZero(XMVectorGetX(value)) && IsZero(XMVectorGetY(value)) && IsZero(XMVectorGetZ(value)); }
inline bool IsEqual(float a, float b) { return IsZero(a - b); }
inline float InverseSqrt(float value) { return 1.0f / sqrtf(value); }


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




/*
	목표 일주일 3챕터, 8주

	현재 chapter 6 / 따라하기 14

	05/27
   ~06/21



   셰이딩 관련 알아둘 사항 ->
	A 구조체들 float3A 등등... 은 메모리에 거꾸로 저장됨..
	z->x x->z ㅋㅋㅋ 고생 좀 했음.
*/
