// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

///////
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>

#include <D3Dcompiler.h>

#include <Directxmath.h>
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

using namespace std;
using namespace std::chrono;
using TimePoint = steady_clock::time_point;
constexpr INT FRAME_BUFFER_WIDTH{ 640 };
constexpr INT FRAME_BUFFER_HEIGHT{ 480 };

constexpr UINT RFR = 144;


/*
	목표 일주일 3챕터, 8주

	현재 chapter 4 / 따라하기 03

	04/29
   ~06/21
*/