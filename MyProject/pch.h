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
			sse 레지스터의 활용을 위해, 함수 안에서 계산에 사용할 파라미터 XMVECTOR의 타입을 지정하자. [ 123F 4G 56H etcC ] 함수이름앞에 XM_CALLCONV 지시자 붙이기
				생성자는 [ 123F etcC ] 지시자 X

		타입이름 규칙 :
			XMVECTOR 는 메모리의 연속성(alignment)이 보장되어야 한다. 지역, 전역변수로 사용시 보장됨 자주 쓸 것이므로 읽기쓰기 편한 vec으로 했음.
			XMFLOAT 류는 메모리의 연속성이 보장되지 않는 클래수 멤버에서 쓸 것이므로 타입이름에 m_을 붙여구분하였음.

		함수이름 규칙 :
			시작 글자가 V => XMVECTOR 가 주체.
			시작 글자가 M => XMVFLOAT 가 주체.

		edited :	4 / 03
*/

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
using	m_vec2 = DirectX::XMFLOAT2;	// 이름 후보 : m_vec2 MVEC2 MV2 vec2 mvec2 Mvec2 XMF2
using	m_vec3 = DirectX::XMFLOAT3;
using	m_vec4 = DirectX::XMFLOAT4;
using	vec = DirectX::XMVECTOR;	// 이름 후보 : vec VEC V XMV
using	Fvec = DirectX::FXMVECTOR;
using	Gvec = DirectX::GXMVECTOR;
using	Hvec = DirectX::HXMVECTOR;
using	Cvec = DirectX::CXMVECTOR;
using	CONSTvec = DirectX::XMVECTORF32;



inline auto	XM_CALLCONV M2V2(const m_vec2* pSource) { return DirectX::XMLoadFloat2(pSource); }
inline auto	XM_CALLCONV M2V3(const m_vec3* pSource) { return DirectX::XMLoadFloat3(pSource); }
inline auto	XM_CALLCONV M2V4(const m_vec4* pSource) { return DirectX::XMLoadFloat4(pSource); }
inline auto	XM_CALLCONV V2M2(m_vec2* pDestination, Fvec V) { return DirectX::XMStoreFloat2(pDestination, V); }
inline auto	XM_CALLCONV V2M3(m_vec3* pDestination, Fvec V) { return DirectX::XMStoreFloat3(pDestination, V); }
inline auto	XM_CALLCONV V2M4(m_vec4* pDestination, Fvec V) { return DirectX::XMStoreFloat4(pDestination, V); }
inline auto XM_CALLCONV VGetX(Fvec V) { return DirectX::XMVectorGetX(V); }
inline auto XM_CALLCONV VGetY(Fvec V) { return DirectX::XMVectorGetY(V); }
inline auto XM_CALLCONV VGetZ(Fvec V) { return DirectX::XMVectorGetZ(V); }
inline auto XM_CALLCONV VGetW(Fvec V) { return DirectX::XMVectorGetW(V); }
inline auto XM_CALLCONV VSetX(Fvec V, float x) { return DirectX::XMVectorSetX(V, x); }
inline auto XM_CALLCONV VSetY(Fvec V, float y) { return DirectX::XMVectorSetY(V, y); }
inline auto XM_CALLCONV VSetZ(Fvec V, float z) { return DirectX::XMVectorSetZ(V, z); }
inline auto XM_CALLCONV VSetW(Fvec V, float w) { return DirectX::XMVectorSetW(V, w); }
inline auto Radians(float fDegrees) { return DirectX::XMConvertToRadians(fDegrees); }
inline auto Degrees(float fRadians) { return DirectX::XMConvertToDegrees(fRadians); }
inline auto XM_CALLCONV MakeVecZero() { return DirectX::XMVectorZero(); }
inline auto XM_CALLCONV MakeVecOne() { return DirectX::XMVectorSplatOne(); }
inline auto XM_CALLCONV MakeVec(float x, float y, float z, float w) { return DirectX::XMVectorSet(x, y, z, w); }
inline auto XM_CALLCONV MakeVecWith(float value) { return DirectX::XMVectorReplicate(value); }
inline auto XM_CALLCONV MakeVecWithX(Fvec V) { return DirectX::XMVectorSplatX(V); }
inline auto XM_CALLCONV MakeVecWithY(Fvec V) { return DirectX::XMVectorSplatY(V); }
inline auto XM_CALLCONV MakeVecWithZ(Fvec V) { return DirectX::XMVectorSplatZ(V); }
inline auto XM_CALLCONV MakeVecWithW(Fvec V) { return DirectX::XMVectorSplatW(V); }
inline auto XM_CALLCONV Length(Fvec V) { return DirectX::XMVector3Length(V); }
inline auto XM_CALLCONV LengthSq(Fvec V) { return DirectX::XMVector3LengthSq(V); }
inline auto XM_CALLCONV Dot(Fvec V1, Fvec V2) { return DirectX::XMVector3Dot(V1, V2); }
inline auto XM_CALLCONV Cross(Fvec V1, Fvec V2) { return DirectX::XMVector3Cross(V1, V2); }
inline auto XM_CALLCONV Normalize(Fvec V) { return DirectX::XMVector3Normalize(V); }
inline auto XM_CALLCONV Orthogonal(Fvec V) { return DirectX::XMVector3Orthogonal(V); }
inline auto XM_CALLCONV RadiansBetweenVecs(Fvec V1, Fvec V2) { return DirectX::XMVector3AngleBetweenVectors(V1, V2); }
inline auto XM_CALLCONV ComponentsFromNormal(vec* pParallel, vec* pPerpendcular, Fvec V, Fvec Normal) { return DirectX::XMVector3ComponentsFromNormal(pParallel, pPerpendcular, V, Normal); }
inline bool operator==(Fvec V1, Fvec V2) { return DirectX::XMVector3Equal(V1, V2); }
inline bool operator!=(Fvec V1, Fvec V2) { return DirectX::XMVector3NotEqual(V1, V2); }





