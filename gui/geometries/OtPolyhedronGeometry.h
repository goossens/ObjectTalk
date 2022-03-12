//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cmath>

#include "OtNumbers.h"

#include "OtGeometry.h"


//
//	OtPolyhedronGeometryClass
//

class OtPolyhedronGeometryClass;
typedef std::shared_ptr<OtPolyhedronGeometryClass> OtPolyhedronGeometry;

class OtPolyhedronGeometryClass : public OtGeometryClass {
public:
	// initialize geometry
	OtObject init(size_t count, OtObject* parameters);

	// update attributes
	OtObject setRadius(float radius);
	OtObject setDetail(int detail);

	// get type definition
	static OtType getMeta();

protected:
	glm::vec3* baseVertices;
	uint32_t* baseIndices;
	size_t baseVertexCount;
	size_t baseIndexCount;

private:
	// generate geometry
	void fillGeometry();
	void addFace(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
	glm::vec2 generateUV(const glm::vec3& normal);

	float azimuth(const glm::vec3& normal) {
		return std::atan2(normal.x, normal.z);
	}

	float inclination(const glm::vec3& normal) {
		return std::atan2(-normal.y, std::sqrt((normal.x * normal.x) + (normal.z * normal.z)));
	}

	// attributes
	float radius = 1.0;
	int detail = 0;
};
