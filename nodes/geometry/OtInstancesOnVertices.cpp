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
	// constructor
	inline OtInstancesOnVerticesNode() : OtNodeClass(name, OtNodeClass::geometry) {}

	// configure node
	inline void configure() override {
		addInputPin("Geometry", geometry);
		addInputPin("Rotation", rotation);
		addInputPin("Scale", scale);
		addOutputPin("Instances", instances);
	}

	// rendering custom fields
	void customRendering(float width) override {
		auto old = serialize().dump();

		if (ImGui::Checkbox("Rotate to Normals", &rotateToNormals)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	float getCustomRenderingWidth() override {
		return fieldWidth;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["rotateToNormals"] = rotateToNormals;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		rotateToNormals = data->value("rotateToNormals", false);
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
			for (auto i = 0; i < count; i++) {
				// determine transformation for instance
				glm::mat4 transform =
					glm::toMat4(glm::quat(glm::radians(rotation))) *
					glm::scale(glm::mat4(1.0f), scale);

				// rotate to normals (if required)
				if (rotateToNormals) {
					static glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
					glm::vec3 v = vertex->normal;
					auto quat = glm::normalize(glm::quat(1.0f + glm::dot(u, v), glm::cross(u, v)));
					instances.add(glm::translate(glm::mat4(1.0f), vertex->position) * glm::toMat4(quat) * transform, false);

				} else {
					instances.add(glm::translate(glm::mat4(1.0f), vertex->position) * transform, false);
				}

				vertex++;
			}
		}
	}

	static constexpr const char* name = "Instances on Vertices";
	static constexpr float fieldWidth = 180.0f;

protected:
	OtGeometry geometry;
	OtInstances instances;
	bool rotateToNormals = false;
	glm::vec3 rotation{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtInstancesOnVerticesNode> type("Geometry");
