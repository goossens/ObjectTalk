//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"
#include "nlohmann/json_fwd.hpp"

#include "OtColorParser.h"


//
//	OtColor
//

class OtColor {
public:
	// constructors
	OtColor() = default;
	inline OtColor(float v) : color(v, v, v, 1.0f) {}
	inline OtColor(float r, float g, float b, float a=1.0f) : color(r, g, b, a) {}
	inline OtColor(glm::vec3 c) : color(c, 1.0f) {}
	inline OtColor(glm::vec4 c) : color(c) {}
	inline OtColor(const std::string& c) { color = OtColorParser::toVec4(c); }

	// access color data
	inline float getRed() const { return color.r; }
	inline float getGreen() const { return color.g; }
	inline float getBlue() const { return color.b; }
	inline float getAlpha() const { return color.a; }
	inline float* data() { return &color.r; }

	inline void set(float r, float g, float b, float a=1.0f) { color = glm::vec4(r, g, b, a); }
	inline void setRed(float r) { color.r = r; }
	inline void setGreen(float g) { color.g = g; }
	inline void setBlue(float b) { color.b = b; }
	inline void setAlpha(float a) { color.a = a; }

	// conversion operators
	inline operator glm::vec3() { return (glm::vec3) color; }
	inline operator glm::vec4() { return color; }

	// see if colors are identical
	inline bool operator==(OtColor& rhs) {
		return color == rhs.color;
	}

	inline bool operator!=(OtColor& rhs) {
		return !operator==(rhs);
	}

private:
	glm::vec4 color{0.0f};
};


//
//	(De)serialization to/from JSON
//

void to_json(nlohmann::json& j, const OtColor& c);
void from_json(const nlohmann::json& j, OtColor& c);
