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
//	OtShapeTransformNode
//

class OtShapeTransformNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", input);
		addInputPin("Translate", translate);
		addInputPin("Rotate", rotate);
		addInputPin("Scale", scale);
		addOutputPin("Output", output);
	}

	// apply transformation to shape
	inline void onExecute() override {
		// do we have a valid input
		if (input.isValid()) {
			// clone the shape and transform it
			output.cloneFrom(input);
			output.transform(translate.x, translate.y, rotate, scale.x, scale.y);

		} else {
			output.clear();
		}
	}

	static constexpr const char* nodeName = "Shape Transform";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::shape;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtShape input;
	OtShape output;
	glm::vec2 translate{0.0f};
	float rotate{0};
	glm::vec2 scale{1.0f};
};

static OtNodesFactoryRegister<OtShapeTransformNode> registration;
