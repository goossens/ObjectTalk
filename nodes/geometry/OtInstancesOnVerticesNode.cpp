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
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGeometry.h"
#include "OtInstances.h"

#include "OtNodesFactory.h"


//
//	OtInstancesOnVerticesNode
//

class OtInstancesOnVerticesNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Geometry", geometry);
		addInputPin("Selection", selection);
		addInputPin("Rotate to Normals", rotateToNormals);
		addInputPin("Rotation", rotation);
		addInputPin("Scale", scale);
		addOutputPin("Instances", instances);
	}

	// create instance for each geometry vertex
	void onExecute() override {
		// reset list of instances
		instances.clear();

		// do we have a valid input
		if (geometry.isValid()) {

			// get accesss to the mesh and vertex list
			auto& mesh = geometry.getMesh();
			OtVertex* vertex = mesh.getVertices().data();
			auto count = mesh.getVertexCount();

			// turn each vertex into an instance
			if (hasVaryingInput()) {
				for (size_t i = 0; i < count; i++) {
					OtNodeVaryingContext context(static_cast<int>(i), vertex[i]);
					evaluateVariableInputs(context);

					if (selection) {
						auto transform = glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);
						generateInstance(vertex[i], transform);
					}
				}

			} else {
				auto transform = glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);

				for (size_t i = 0; i < count; i++) {
					if (selection) {
						generateInstance(*vertex++, transform);
					}
				}
			}
		}
	}

	static constexpr const char* nodeName = "Instances on Vertices";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::geometry;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// generate a single instance
	void generateInstance(OtVertex& vertex, glm::mat4& transform) {
		// rotate to normals (if required)
		if (rotateToNormals) {
			static glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 v = vertex.normal;
			auto quat = glm::normalize(glm::quat(1.0f + glm::dot(u, v), glm::cross(u, v)));
			instances.add(glm::translate(glm::mat4(1.0f), vertex.position) * glm::toMat4(quat) * transform, false);

		} else {
			instances.add(glm::translate(glm::mat4(1.0f), vertex.position) * transform, false);
		}
	}

protected:
	OtGeometry geometry;
	OtInstances instances;
	bool selection = true;
	bool rotateToNormals = false;
	glm::vec3 rotation{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtInstancesOnVerticesNode> registration;
