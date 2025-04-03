//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <set>
#include <vector>

#include "delaunator.h"
#include "glm/glm.hpp"

#include "OtHash.h"
#include "OtLog.h"

#include "OtCanvas.h"
#include "OtNoise.h"

#include "OtNodesFactory.h"
#include "OtTextureGeneratorNode.h"


//
//	OtMapGeneratorNode
//

class OtMapGeneratorNode : public OtTextureGeneratorNode {
public:
	// configure node
	inline void configure() override {
		OtTextureGeneratorNode::configure();
		addInputPin("Seed", seed);
		addInputPin("Size", size);
		addInputPin("Disturbance", disturbance);
	}

	// validate input parameters
	inline void onValidate() override {
		seed = std::clamp(seed, 1, 256);
		size = std::clamp(size, 4, 128);
		disturbance = std::clamp(disturbance, 0.0f, 1.0f);
	}

	// run the map generator
	inline void onGenerate(OtFrameBuffer& output) override {
		// generate the map
		generatePoints();
		triangulate();
		calculateCentroids();
		calculateHeights();
		calculateMoisture();

		// render the map
		canvas.render(output, 1.0f, [this]() {
			auto scale = static_cast<float>(std::min(width, height)) / static_cast<float>(size);
			canvas.scale(scale, scale);
			auto numEdges = halfedges.size();
			std::set<size_t> seen;

			for (size_t edge = 0; edge < numEdges; edge++) {
				auto region = triangles[nextHalfedge(edge)];

				if (seen.find(region) == seen.end()) {
					seen.insert(region);

					std::vector<glm::vec2> p;
					getPolygon(edge, p);

					setBiomeColor(region);
					canvas.beginPath();
					canvas.moveTo(p[0].x, p[0].y);

					for (size_t i = 1; i < p.size(); i++) {
						canvas.lineTo(p[i].x, p[i].y);
					}

					canvas.closePath();
					canvas.fill();
				}
			}
		});
	}

	void generatePoints() {
		// render jitter grid
		points.clear();

		for (auto gy = 0; gy <= size; gy++) {
			points.emplace_back(-1.0f, static_cast<float>(gy));

			for (auto gx = 0; gx <= size; gx++) {
				points.emplace_back(
					(OtHash::toFloat(gx, gy, seed, 0) * 2.0f - 1.0f) * disturbance + gx,
					(OtHash::toFloat(gx, gy, seed, 1) * 2.0f - 1.0f) * disturbance + gy);
			}

			points.emplace_back(static_cast<float>(size + 1), static_cast<float>(gy));
		}

		for (auto gx = -1; gx <= size + 1; gx++) {
			points.emplace_back(static_cast<float>(gx), -1.0f);
			points.emplace_back(static_cast<float>(gx), static_cast<float>(size + 1));
		}
	}

	void triangulate() {
		// perform Delaunay triangulation
		std::vector<double> coords;

		for (auto& point : points) {
			coords.emplace_back(point.x);
			coords.emplace_back(point.y);
		}

		delaunator::Delaunator delaunator(coords);
		triangles = delaunator.triangles;
		halfedges = delaunator.halfedges;
	}

	void calculateCentroids() {
		// determine centroids
		centroids.clear();
		heights.clear();
		size_t numTriangles = triangles.size() / 3;

		for (size_t c = 0; c < numTriangles; c ++) {
			glm::vec2 sum(0.0f);

			for (size_t i = 0; i < 3; i++) {
				sum += points[triangles[3 * c + i]];
			}

			centroids.emplace_back(sum / 3.0f);
		}
	}

	void calculateHeights() {
		// determine cell heights
		heights.clear();
		OtNoise noise;

		for (auto& point : points) {
			auto nx = point.x / size - 0.5f;
			auto ny = point.y / size - 0.5f;
			auto h = (1.0f + noise.perlin(nx / 0.5f, ny / 0.5f, seed)) / 2.0f;
			auto d = std::max(std::abs(nx), std::abs(ny)) * 2.0f;
			heights.emplace_back((1.0f + h - d) / 2.0f);
		}
	}

	void calculateMoisture() {
		// determine cell heights
		moisture.clear();
		OtNoise noise;

		for (auto& point : points) {
			auto nx = point.x / size - 0.5f;
			auto ny = point.y / size - 0.5f;
			moisture.emplace_back((1.0f + noise.perlin(nx / 0.5f, ny / 0.5f, seed * 7)) / 2.0f);
		}
	}

	void getPolygon(size_t start, std::vector<glm::vec2>& p) {
		p.clear();
		auto incoming = start;

		do {
			p.emplace_back(centroids[triangleOfEdge(incoming)]);
			auto outgoing = nextHalfedge(incoming);
			incoming = halfedges[outgoing];
		} while (incoming != delaunator::INVALID_INDEX && incoming != start);
	}

	void setBiomeColor(size_t region) {
		auto h = (heights[region] - 0.5f) * 2.0f;
		auto m = moisture[region];

		if (h < 0.0f) {
			canvas.fillColor(0.2f + 0.2f * h, 0.25f + 0.25f * h, 0.5f + 0.5f * h, 1.0f);

		} else {
			m = m * (1.0f - h);
			h = h * h * h * h;

			auto r = 0.8f - 0.4f * m;
			auto g = 0.7f - 0.2f * m;
			auto b = 0.5f - 0.2f * m;

			r = h + r * (1.0f - h),
			g = h + g * (1.0f - h),
			b = h + b * (1.0f - h);

			canvas.fillColor(r, g, b, 1.0f);
		}
	}

	static constexpr const char* nodeName = "Map Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::generate;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int seed = 1;
	int size = 32;
	float disturbance = 0.2f;

	// map components
	std::vector<glm::vec2> points;
	std::vector<glm::vec2> centroids;
	std::vector<float> heights;
	std::vector<float> moisture;
    std::vector<std::size_t> triangles;
    std::vector<std::size_t> halfedges;

	// utility functions
	inline size_t triangleOfEdge(size_t e) { return e / 3; }
	inline size_t nextHalfedge(size_t e) { return (e % 3 == 2) ? e - 2 : e + 1; }

	// rendering support
	OtCanvas canvas;
};


static OtNodesFactoryRegister<OtMapGeneratorNode> registration;
