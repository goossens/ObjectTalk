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
//	OtGeometryMergeNode
//

class OtGeometryMergeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input 1", input1);
		addInputPin("Input 2", input2);
		addInputPin("Input 3", input3);
		addInputPin("Input 4", input4);
		addOutputPin("Output", output);
	}

	// apply transformation to geometry
	inline void onExecute() override {
		output.clear();
		if (input1.isValid()) { output.add(input1); }
		if (input2.isValid()) { output.add(input2); }
		if (input3.isValid()) { output.add(input3); }
		if (input4.isValid()) { output.add(input4); }
	}

	static constexpr const char* nodeName = "Geometry Merge";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::geometry;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtGeometry input1;
	OtGeometry input2;
	OtGeometry input3;
	OtGeometry input4;
	OtGeometry output;
};

static OtNodesFactoryRegister<OtGeometryMergeNode> registration;
