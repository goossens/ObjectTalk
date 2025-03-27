//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtManifold.h"
#include "OtShape.h"

#include "OtNodesFactory.h"


//
//	OtManifoldRevolveNode
//

class OtManifoldRevolveNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Shape", shape);
		addInputPin("Segments", segments);
		addInputPin("revolveDegrees", revolveDegrees);
		addInputPin("Tolerance", tolerance);
		addOutputPin("Manifold", manifold);
	}

	// create the manifold
	inline void onExecute() override {
		if (shape.isValid()) {
			manifold.revolve(shape, segments, revolveDegrees, tolerance);

		} else {
			manifold.clear();
		}
	}

	static constexpr const char* nodeName = "Manifold Revolve";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::manifold;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtShape shape;
	OtManifold manifold;
	int segments = 128;
	float revolveDegrees = 360.0f;
	float tolerance = 0.25f;
};

static OtNodesFactoryRegister<OtManifoldRevolveNode> registration;
