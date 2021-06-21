#pragma once

#include <DirectxMath.h>
#include <concepts>
#include <cmath>
#include <numeric>

namespace SUNKUE {
	using namespace std;
	using namespace DirectX;

	static constexpr XMVECTORF32 xAxis{ 1.0f,0.0f,0.0f,0.0f };
	static constexpr XMVECTORF32 yAxis{ 0.0f,1.0f,0.0f,0.0f };
	static constexpr XMVECTORF32 zAxis{ 0.0f,0.0f,1.0f,0.0f };

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


	template<size_t N>requires (1 < N) and (N < 5)
	XMVECTOR XM_CALLCONV CaculateBezierCurves(const vector<array<XMFLOAT3, N>>& BezierCurves, float t)
	{
		assert(false == BezierCurves.empty());
		if (BezierCurves.size() < t)return Load(*(prev(end(*prev(end(BezierCurves), 1)), 1)));
		if (t < 0)return Load(BezierCurves.at(0).at(0));
		int tInt = static_cast<int>(t);
		float tPercent = t - tInt;

		vector<XMFLOAT3> ret{ ALLOF(BezierCurves.at(tInt)) };
		while (1 != ret.size()) {
			vector<XMFLOAT3> temp;
			adjacent_difference(ALLOF(ret), back_inserter(temp),
				[tPercent](XMFLOAT3 a, XMFLOAT3 b)->XMFLOAT3 {
					return { lerp(a.x, b.x, tPercent),lerp(a.y, b.y, tPercent),lerp(a.z, b.z, tPercent) };
				});
			ret = { next(begin(temp),1),end(temp) };
		}
		return Load(ret[0]);
	}
}