#pragma once

#include <DirectxMath.h>

namespace SUNKUE {
	using namespace DirectX;

	inline XMVECTOR xAxis() { return XMVECTOR{ 1.0f,0.0f,0.0f,0.0f }; }
	inline XMVECTOR yAxis() { return XMVECTOR{ 0.0f,1.0f,0.0f,0.0f }; }
	inline XMVECTOR zAxis() { return XMVECTOR{ 0.0f,0.0f,1.0f,0.0f }; }

	template<typename XMFLOAT>
	inline XMVECTOR GetVector(const XMFLOAT& src)
	{
		if constexpr (is_same<XMFLOAT, XMFLOAT2>::value)return XMLoadFloat2(&src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT2A>::value)return XMLoadFloat2A(&src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT3>::value)return XMLoadFloat3(&src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT3A>::value)return XMLoadFloat3A(&src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT4>::value)return XMLoadFloat4(&src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT4A>::value)return XMLoadFloat4A(&src);
		else static_assert(false, "vec::Is not XMFLOAT");
	}

	template<typename XMFLOAT>
	inline void SetVector(XMFLOAT& to, XMVECTOR src)
	{
		if constexpr (is_same<XMFLOAT, XMFLOAT2>::value)return XMStoreFloat2(&to, src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT2A>::value)return XMStoreFloat2A(&to, src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT3>::value)return XMStoreFloat3(&to, src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT3A>::value)return XMStoreFloat3A(&to, src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT4>::value)return XMStoreFloat4(&to, src);
		else if constexpr (is_same<XMFLOAT, XMFLOAT4A>::value)return XMStoreFloat4A(&to, src);
		else static_assert(false, "vec::Is not XMFLOAT");
	}

	template<typename XMFLOATX>
	inline XMMATRIX GetMatrix(const XMFLOATX& src)
	{
		if constexpr (is_same<XMFLOATX, XMFLOAT4X4>::value)return XMLoadFloat4x4(&src);
		else if constexpr (is_same<XMFLOATX, XMFLOAT4X4A>::value)return XMLoadFloat4x4A(&src);
		else static_assert(false, "vec::Is not XMFLOATX");
	}

	template<typename XMFLOATX>
	inline void SetMatrix(XMFLOATX& to, XMMATRIX src)
	{
		if constexpr (is_same<XMFLOATX, XMFLOAT4X4>::value)return XMStoreFloat4x4(&to, src);
		else if constexpr (is_same<XMFLOATX, XMFLOAT4X4A>::value)return XMStoreFloat4x4A(&to, src);
		else static_assert(false, "vec::Is not XMFLOATX");
	}
}