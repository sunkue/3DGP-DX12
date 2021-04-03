#pragma once
/*
#include "pch.h"
#include <type_traits>

	폐기 -> 다렉매쓰사용!!
class vec2 {
	union {	
		struct { float x, y; };
		struct { float r, g; };
		struct { float s, t; };
	};
	vec2(const float init = 0) :x{ init }, y{ init }{}
	vec2(const vec2& other) :{ other.data } {}
};

class vec3 {
	union {	 
		struct { float x, y, z; };
		struct { float r, g, b; };
		struct { float s, t, p; };
	};
	vec3(const float init = 0) :x{ init }, y{ init }, z{ init }{}
	vec3(const vec3& other) :data{ other.data } {}
	vec4(const vec4& Vec4) :{ Vec4 } {}
};


class vec4 {
	union {
		struct data{ float x, y, z, w; };
		struct data{ float r, g, b, a; };
		struct data{ float s, t, p, q; };
	};
	vec4(const float init = 0) :x{ init }, y{ init }, z{ init }, w{ init }{}
	vec4(const vec4& other) :data{ other.data } {}
	vec4(vec4&& src) :data{ std::move(src.data } {}
	vec4(const vec3<float>& Vec3) :data{ Vec3,0 } {}
};


class mat3 {
	float data[3][3];

	mat3(const float basic = 1) : data{ basic,0,0,0,basic,0,0,0,basic } {}
	mat3(const mat3& other):data{other.data }{}
	mat3(mat3&& src):mat{std::move(src.data }{}
};


class mat4 {
	float data[4][4];

	mat4(const float basic = 1) : data{ basic,0,0,0,basic,0,0,0,basic } {}
	mat4(const mat4& other) :data{ other.data } {}
	mat34(mat4&& src) :data{ std::move(src.data } {}
};


mat4<float> LookAt(vec3<float> eye,vec3<float> at,vec3<float> up) {
	mat4<float> result;

	return reuslt;
}

mat4<float> Perspective(float fovy, float aspect,float n,float f) {

}
*/

