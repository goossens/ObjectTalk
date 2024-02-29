//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtTerrainBuilder
//

class OtTerrainBuilder : public OtNodeClass {
public:
	// constructor
	inline OtTerrainBuilder() : OtNodeClass(name, OtNodeClass::geometry) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", image);
		addOutputPin("Output", geometry);
	}

	// create grid and apply heightmap
	void onExecute() override {
		// do we have a valid input
		if (image.isValid()) {
			// get the mesh
			auto& mesh = geometry.getMesh();

			// get image dimensions
			auto imageWidth = image.getWidth();
			auto imageHeight = image.getHeight();

			// update geometry if required
			if (width != imageWidth || depth != imageHeight) {
				width = imageWidth;
				depth = imageHeight;
				createMesh(mesh);
			}

			// update heightmap
			updateHeights(mesh);

			// update geometries version number
			geometry.incrementVersion();

		} else {
			// no valid input, just clear the geometry
			geometry.clear();
		}
	}

	// create the terrain mesh
	void createMesh(OtMesh& mesh) {
		// clear mesh first
		mesh.clear();

		// add vertices
		for (auto z = 0; z < depth; z++) {
			auto v = (float) z / (depth - 1);

			for (auto x = 0; x < width; x++) {
				auto u = (float) x / (width - 1);

				mesh.addVertex(OtVertex(
					glm::vec3(u - 0.5f, 0.0f, v - 0.5f),
					glm::vec3(0.0f, 1.0f, 0.0f),
					glm::vec2(u, v)));
			}
		}

		// add triangles
		for (auto z = 0; z < depth; z++) {
			for (auto x = 0; x < width; x++) {
				auto a = x + width * z;
				auto b = a + 1;
				auto c = a + width;
				auto d = c + 1;

				mesh.addTriangle(a, c, d);
				mesh.addTriangle(a, d, b);
			}
		}
	}

	// update the terrain heights
	void updateHeights(OtMesh& mesh) {
		OtVertex* vertex = mesh.getVertices(true).data();

		for (auto z = 0; z < depth; z++) {
			for (auto x = 0; x < width; x++) {
				(vertex++)->position.y = image.getPixelGray(x, z);
			}
		}

		mesh.generateNormals();
	}

	static constexpr const char* name = "Terrain Builder";

protected:
	OtImage image;
	OtGeometry geometry;
	int width = 0;
	int depth = 0;
};

static OtNodesFactoryRegister<OtTerrainBuilder> type("Geometry");
