#pragma once

#include <DirectxMath.h>
#include <concepts>

namespace SUNKUE {
	using namespace std;
	using namespace DirectX;

	inline XMVECTOR xAxis() { return XMVECTOR{ 1.0f,0.0f,0.0f,0.0f }; }
	inline XMVECTOR yAxis() { return XMVECTOR{ 0.0f,1.0f,0.0f,0.0f }; }
	inline XMVECTOR zAxis() { return XMVECTOR{ 0.0f,0.0f,1.0f,0.0f }; }

	template<class T>concept DXFLOATV = is_same<T, XMFLOAT2>::value or is_same<T, XMFLOAT3>::value or is_same<T, XMFLOAT4>::value;
	template<class T>concept DXFLOATA = is_same<T, XMFLOAT2A>::value or is_same<T, XMFLOAT3A>::value or is_same<T, XMFLOAT4A>::value;
	template<class T>concept DXFLOAT = DXFLOATV<T> or DXFLOATA<T>;

	template<class T>concept DXFLOATXV = is_same<T, XMFLOAT3X3>::value or is_same<T, XMFLOAT3X4>::value or is_same<T, XMFLOAT4X3>::value or is_same<T, XMFLOAT4X4>::value;
	template<class T>concept DXFLOATXA = is_same<T, XMFLOAT3X4A>::value or is_same<T, XMFLOAT4X3A>::value or is_same<T, XMFLOAT4X4A>::value;
	template<class T>concept DXFLOATX = DXFLOATXV<T> or DXFLOATXA<T>;

	template<class T>concept DXFLOATSTRUCT = DXFLOAT<T> or DXFLOATX<T>;

	template<class T>requires DXFLOATSTRUCT<T>
	auto Load(const T& src)
	{
		if constexpr (is_same<T, XMFLOAT2>::value)return XMLoadFloat2(&src);
		else if constexpr (is_same<T, XMFLOAT2A>::value)return XMLoadFloat2A(&src);
		else if constexpr (is_same<T, XMFLOAT3>::value)return XMLoadFloat3(&src);
		else if constexpr (is_same<T, XMFLOAT3A>::value)return XMLoadFloat3A(&src);
		else if constexpr (is_same<T, XMFLOAT4>::value)return XMLoadFloat4(&src);
		else if constexpr (is_same<T, XMFLOAT4A>::value)return XMLoadFloat4A(&src);
		else if constexpr (is_same<T, XMFLOAT4X4>::value)return XMLoadFloat4x4(&src);
		else if constexpr (is_same<T, XMFLOAT4X4A>::value)return XMLoadFloat4x4A(&src);
		else static_assert(false, "Add yourthing");
	}

	template<class T>requires DXFLOATSTRUCT<T>
	inline void Store(T& to, auto src)
	{
		if constexpr (is_same<T, XMFLOAT2>::value)return XMStoreFloat2(&to, src);
		else if constexpr (is_same<T, XMFLOAT2A>::value)return XMStoreFloat2A(&to, src);
		else if constexpr (is_same<T, XMFLOAT3>::value)return XMStoreFloat3(&to, src);
		else if constexpr (is_same<T, XMFLOAT3A>::value)return XMStoreFloat3A(&to, src);
		else if constexpr (is_same<T, XMFLOAT4>::value)return XMStoreFloat4(&to, src);
		else if constexpr (is_same<T, XMFLOAT4A>::value)return XMStoreFloat4A(&to, src);
		else if constexpr (is_same<T, XMFLOAT4X4>::value)return XMStoreFloat4x4(&to, src);
		else if constexpr (is_same<T, XMFLOAT4X4A>::value)return XMStoreFloat4x4A(&to, src);
		else static_assert(false, "Add yourthing");
	}

}