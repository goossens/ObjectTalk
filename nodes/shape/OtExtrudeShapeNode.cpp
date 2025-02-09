//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <memory>

#include "imgui.h"
#include "tesselator.h"

#include "OtLog.h"

#include "OtGeometry.h"
#include "OtMesh.h"
#include "OtShape.h"

#include "OtNodesFactory.h"


//
//	OtExtrudeShapeNode
//

class OtExtrudeShapeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Shape", shape);
		addInputPin("Depth", depth);
		addOutputPin("Geometry", geometry);
	}

	// extrude a 2D shape
	void onExecute() override {
		if (shape.isValid()) {
			auto pathCount = shape.getPathCount();

			// create a tesselator
			TESStesselator* tess = tessNewTess(NULL);
    		tessSetOption(tess, TESS_CONSTRAINED_DELAUNAY_TRIANGULATION, 1);

			// create an empty mesh
			std::shared_ptr<OtMesh> mesh = std::make_shared<OtMesh>();

			// process all paths
			std::vector<glm::vec2> path;
			bool cw;

			for (auto i = 0; i < pathCount; i++) {
				// get the points on the path
				shape.getPath(path, i);

				// determine winding order of first path
				if (i == 0) {
					cw = OtShape::isPathClockwise(path);
				}

				// reverse path order if shape uses clockwise order for external contours
				if (cw) {
					std::reverse(path.begin(), path.end());
				}

				// add path to tesselator
				tessAddContour(tess, 2, path.data(), sizeof(glm::vec2), (int) path.size());

				// create faces to connect front and back
				for (auto j = 0; j < path.size() - 1; j++) {
					mesh->addFace(
						glm::vec3(path[j].x, path[j].y, 0.0),
						glm::vec3(path[j].x, path[j].y, -depth),
						glm::vec3(path[j + 1].x, path[j + 1].y, -depth),
						glm::vec3(path[j + 1].x, path[j + 1].y, 0.0));
				}
			}

			// perform the tesselation
			if (!tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, 3, 2, nullptr)) {
				OtLogFatal("Can't tesselate geometry");
			}

			// create the front and back-facing vertices (interwoven)
			int offset = (uint32_t) mesh->getVertexCount();
			int vertexCount = tessGetVertexCount(tess);
			const TESSreal* verts = tessGetVertices(tess);

			for (auto i = 0; i < vertexCount; i++) {
				auto x = *verts++;
				auto y = *verts++;

				mesh->addVertex(OtVertex(glm::vec3(x, y, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
				mesh->addVertex(OtVertex(glm::vec3(x, y, -depth), glm::vec3(0.0f, 0.0f, -1.0f)));
			}

			// create the front and back-facing indices
			int indexCount = tessGetElementCount(tess);
			const TESSindex* indices = tessGetElements(tess);

			for (auto i = 0; i < indexCount; i++) {
				auto i1 = offset + *indices++ * 2;
				auto i2 = offset + *indices++ * 2;
				auto i3 = offset + *indices++ * 2;
				mesh->addTriangle(i1, i2, i3);
				mesh->addTriangle(i1 + 1, i3 + 1, i2 + 1);
			}

			// cleanup
			tessDeleteTess(tess);

			// submit mesh to geometry
			geometry.setMesh(mesh);

		} else {
			// no valid shape so we clear out output
			geometry.clear();
		}
	}

	static constexpr const char* nodeName = "Extrude Shape";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::shape;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtShape shape;
	float depth = 1.0f;
	OtGeometry geometry;
};

static OtNodesFactoryRegister<OtExtrudeShapeNode> type;
