//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "OtShape.h"

#include "OtNodesFactory.h"


//
//	OtMergeShapesNode
//

class OtMergeShapesNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addInputPin("B", b);
		addOutputPin("Output", output);
	}

	// apply transformation to shape
	void onExecute() override {
		output.cloneFrom(a);
		output.mergeWith(b);
	}

	static constexpr const char* nodeName = "Merge Shapes";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::shape;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtShape a;
	OtShape b;
	OtShape output;
};

static OtNodesFactoryRegister<OtMergeShapesNode> type;
