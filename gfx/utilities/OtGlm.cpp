//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/gtc/type_ptr.hpp"

#include "nlohmann/json.hpp"

#include "OtGlm.h"


//
//	glm::to_json
//

void glm::to_json(nlohmann::json& j, const glm::vec3& v)  {
	j = nlohmann::json{v.x, v.y, v.z};
}

void glm::to_json(nlohmann::json& j, const glm::vec4& v)  {
	j = nlohmann::json{v.x, v.y, v.z, v.w};
}

void glm::to_json(nlohmann::json& j, const glm::mat3& m)  {
	auto p = glm::value_ptr(m);

	j = nlohmann::json{
		p[0], p[1], p[2],
		p[3], p[4], p[5],
		p[6], p[7], p[8]
	};
}

void glm::to_json(nlohmann::json& j, const glm::mat4& m)  {
	auto p = glm::value_ptr(m);

	j = nlohmann::json{
		p[0], p[1], p[2], p[3],
		p[4], p[5], p[6], p[7],
		p[8], p[9], p[10], p[11],
		p[12], p[13], p[14], p[15]
	};
}


//
//	glm::from_json
//

void glm::from_json(const nlohmann::json& j, glm::vec3& v) {
	v.x = j[0];
	v.y = j[1];
	v.z = j[2];
}

void glm::from_json(const nlohmann::json& j, glm::vec4& v) {
	v.x = j[0];
	v.y = j[1];
	v.z = j[2];
	v.w = j[3];
}

void glm::from_json(const nlohmann::json& j, glm::mat3& m) {
	auto p = glm::value_ptr(m);

	for (auto i = 0; i < 9; i++) {
		p[i] = j[i];
	}
}

void glm::from_json(const nlohmann::json& j, glm::mat4& m) {
	auto p = glm::value_ptr(m);

	for (auto i = 0; i < 16; i++) {
		p[i] = j[i];
	}
}
