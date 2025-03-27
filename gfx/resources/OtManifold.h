//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>

#include "glm/glm.hpp"

#if defined __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wshorten-64-to-32"

#elif defined _MSC_VER
#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#endif

#include "manifold/manifold.h"

#if defined __clang__
#pragma clang diagnostic pop

#elif defined _MSC_VER
#pragma warning(pop)
#endif

#include "OtMesh.h"
#include "OtShape.h"


//
//	Manifold versus Geometry
//
//	In ObjectTalk, Manifolds are specializations of Geometries. A manifold mesh is watertight and
//	contains no holes or missing faces that would cause leaks into the interior of the shape's volume.
//	For a mesh to be manifold, every edge must have exactly two adjacent faces. Geometries do not have
//	this constraint.
//
//	Manifolds are often used in CAD applications to create 3D models through Constructive Solid Geometry.
//	You can combined or subtract solid shapes, transform them, extrude them from 2d paths or slice 3D
//	models back into 2D paths.
//
//	Another difference between Manifolds and Geometries is their coordinate system. In ObjectTalk,
//	a classic right-handed coordinate system is used where the positive X-axis points to the right,
//	the positive Y-axis points up, and the positive Z-axis points out of the screen towards the viewer.
//	This is called a right-handed Y-up coordinate system. CAD programs and therefore Manifolds use a
//	left-handed Z-up cooridinate system where the positive X-axis points to the right, the positive
//	Y-axis points away, and the positive Z-axis points up.
//
//	So when creating a Manifold, it must be done in the left-handed Z-up cooridinate system. When converting
//	to a Mesh or Geometry, this class automativally converts the vertex coordinates to the right-handed
//	Y-up coordinate system.
//
//	In ObjectTalk, Manifolds are used in Nodes to interactively create models that can be rendered.
//


//
//
//	OtManifold
//

class OtManifold {
public:
	// constructors
	OtManifold() = default;
	OtManifold(const manifold::Manifold& m);

	// clear the manifold
	void clear();

	// see if anifold is valid
	bool isValid() { return manifold != nullptr && !manifold->IsEmpty(); }

	// manifold I/O
	void load(const std::string& filepath);
	void save(const std::string& filepath);

	// create primitives
	void cube(float width, float height, float depth, bool center);
	void cylinder(float height, float bottomRadius, float topRadius, int segments, bool center);
	void sphere(float radius, int segments);

	// combine manifolds
	OtManifold unionManifolds(OtManifold& other);
	OtManifold differenceManifolds(OtManifold& other);
	OtManifold intersectManifolds(OtManifold& other);

	// 2D to 3D and visa versa
	void extrude(OtShape& shape, float height, int segments, float twistDegrees, float scaleTop, float tolerance);
	void revolve(OtShape& shape, int segments, float revolveDegrees, float tolerance);

	// transform manifold
	OtManifold translate(float x, float y, float z);
	OtManifold rotate(float x, float y, float z);
	OtManifold scale(float x, float y, float z);
	OtManifold mirror(float x, float y, float z);

	inline OtManifold translate(glm::vec3 xyz) { return translate(xyz.x, xyz.y, xyz.z); }
	inline OtManifold rotate(glm::vec3 xyz) { return rotate(xyz.x, xyz.y, xyz.z); }
	inline OtManifold scale(glm::vec3 xyz) { return scale(xyz.x, xyz.y, xyz.z); }
	inline OtManifold mirror(glm::vec3 xyz) { return mirror(xyz.x, xyz.y, xyz.z); }

	// get manifold information
	inline int getVertexCount() { return manifold ? static_cast<int>(manifold->NumVert()) : 0; }
	inline int getTriangleCount() { return manifold ? static_cast<int>(manifold->NumTri()) : 0; }

	// create a mesh
	void createMesh(OtMesh& mesh);

	// version management
	inline void setVersion(int v) { version = v; }
	inline int getVersion() { return version; }
	inline void incrementVersion() { version++; }

	// see if manifold are identical
	inline bool operator==(OtManifold& rhs) {
		return manifold == rhs.manifold && version == rhs.version;
	}

	inline bool operator!=(OtManifold& rhs) {
		return !operator==(rhs);
	}

private:
	// properties
	std::shared_ptr<manifold::Manifold> manifold;
	int version = 0;
};
