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

#include "OtInstances.h"

#include "OtNodesFactory.h"


//
//	OtTransformInstancesNode
//

class OtTransformInstancesNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", input);
		addInputPin("Translate", translate);
		addInputPin("Rotate", rotate);
		addInputPin("Scale", scale);
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
			if (hasVaryingInput()) {
				OtNodeVaryingContext context;

				for (auto i = 0; i < count; i++) {
					evaluateVariableInputs(context);

					output.add(
						glm::translate(glm::mat4(1.0f), translate) *
						glm::toMat4(glm::quat(glm::radians(rotate))) *
						glm::scale(glm::mat4(1.0f), scale) *
						input[i]);
				}

			} else {
				auto transform =
					glm::translate(glm::mat4(1.0f), translate) *
					glm::toMat4(glm::quat(glm::radians(rotate))) *
					glm::scale(glm::mat4(1.0f), scale);

				for (auto i = 0; i < count; i++) {
					output.add(transform * input[i]);
				}
			}
		}
	}

	static constexpr const char* nodeName = "Instances Transform";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::geometry;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtInstances input;
	OtInstances output;
	glm::vec3 translate{0.0f};
	glm::vec3 rotate{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtTransformInstancesNode> type;
