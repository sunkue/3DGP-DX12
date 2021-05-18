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

#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

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
#include "d3dx12.h"
#include "D3DX12Helper.h"
#include "ResourceHelper.h"

using namespace std;
using namespace std::chrono;
using TimePoint = steady_clock::time_point;

constexpr UINT RFR = 144;

#define WIDTH	640
#define HEIGHT	480

/*
	목표 일주일 3챕터, 8주

	현재 chapter 6 / 따라하기 07

	05/17
   ~06/21
*/
