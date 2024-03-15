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
				for (auto i = 0; i < count; i++) {
					evaluateVariableInputs();
					auto transform = glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);
					generateInstance(vertex++, transform);
				}

			} else {
				auto transform = glm::toMat4(glm::quat(glm::radians(rotation))) * glm::scale(glm::mat4(1.0f), scale);

				for (auto i = 0; i < count; i++) {
					generateInstance(vertex++, transform);
				}
			}
		}
	}

	static constexpr const char* nodeName = "Instances on Vertices";
	static constexpr int nodeCategory = OtNodeClass::geometry;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 180.0f;

private:
	// generate a single instance
	void generateInstance(OtVertex* vertex, glm::mat4& transform) {
		// rotate to normals (if required)
		if (rotateToNormals) {
			static glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 v = vertex->normal;
			auto quat = glm::normalize(glm::quat(1.0f + glm::dot(u, v), glm::cross(u, v)));
			instances.add(glm::translate(glm::mat4(1.0f), vertex->position) * glm::toMat4(quat) * transform, false);

		} else {
			instances.add(glm::translate(glm::mat4(1.0f), vertex->position) * transform, false);
		}
	}

protected:
	OtGeometry geometry;
	OtInstances instances;
	bool rotateToNormals = false;
	glm::vec3 rotation{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtInstancesOnVerticesNode> type;
