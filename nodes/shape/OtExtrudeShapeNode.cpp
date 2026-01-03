//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
		addInputPin("Tolerance", tolerance);
		addOutputPin("Geometry", geometry);
	}

	// extrude a 2D shape
	inline void onExecute() override {
		if (shape.isValid()) {
			// create a tesselator
			TESStesselator* tess = tessNewTess(NULL);
			tessSetOption(tess, TESS_CONSTRAINED_DELAUNAY_TRIANGULATION, 1);

			// create an empty mesh
			std::shared_ptr<OtMesh> mesh = std::make_shared<OtMesh>();

			// get the shape's path segments
			std::vector<glm::vec2> points;
			std::vector<size_t> sizes;
			shape.getSegments(points, sizes, tolerance);
			size_t start = 0;

			// determine winding order of first segment
			auto n = sizes[0];
			float area = 0.0f;

			for (size_t p = n - 1, q = 0; q < n; p = q++) {
				area += points[p].x * points[q].y - points[q].x * points[p].y;
			}

			bool cw = area < 0;

			// process all segments
			for (auto size : sizes) {
				// reverse order if shape uses clockwise order for external contours (i.e. first segment)
				if (cw) {
					auto first = points.begin() + start;
					std::reverse(first, first + size);
				}

				// add segment to the tesselator
				tessAddContour(tess, 2, &points[start], sizeof(glm::vec2), (int) size);

				// create faces to connect front and back of segment
				for (auto j = start; j < start + size - 1; j++) {
					mesh->addFace(
						glm::vec3(points[j].x, points[j].y, 0.0),
						glm::vec3(points[j].x, points[j].y, -depth),
						glm::vec3(points[j + 1].x, points[j + 1].y, -depth),
						glm::vec3(points[j + 1].x, points[j + 1].y, 0.0));
				}

				start += size;
			}

			// perform the tesselation
			if (!tessTesselate(tess, TESS_WINDING_ODD, TESS_POLYGONS, 3, 2, nullptr)) {
				OtLogFatal("Can't tesselate geometry");
			}

			// create the front and back-facing vertices (interwoven)
			int offset = static_cast<uint32_t>(mesh->getVertexCount());
			int vertexCount = tessGetVertexCount(tess);
			const TESSreal* verts = tessGetVertices(tess);

			for (auto i = 0; i < vertexCount; i++) {
				auto vx = *verts++;
				auto vy = *verts++;

				mesh->addVertex(OtVertex(glm::vec3(vx, vy, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
				mesh->addVertex(OtVertex(glm::vec3(vx, vy, -depth), glm::vec3(0.0f, 0.0f, -1.0f)));
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

private:
	// properties
	OtShape shape;
	float depth = 1.0f;
	float tolerance = 0.25f;
	OtGeometry geometry;
};

static OtNodesFactoryRegister<OtExtrudeShapeNode> registration;
