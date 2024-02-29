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

#include "OtGeometry.h"
#include "OtImage.h"

#include "OtNodesFactory.h"


//
//	OtTransformGeometryNode
//

class OtTransformGeometryNode : public OtNodeClass {
public:
	// constructor
	inline OtTransformGeometryNode() : OtNodeClass(name, OtNodeClass::geometry) {}

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
			// determine transformation matrix
			auto transform = glm::translate(glm::mat4(1.0f), translate) *
				glm::toMat4(glm::quat(glm::radians(rotate))) *
				glm::scale(glm::mat4(1.0f), scale);

			// clone the mesh
			output.cloneMesh(input);
			auto& mesh = output.getMesh();

			// get acces to the vertices mesh
			OtVertex* vertex = mesh.getVertices().data();
			auto count = mesh.getVertexCount();

			// transform all vertices
			for (auto i = 0; i < count; i++) {
				vertex->position = glm::vec3(transform * glm::vec4(vertex->position, 1.0f));
				vertex->normal = glm::vec3(glm::normalize((transform * glm::vec4(vertex->normal, 1.0f))));
				vertex++;
			}

			// update geometries version number
			output.incrementVersion();

		} else {
			// no valid input, just clear the output
			output.clear();
		}
	}

	static constexpr const char* name = "Transform Geometry";

protected:
	OtGeometry input;
	OtGeometry output;
	glm::vec3 translate{0.0f};
	glm::vec3 rotate{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtTransformGeometryNode> type("Geometry");
