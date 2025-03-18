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

OtManifold::OtManifold() {
	manifold = std::make_shared<manifold::Manifold>();
	incrementVersion();
}


//
//	OtManifold::OtManifold
//

OtManifold::OtManifold(const manifold::Manifold& m) {
	manifold = std::make_shared<manifold::Manifold>(m);
	incrementVersion();
}


//
//	OtManifold::cube
//

void OtManifold::cube(float width, float height, float depth, bool center) {
	auto m = manifold::Manifold::Cube(manifold::vec3(width, height, depth), center);
	manifold = std::make_shared<manifold::Manifold>(m.Rotate(90.0, 0.0, 0.0));
	incrementVersion();
}


//
//	OtManifold::cylinder
//

void OtManifold::cylinder(float height, float bottomRadius, float topRadius, int segments, bool center) {
	auto m = manifold::Manifold::Cylinder(height, bottomRadius, topRadius, segments, center);
	manifold = std::make_shared<manifold::Manifold>(m.Rotate(90.0, 0.0, 0.0));
	incrementVersion();
}


//
//	OtManifold::sphere
//

void OtManifold::sphere(float radius, int segments) {
	auto m = manifold::Manifold::Sphere(radius, segments);
	manifold = std::make_shared<manifold::Manifold>(m.Rotate(90.0, 0.0, 0.0));
	incrementVersion();
}


//
//	OtManifold::createMesh
//

void OtManifold::createMesh(OtMesh& mesh) {
	auto m = manifold->GetMeshGL();
	auto numberOfVertices = static_cast<size_t>(m.NumVert());
	auto numberOfTriangles = static_cast<size_t>(m.NumTri());

	mesh.clear();

	for (size_t v = 0; v < numberOfVertices; v++) {
		auto vertext = m.GetVertPos(v);
		mesh.addVertex(glm::vec3(vertext.x, vertext.y, vertext.z));
	}

	for (size_t t = 0; t < numberOfTriangles; t++) {
		auto triangles = m.GetTriVerts(t);
		mesh.addTriangle(triangles[0], triangles[1], triangles[2]);
	}

	mesh.generateAABB();
	mesh.generateNormals();
	mesh.generateTangents();
}
