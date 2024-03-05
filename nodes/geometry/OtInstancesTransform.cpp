//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "OtInstances.h"

#include "OtNodesFactory.h"


//
//	OtTransformInstancesNode
//

class OtTransformInstancesNode : public OtNodeClass {
public:
	// constructor
	inline OtTransformInstancesNode() : OtNodeClass(name, OtNodeClass::geometry) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", input);
		addInputPin("Translate", translate, true);
		addInputPin("Rotate", rotate, true);
		addInputPin("Scale", scale, true);
		addOutputPin("Output", output);
	}

	// apply transformation to geometry
	void onExecute() override {
		// clear the output
		output.clear();

		// do we have a valid input
		if (input.isValid()) {
			// transform all input instances to the output
			auto count = input.size();

			// transform all vertices
			for (auto i = 0; i < count; i++) {
				evaluateVariableInputs();

				output.add(
					glm::translate(glm::mat4(1.0f), translate) *
					glm::toMat4(glm::quat(glm::radians(rotate))) *
					glm::scale(glm::mat4(1.0f), scale) *
					input[i]);
			}
		}
	}

	static constexpr const char* name = "Instances Transform";

protected:
	OtInstances input;
	OtInstances output;
	glm::vec3 translate{0.0f};
	glm::vec3 rotate{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtTransformInstancesNode> type("Geometry");
