//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtNumbers.h"

#include "OtGeometry.h"
#include "OtInstances.h"

#include "OtNodesFactory.h"


//
//	OtInstancesOnFacesNode
//

class OtInstancesOnFacesNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Geometry", geometry);
		addInputPin("Instance Count", instanceCount);
		addInputPin("Rotate to Normals", rotateToNormals);
		addInputPin("Rotation", rotation);
		addInputPin("Scale", scale);
		addOutputPin("Instances", instances);
	}

	// randomly create instances on geometry faces
	void onExecute() override {
		// reset list of instances
		instances.clear();

		// do we have a valid input
		if (geometry.isValid()) {
			// get accesss to the mesh and vertex list
			auto& mesh = geometry.getMesh();
			OtVertex* vertices = mesh.getVertices().data();
			uint32_t* indices = mesh.getIndices().data();
			auto triangles = mesh.getIndexCount() / 3;

			// generate instances
			if (hasVaryingInput()) {
				for (auto i = 0; i < instanceCount; i++) {
					evaluateVariableInputs();

					glm::mat4 transform =
						glm::toMat4(glm::quat(glm::radians(rotation))) *
						glm::scale(glm::mat4(1.0f), scale);

					// generate instance
					generateInstance(vertices, indices, triangles, transform);
				}

			}  else {
				// determine shared transformation
				glm::mat4 transform =
					glm::toMat4(glm::quat(glm::radians(rotation))) *
					glm::scale(glm::mat4(1.0f), scale);

				// generate instances
				for (auto i = 0; i < instanceCount; i++) {
					generateInstance(vertices, indices, triangles, transform);
				}
			}
		}
	}

	static constexpr const char* nodeName = "Instances on Faces";
	static constexpr int nodeCategory = OtNodeClass::geometry;
	static constexpr int nodeKind = OtNodeClass::fixed;

private:
	// generate a single instance
	void generateInstance(OtVertex* vertices, uint32_t* indices, size_t triangles, glm::mat4& transform) {
		// select a random triangle
		int triangle = OtRandom(0, int(triangles) - 1);

		// get the corners of the triangle
		auto index = indices + triangle * 3;
		auto& v1 = vertices[index[0]];
		auto& v2 = vertices[index[1]];
		auto& v3 = vertices[index[2]];

		// pick a random point within the triangle
		auto p = getRandomPointOnTriangle(v1.position, v2.position, v3.position);

		// rotate to normals (if required)
		if (rotateToNormals) {
			static glm::vec3 u = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 v = glm::normalize((v1.normal + v2.normal + v3.normal) / 3.0f);
			auto quat = glm::normalize(glm::quat(1.0f + glm::dot(u, v), glm::cross(u, v)));
			instances.add(glm::translate(glm::mat4(1.0f), p) * glm::toMat4(quat) * transform, false);

		} else {
			instances.add(glm::translate(glm::mat4(1.0f), p) * transform, false);
		}
	}

	// get random point inside triangle based on barycentric coordinate
	glm::vec3 getRandomPointOnTriangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
		auto bc = randomBaryentricCoordinate();

		return glm::vec3(
			p1.x * bc.x + p2.x * bc.y + p3.x * bc.z,
			p1.y * bc.x + p2.y * bc.y + p3.y * bc.z,
			p1.z * bc.x + p2.z * bc.y + p3.z * bc.z);
	}

	// generate a random barycentric coordinate
	glm::vec3 randomBaryentricCoordinate() {
		auto a = OtRandom(0.0f, 1.0f);
		auto b = OtRandom(0.0f, 1.0f);

		if (a + b > 1.0f) {
			a = 1.0f - a;
			b = 1.0f - b;
		}

		return glm::vec3(a, b, 1.0f - a - b);
	}

protected:
	OtGeometry geometry;
	OtInstances instances;
	int instanceCount = 16;
	bool rotateToNormals = false;
	glm::vec3 rotation{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtInstancesOnFacesNode> type;
