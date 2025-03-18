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
//	In ObjectTalk, Manifolds are used in Nodes to interactively create models that can be rendered.
//


//
//
//	OtManifold
//

class OtManifold {
public:
	// constructors
	OtManifold();
	OtManifold(const manifold::Manifold& m);

	// create primitives
	void cube(float width, float height, float depth, bool center);
	void cylinder(float height, float bottomRadius, float topRadius, int segments, bool center);
	void sphere(float radius, int segments);

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
