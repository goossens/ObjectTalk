//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtManifold.h"


//
//	OtManifold::OtManifold
//

OtManifold::OtManifold(const manifold::Manifold& m) {
	manifold = std::make_shared<manifold::Manifold>(m);
	incrementVersion();
}


//
//	OtManifold::clear
//

void OtManifold::clear() {
	manifold = nullptr;
	incrementVersion();
}


//
//	OtManifold::cube
//

void OtManifold::cube(float width, float height, float depth, bool center) {
	manifold = std::make_shared<manifold::Manifold>(
		manifold::Manifold::Cube(manifold::vec3(width, height, depth), center).CalculateNormals(0));

	incrementVersion();
}


//
//	OtManifold::cylinder
//

void OtManifold::cylinder(float height, float bottomRadius, float topRadius, int segments, bool center) {
	manifold = std::make_shared<manifold::Manifold>(
		manifold::Manifold::Cylinder(height, bottomRadius, topRadius, segments, center).CalculateNormals(0));

	incrementVersion();
}


//
//	OtManifold::sphere
//

void OtManifold::sphere(float radius, int segments) {
	manifold = std::make_shared<manifold::Manifold>(
		manifold::Manifold::Sphere(radius, segments).CalculateNormals(0));

	incrementVersion();
}


//
//	OtManifold::unionManifolds
//

void OtManifold::unionManifolds(const OtManifold& a, const OtManifold& b) {
	manifold = std::make_shared<manifold::Manifold>(*a.manifold + *b.manifold);
	incrementVersion();
}


//
//	OtManifold::differenceManifolds
//

void OtManifold::differenceManifolds(const OtManifold& a, const OtManifold& b) {
	manifold = std::make_shared<manifold::Manifold>(*a.manifold - *b.manifold);
	incrementVersion();
}


//
//	OtManifold::intersectManifolds
//

void OtManifold::intersectManifolds(const OtManifold& a, const OtManifold& b) {
	manifold = std::make_shared<manifold::Manifold>(*a.manifold ^ *b.manifold);
	incrementVersion();
}


//
//	OtManifold::createMesh
//

void OtManifold::createMesh(OtMesh& mesh) {
	// get manifold information
	auto m = manifold->GetMeshGL();
	auto numberOfVertices = static_cast<size_t>(m.NumVert());
	auto numberOfTriangles = static_cast<size_t>(m.NumTri());
	auto numberOfProperties = static_cast<size_t>(m.numProp);

	// clear mesh and start converting
	mesh.clear();
	size_t offset = 0;

	// see if manifold includes normals
	if (numberOfProperties >= 6) {
		for (size_t v = 0; v < numberOfVertices; v++) {
			mesh.addVertex(OtVertex(
				glm::vec3(m.vertProperties[offset], m.vertProperties[offset + 2], -m.vertProperties[offset + 1]),
				glm::vec3(m.vertProperties[offset + 3], m.vertProperties[offset + 5], -m.vertProperties[offset + 4])));

			offset += numberOfProperties;
		}

	} else {
		for (size_t v = 0; v < numberOfVertices; v++) {
			mesh.addVertex(glm::vec3(m.vertProperties[offset], m.vertProperties[offset + 2], -m.vertProperties[offset + 1]));
			offset += numberOfProperties;
		}
	}

	// generate triangles
	for (size_t t = 0; t < numberOfTriangles; t++) {
		auto triangles = m.GetTriVerts(t);
		mesh.addTriangle(triangles[0], triangles[1], triangles[2]);
	}

	mesh.generateAABB();

	// if normals were not included
	if (numberOfProperties < 6) {
		mesh.generateNormals();
	}
}
