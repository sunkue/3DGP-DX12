#pragma once

#include<concepts>
#include<type_traits>

namespace SUNKUE {
	using namespace std;
	
	template<class T>
	struct TypeInfo {
		using value_type = remove_pointer_t<T>;
	};
	
}