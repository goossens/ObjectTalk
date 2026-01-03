//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <cmath>

#include "nlohmann/json_fwd.hpp"

#include "OtPrimitiveBase.h"


//
//	OtPolyhedronPrimitive
//

class OtPolyhedronPrimitive : public OtPrimitiveBase {
public:
	// UI to change primitive's properties
	bool renderUI() override;

	// (de)serialize primitive
	nlohmann::json serialize(std::string* basedir) override;
	void deserialize(nlohmann::json data, std::string* basedir) override;

	// get type name of primitive
	const char* getTypeName() override { return name; }

	// primitive name
	static constexpr char const* name = "Polyhedron";

protected:
	glm::vec3* baseVertices;
	uint32_t* baseIndices;
	size_t baseVertexCount;
	size_t baseIndexCount;

private:
	// generate mesh
	void createMesh(OtMesh* mesh) override;
	void addFace(OtMesh* mesh, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
	glm::vec2 generateUV(const glm::vec3& normal);

	float azimuth(const glm::vec3& normal) {
		return std::atan2(normal.x, normal.z);
	}

	float inclination(const glm::vec3& normal) {
		return std::atan2(-normal.y, std::sqrt((normal.x * normal.x) + (normal.z * normal.z)));
	}

	// attributes
	int detail = 0;
};
