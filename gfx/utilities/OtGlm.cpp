//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtGlm.h"


//
//	glm::to_json
//

void glm::to_json(nlohmann::json &j, const glm::vec3 &v)  {
	j = nlohmann::json{ v.x, v.y, v.z };
}

void glm::to_json(nlohmann::json &j, const glm::vec4 &v)  {
	j = nlohmann::json{ v.x, v.y, v.z, v.w };
}


//
//	glm::from_json
//

void glm::from_json(const nlohmann::json &j, glm::vec3 &v) {
	v.x = j[0];
	v.y = j[1];
	v.z = j[2];
}

void glm::from_json(const nlohmann::json &j, glm::vec4 &v) {
	v.x = j[0];
	v.y = j[1];
	v.z = j[2];
	v.w = j[3];
}
