//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


/*
#pragma warning(disable:4244)
Z:\ObjectTalk\build\vs\deps\manifold\manifold-src\include\manifold/common.h(547,15): warning C4244: 'initializing': conversion from 'double' t
o 'int', possible loss of data [Z:\ObjectTalk\build\vs\gfx\gfx.vcxproj]
Z:\ObjectTalk\build\vs\deps\manifold\manifold-src\include\manifold/common.h(548,15): warning C4244: 'initializing': conversion from 'double' t
o 'int', possible loss of data [Z:\ObjectTalk\build\vs\gfx\gfx.vcxproj]
Z:\ObjectTalk\build\vs\deps\manifold\manifold-src\include\manifold/common.h(549,14): warning C4244: 'initializing': conversion from 'double' t
o 'int', possible loss of data [Z:\ObjectTalk\build\vs\gfx\gfx.vcxproj]
*/


//
//	Include files
//

#include <memory>

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
	OtManifold() {}
	OtManifold(const manifold::Manifold& m);

	// clear the manifold
	void clear();

	// see if anifold is valid
	bool isValid() { return manifold != nullptr && !manifold->IsEmpty(); }

	// create primitives
	void cube(float width, float height, float depth, bool center);
	void cylinder(float height, float bottomRadius, float topRadius, int segments, bool center);
	void sphere(float radius, int segments);

	// combine manifolds
	void unionManifolds(const OtManifold& a, const OtManifold& b);
	void differenceManifolds(const OtManifold& a, const OtManifold& b);
	void intersectManifolds(const OtManifold& a, const OtManifold& b);

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
