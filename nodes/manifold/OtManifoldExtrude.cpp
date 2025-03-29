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
//	OtManifoldExtrudeNode
//

class OtManifoldExtrudeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Shape", shape);
		addInputPin("Height", height);
		addInputPin("Divisions", divisions);
		addInputPin("Twist Degrees", twistDegrees);
		addInputPin("Scale Top", scaleTop);
		addInputPin("Tolerance", tolerance);
		addOutputPin("Manifold", manifold);
	}

	// create the manifold
	inline void onExecute() override {
		if (shape.isValid()) {
			manifold = OtManifold::extrude(shape, height, divisions, twistDegrees, scaleTop, tolerance);

		} else {
			manifold.clear();
		}
	}

	static constexpr const char* nodeName = "Manifold Extrude";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::manifold;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtShape shape;
	OtManifold manifold;
	float height = 1.0f;
	int divisions = 0;
	float twistDegrees = 0.0f;
	float scaleTop = 1.0f;
	float tolerance = 0.25f;
};

static OtNodesFactoryRegister<OtManifoldExtrudeNode> registration;
