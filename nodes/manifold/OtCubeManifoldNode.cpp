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
//	OtCubeManifoldNode
//

class OtCubeManifoldNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Width", width);
		addInputPin("Height", height);
		addInputPin("Depth", depth);
		addInputPin("Center", center);
		addOutputPin("Manifold", manifold);
	}

	// create the manifold
	inline void onExecute() override {
		width = std::max(width, 0.0f);
		height = std::max(height, 0.0f);
		depth = std::max(depth, 0.0f);
		manifold = OtManifold::cube(width, height, depth, center);
	}

	static constexpr const char* nodeName = "Cube Manifold";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::manifold;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	float width = 1.0f;
	float height = 1.0f;
	float depth = 1.0f;
	bool center = false;

	OtManifold manifold;
};

static OtNodesFactoryRegister<OtCubeManifoldNode> registration;
