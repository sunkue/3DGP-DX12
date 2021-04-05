// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <algorithm>
#include <vector>
#include <list>
#include <string>
#include <array>
#include <cmath>
#include <chrono>
#include <memory>
#include <filesystem>

using	std::unique_ptr;
using	std::shared_ptr;
using	std::make_unique;
using	std::make_shared;
using	std::vector;
using	std::string;
using	std::list;
using	std::array;
using	std::sin;
using	std::cos;
using	std::tan;
using	clk = std::chrono::high_resolution_clock;
using	tp = std::chrono::steady_clock::time_point;
using	index = unsigned int;

/* DX */
/*
		idea :
			XMVECTOR 타입을 통하여 SIMD 명령을 사용해야한다.
			데이터는 XMVECTOR XMFLOAT(멤버변수) XMMATRIX(XMVECTOR[4]) 등을 사용.
			sse 레지스터의 활용을 위해, 함수 안에서 계산에 사용할 파라미터 XMVECTOR의 타입을 지정하자. [ 123F 4G 56H etcC ] 함수이름앞에 XM_CALLCONV 지시자 붙이기
				생성자는 [ 123F etcC ] 지시자 X
			위치 w = 1 , 방향 w = 0
		logic :
			충돌검사 => 모델좌표계, 월드좌표계, 카메라 좌표계 어디서든 가능. 역변환을 활용 할 수도 있곗다. (ex: 총알과 사람 => 사람의 월드역변환을 총알에 하고 모델조좌표계에서 검사)
		tip :
			XMGet Access 함수는 느림! 통쨰로 스토어하고 읽는게 더 빠를 수 잇음!
			Aligned 버전을 쓰자!

		edited :	4 / 04
*/

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

XMGLOBALCONST XMFLOAT4X4A BASICMAT{	1.f,0.f,0.f,0.f,
									0.f,1.f,0.f,0.f,
									0.f,0.f,1.f,0.f,
									0.f,0.f,0.f,1.f };

XMGLOBALCONST XMVECTORF32 DIRECTIONZERO{ 0.f,0.f,0.f,0.f };
XMGLOBALCONST XMVECTORF32 VERTEXZERO{ 0.f,0.f,0.f,1.f };

inline bool XM_CALLCONV operator==(FXMVECTOR V1, FXMVECTOR V2) { return DirectX::XMVector3Equal(V1, V2); }
inline bool XM_CALLCONV operator!=(FXMVECTOR V1, FXMVECTOR V2) { return !(V1 == V2); }




/*
	[ 독파 진행상황 ]
	2	/	23 챕터
	1주일에 3개씩 -> 8주 정복. 두달.
*/	