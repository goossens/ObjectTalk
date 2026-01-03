//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtManifold.h"

#include "OtNodesFactory.h"


//
//	OtManifoldTransformNode
//

class OtManifoldTransformNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", input);
		addInputPin("Translate", translate);
		addInputPin("Rotate", rotate);
		addInputPin("Scale", scale);
		addOutputPin("Output", output);
	}

	// create the manifold
	inline void onExecute() override {
		if (input.isValid()) {
			auto temp = input.scale(scale);
			temp = temp.rotate(rotate);
			output = temp.translate(translate);

		} else {
			output.clear();
		}
	}

	static constexpr const char* nodeName = "Manifold Transform";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::manifold;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtManifold input;
	OtManifold output;
	glm::vec3 translate{0.0f};
	glm::vec3 rotate{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtManifoldTransformNode> registration;
