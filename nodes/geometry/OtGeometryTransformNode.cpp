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

#include "OtGeometry.h"

#include "OtNodesFactory.h"


//
//	OtGeometryTransform
//

class OtGeometryTransform : public OtNodeClass {
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
		// do we have a valid input
		if (input.isValid()) {
			// clone the mesh
			output.cloneFrom(input);

			// get accesss to the mesh and vertex list
			auto& mesh = output.getMesh();
			OtVertex* vertex = mesh.getVertices().data();
			auto count = mesh.getVertexCount();

			// transform all vertices
			if (hasVaryingInput()) {
				OtNodeVaryingContext context;

				for (auto i = 0; i < count; i++) {
					evaluateVariableInputs(context);

					auto transform =
						glm::translate(glm::mat4(1.0f), translate) *
						glm::toMat4(glm::quat(glm::radians(rotate))) *
						glm::scale(glm::mat4(1.0f), scale);

					vertex->position = glm::vec3(transform * glm::vec4(vertex->position, 1.0f));
					vertex->normal = glm::vec3(glm::normalize((transform * glm::vec4(vertex->normal, 0.0f))));
					vertex++;
				}

			} else {
				auto transform =
					glm::translate(glm::mat4(1.0f), translate) *
					glm::toMat4(glm::quat(glm::radians(rotate))) *
					glm::scale(glm::mat4(1.0f), scale);

				for (auto i = 0; i < count; i++) {
					vertex->position = glm::vec3(transform * glm::vec4(vertex->position, 1.0f));
					vertex->normal = glm::vec3(glm::normalize((transform * glm::vec4(vertex->normal, 0.0f))));
					vertex++;
				}
			}

			mesh.generateAABB();

		} else {
			// no valid input, just clear the output
			output.clear();
		}
	}

	static constexpr const char* nodeName = "Geometry Transform";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::geometry;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtGeometry input;
	OtGeometry output;
	glm::vec3 translate{0.0f};
	glm::vec3 rotate{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtGeometryTransform> type;
