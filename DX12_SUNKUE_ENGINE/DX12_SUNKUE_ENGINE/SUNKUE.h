#pragma once

#include<concepts>

namespace SUNKUE {
	template<class T>concept arithmetic = std::integral<T> or std::floating_point<T>;
}