//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtx/io.hpp"

#include "OtNumbers.h"


//
//	Functions
//

inline void OtGlmDebug(const glm::vec2& v) {
	std::cout << v << std::endl;
}

inline void OtGlmDebug(const glm::vec3& v) {
	std::cout << v << std::endl;
}

inline void OtGlmDebug(const glm::vec4& v) {
	std::cout << v << std::endl;
}


inline void OtGlmDebug(const glm::mat3& m) {
	std::cout << m << std::endl;
}

inline void OtGlmDebug(const glm::mat4& m) {
	std::cout << m << std::endl;
}

inline glm::vec4 OtGlmHomogonize(const glm::vec4& v) {
	return glm::vec4(v.x / v.w, v.y / v.w, v.z / v.w, 1.0f);
}

inline glm::vec2 OtGlmNormalToLatLon(const glm::vec3& normal) {
	return glm::vec2(
		std::asin(normal.y),
		std::atan2(normal.x, normal.z));
}

inline glm::vec2 OtGlmNormalToUV(const glm::vec3& normal) {
	glm::vec2 ll = OtGlmNormalToLatLon(normal);

	return glm::vec2(
		(ll.y + std::numbers::pi) / (2.0 * std::numbers::pi),
		(ll.x - std::numbers::pi / 2.0) / -std::numbers::pi);
}

inline glm::vec3 OtGlmLatLonToNormal(const glm::vec2& ll) {
	float r = std::cos(ll.x);

	return glm::vec3(
		std::sin(ll.y) * r,
		std::sin(ll.x),
		std::cos(ll.y) * r);
}

inline glm::vec3 OtGlmUvToNormal(const glm::vec2& uv) {
	return OtGlmLatLonToNormal(glm::vec2(
		(uv.y * -std::numbers::pi) + std::numbers::pi,
		uv.x * std::numbers::pi * 2.0 - std::numbers::pi));
}