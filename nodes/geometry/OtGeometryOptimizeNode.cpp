//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtGeometry.h"

#include "OtNodesFactory.h"


//
//	OtGeometryOptimizeNode
//

class OtGeometryOptimizeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", input);
		addOutputPin("Output", output);
	}

	// apply transformation to geometry
	inline void onExecute() override {
		// do we have a valid input
		if (input.isValid()) {
			output.cloneFrom(input);
			output.optimize();

		} else {
			output.clear();
		}
	}

	static constexpr const char* nodeName = "Geometry Optimize";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::geometry;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtGeometry input;
	OtGeometry output;
};

static OtNodesFactoryRegister<OtGeometryOptimizeNode> registration;
