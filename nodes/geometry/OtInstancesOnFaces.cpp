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

#include "OtHash.h"

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
		addInputPin("Seed", seed);
		addInputPin("Instance Count", instanceCount);
		addInputPin("Selection", selection);
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
					OtVertex p = getRandomPoint(i, vertices, indices, triangles);
					OtNodeVaryingContext context(i, p);
					evaluateVariableInputs(context);

					if (selection) {
						glm::mat4 transform =
							glm::toMat4(glm::quat(glm::radians(rotation))) *
							glm::scale(glm::mat4(1.0f), scale);

						// add a new instance
						addInstance(p, transform);
					}
				}

			}  else {
				// determine shared transformation
				glm::mat4 transform =
					glm::toMat4(glm::quat(glm::radians(rotation))) *
					glm::scale(glm::mat4(1.0f), scale);

				// generate instances
				for (auto i = 0; i < instanceCount; i++) {
					OtVertex p = getRandomPoint(i, vertices, indices, triangles);
					addInstance(p, transform);
				}
			}
		}

		// mark instances as a new version
		instances.incrementVersion();
	}

	static constexpr const char* nodeName = "Instances on Faces";
	static constexpr int nodeCategory = OtNodeClass::geometry;
	static constexpr int nodeKind = OtNodeClass::fixed;

private:
	// add new instance to the list
	void addInstance(OtVertex& vertex, glm::mat4& transform) {
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

	// get random point inside triangle based on barycentric coordinate
	OtVertex getRandomPoint(int id, OtVertex* vertices, uint32_t* indices, size_t triangles) {
		// select a random triangle
		int triangle = int(OtHash::toFloat(id, seed) * float(triangles - 1));

		// get the corners of the triangle
		auto index = indices + triangle * 3;
		auto& v1 = vertices[index[0]];
		auto& v2 = vertices[index[1]];
		auto& v3 = vertices[index[2]];

		// generate a random barycentric coordinate
		auto a = OtHash::toFloat(id, seed, 1);
		auto b = OtHash::toFloat(id, seed, 2);

		if (a + b > 1.0f) {
			a = 1.0f - a;
			b = 1.0f - b;
		}

		auto bc = glm::vec3(a, b, 1.0f - a - b);

		// generate vertex at random point
		return OtVertex(
			v1.position * bc.x + v2.position * bc.y + v3.position * bc.z,
			glm::normalize(v1.normal * bc.x + v2.normal * bc.y + v3.normal * bc.z),
			v1.uv * bc.x + v2.uv * bc.y + v3.uv * bc.z);
	}

protected:
	OtGeometry geometry;
	int seed;
	OtInstances instances;
	int instanceCount = 16;
	bool selection = true;
	bool rotateToNormals = false;
	glm::vec3 rotation{0.0f};
	glm::vec3 scale{1.0f};
};

static OtNodesFactoryRegister<OtInstancesOnFacesNode> type;
