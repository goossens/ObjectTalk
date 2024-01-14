//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cmath>

#include "nlohmann/json_fwd.hpp"

#include "OtNumbers.h"

#include "OtGeometry.h"


//
//	OtPolyhedronGeometryClass
//

class OtPolyhedronGeometryClass;
using OtPolyhedronGeometry = OtObjectPointer<OtPolyhedronGeometryClass>;

class OtPolyhedronGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	void init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setDetail(int detail);

	// UI to change geometry properties
	bool renderUI() override;

	// (de)serialize geometry
	nlohmann::json serialize(std::filesystem::path* basedir) override;
	void deserialize(nlohmann::json data, std::filesystem::path* basedir) override;

	// get type name of geometry
	const char* getTypeName() override { return name; }

	// geometry name
	static constexpr char const* name = "Polyhedron";

	// get type definition
	static OtType getMeta();

protected:
	glm::vec3* baseVertices;
	uint32_t* baseIndices;
	size_t baseVertexCount;
	size_t baseIndexCount;

private:
	// generate geometry
	void fillGeometry() override;
	void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) override;
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
