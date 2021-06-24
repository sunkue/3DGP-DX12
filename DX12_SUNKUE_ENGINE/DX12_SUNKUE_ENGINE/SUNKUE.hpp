#pragma once

#include<concepts>
#include<type_traits>
#include<string>

#ifdef UNICODE
using tstring = std::wstring;
#else
using tstring = std::string;
#endif // UNICODE

#define ALLOF(cont)(begin(cont)),(end(cont))

namespace SUNKUE {
	using namespace std;

	template<class T>
	struct TypeInfo
	{
		using value_type = decltype(_GetValueType<T>());
	};

	template<class T>
	auto consteval _GetValueType()
	{
		if constexpr (is_pointer<T>()) return _GetValueType<remove_pointer_t<T>>();
		else return T();
	}

	



}