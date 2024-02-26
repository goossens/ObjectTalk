//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGeometry.h"
#include "OtImage.h"

#include "OtNodesFactory.h"


//
//	OtTerrainBuilder
//

class OtTerrainBuilder : public OtNodeClass {
public:
	// constructor
	inline OtTerrainBuilder() : OtNodeClass(name, OtNodeClass::geometry) {}

	// configure node
	inline void configure() override {
		addInputPin("Width", width);
		addInputPin("Depth", depth);
		addInputPin("Input", image);
		addOutputPin("Output", geometry);
	}

	// create grid and apply heightmap
	void onExecute() override {
		// update geometry if required
		auto& mesh = geometry.getMesh();

		if (mesh.getVertexCount() != (width + 1) * (depth + 1)) {
			createMesh(mesh);
		}

		// update heightmap if required
		if (image.isValid()) {
			updateHeights(mesh);
		}

		// update geometries version number
		geometry.incrementVersion();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["width"] = width;
		(*data)["depth"] = depth;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		width = data->value("width", 256);
		depth = data->value("depth", 256);
	}

	// create the terrain mesh
	void createMesh(OtMesh& mesh) {
		mesh.clear();

		// add vertices
		auto gridX1 = width + 1;
		auto gridZ1 = depth + 1;

		auto segmentWidth = 1.0f / (float) width;
		auto segmentDepth = 1.0f / (float) depth;

		for (auto iz = 0; iz < gridZ1; iz++) {
			auto z = iz * segmentDepth - 0.5f;

			for (auto ix = 0; ix < gridX1; ix++) {
				auto x = ix * segmentWidth - 0.5f;
				auto u = (float) ix / (float) width;
				auto v = (float) iz / (float) depth;

				mesh.addVertex(OtVertex(
					glm::vec3(x, 0.0f, z),
					glm::vec3(0.0f, 1.0f, 0.0f),
					glm::vec2(u, v)));
			}
		}

		// add triangles
		for (auto iz = 0; iz < depth; iz++) {
			for (auto ix = 0; ix < width; ix++) {
				auto a = ix + gridX1 * iz;
				auto b = a + 1;
				auto c = a + gridX1;
				auto d = c + 1;

				mesh.addTriangle(a, c, d);
				mesh.addTriangle(a, d, b);
			}
		}
	}

	// update the terrain heights
	void updateHeights(OtMesh& mesh) {
		mesh.postProcess([&](std::vector<OtVertex>& vertices, std::vector<uint32_t>& indices) {
			for (auto& vertex : vertices) {
				vertex.position.y = image.sampleValue(vertex.uv.x, vertex.uv.y);
			}
		});

		mesh.generateNormals();
	}

	static constexpr const char* name = "Terrain Builder";

protected:
	int width = 256;
	int depth = 256;
	OtImage image;
	OtGeometry geometry;
};

static OtNodesFactoryRegister<OtTerrainBuilder> type("Geometry");
