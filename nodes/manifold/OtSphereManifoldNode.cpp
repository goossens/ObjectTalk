//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtManifold.h"

#include "OtNodesFactory.h"


//
//	OtSphereManifoldNode
//

class OtSphereManifoldNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Radius", radius);
		addInputPin("Segments", segments);
		addOutputPin("Manifold", manifold);
	}

	// create the manifold
	inline void onExecute() override {
		radius = std::max(radius, 0.00000001f);
		segments = std::max(segments, 3);
		manifold = OtManifold::sphere(radius, segments);
	}

	static constexpr const char* nodeName = "Sphere Manifold";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::manifold;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	float radius = 1.0f;
	int segments = 32;

	OtManifold manifold;
};

static OtNodesFactoryRegister<OtSphereManifoldNode> registration;
