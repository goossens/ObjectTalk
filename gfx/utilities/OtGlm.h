//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/quaternion.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtNumbers.h"


//
//	Calculation functions
//

inline glm::vec3 OtGlmMul(const glm::mat4& m, const glm::vec3& v) {
	auto r = m * glm::vec4(v, 1.0f);
	return glm::vec3(r.x / r.w, r.y / r.w, r.z / r.w);
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


//
//	(De)serialization to/from JSON
//

namespace glm {
	void to_json(nlohmann::json& j, const glm::vec2& v);
	void to_json(nlohmann::json& j, const glm::vec3& v);
	void to_json(nlohmann::json& j, const glm::vec4& v);
	void to_json(nlohmann::json& j, const glm::mat3& m);
	void to_json(nlohmann::json& j, const glm::mat4& m);

	void from_json(const nlohmann::json& j, glm::vec2& v);
	void from_json(const nlohmann::json& j, glm::vec3& v);
	void from_json(const nlohmann::json& j, glm::vec4& v);
	void from_json(const nlohmann::json& j, glm::mat3& m);
	void from_json(const nlohmann::json& j, glm::mat4& m);
}


//
//	Debug functions
//

inline void OtGlmDebug(const char* label, const glm::vec2& v) {
	std::cout << label << glm::to_string(v) << std::endl;
}

inline void OtGlmDebug(const char* label, const glm::vec3& v) {
	std::cout << label << glm::to_string(v) << std::endl;
}

inline void OtGlmDebug(const char* label, const glm::vec4& v) {
	std::cout << label << glm::to_string(v) << std::endl;
}


inline void OtGlmDebug(const char* label, const glm::quat& q) {
	std::cout << label << glm::to_string(q) << std::endl;
}


inline void OtGlmDebug(const char* label, const glm::mat3& m) {
	std::cout << label << glm::to_string(m) << std::endl;
}

inline void OtGlmDebug(const char* label, const glm::mat4& m) {
	std::cout << label << glm::to_string(m) << std::endl;
}
