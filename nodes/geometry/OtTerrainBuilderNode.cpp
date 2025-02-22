//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtGeometry.h"
#include "OtImage.h"

#include "OtNodesFactory.h"


//
//	OtTerrainBuilderNode
//

class OtTerrainBuilderNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", image);
		addOutputPin("Output", geometry);
	}

	// create grid and apply heightmap
	void onExecute() override {
		// do we have a valid input
		if (image.isValid()) {
			// recreate mesh if required
			auto imageWidth = image.getWidth();
			auto imageHeight = image.getHeight();

			if (width != imageWidth || depth != imageHeight) {
				width = imageWidth;
				depth = imageHeight;
				createMesh();
			}

			// update heightmap
			updateHeights();

		} else {
			// no valid input, just clear the geometry
			geometry.clear();
		}
	}

	// create the terrain mesh
	void createMesh() {
		// clear geometry and get access to mesh
		geometry.clear();
		auto& mesh = geometry.getMesh();

		// add vertices
		for (auto vz = 0; vz < depth; vz++) {
			auto v = (float) vz / (depth - 1);

			for (auto vx = 0; vx < width; vx++) {
				auto u = (float) vx / (width - 1);

				mesh.addVertex(OtVertex(
					glm::vec3(u - 0.5f, 0.0f, v - 0.5f),
					glm::vec3(0.0f, 1.0f, 0.0f),
					glm::vec2(u, v)));
			}
		}

		// add triangles
		for (auto vz = 0; vz < (depth - 1); vz++) {
			for (auto vx = 0; vx < (width - 1); vx++) {
				auto a = vz * width + vx;
				auto b = a + 1;
				auto c = a + width;
				auto d = c + 1;

				mesh.addTriangle(a, c, d);
				mesh.addTriangle(a, d, b);
			}
		}
	}

	// update the terrain heights
	void updateHeights() {
		// get accesss to the mesh and vertex list
		auto& mesh = geometry.getMesh();
		OtVertex* vertex = mesh.getVertices(true).data();

		// update height of each vertex
		for (auto vz = 0; vz < depth; vz++) {
			for (auto vx = 0; vx < width; vx++) {
				(vertex++)->position.y = image.getPixelGray(vx, vz);
			}
		}

		// now we need to update all normals
		mesh.generateNormals();

		// update geometry version number
		geometry.incrementVersion();
	}

	static constexpr const char* nodeName = "Terrain Builder";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::geometry;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtImage image;
	OtGeometry geometry;
	int width = 0;
	int depth = 0;
};

static OtNodesFactoryRegister<OtTerrainBuilderNode> registration;
