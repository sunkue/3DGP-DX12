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
	struct TypeInfo {
		using value_type = remove_pointer_t<T>;
	};

}