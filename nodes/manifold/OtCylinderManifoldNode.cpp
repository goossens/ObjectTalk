//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtCylinderManifoldNode
//

class OtCylinderManifoldNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Height", height);
		addInputPin("Top Radius", topRadius);
		addInputPin("Bottom Radius", bottomRadius);
		addInputPin("Segments", segments);
		addInputPin("Center", center);
		addOutputPin("Manifold", manifold);
	}

	// create the manifold
	inline void onExecute() override {
		height = std::max(height, 0.0f);
		bottomRadius = std::max(bottomRadius, 0.00000001f);
		topRadius = std::max(topRadius, 0.00000001f);
		segments = std::max(segments, 3);
		manifold.cylinder(height, bottomRadius, topRadius, segments, center);
	}

	static constexpr const char* nodeName = "Cylinder Manifold";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::manifold;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	// properties
	float height = 1.0f;
	float bottomRadius = 0.5f;
	float topRadius = 0.5f;
	int segments = 32;
	bool center = false;

	OtManifold manifold;
};

static OtNodesFactoryRegister<OtCylinderManifoldNode> registration;
