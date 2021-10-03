//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <array>
#include <cmath>
#include <memory>

#include "glm/glm.hpp"

#include "ot/function.h"

#include "terrain.h"



//
//	TerrainTile class
//

class TerrainTile {
public:
	TerrainTile(float _x, float _y, float _w, float _h, int l) : x(_x), y(_y), w(_w), h(_h), level(l) {}
	std::array<std::shared_ptr<TerrainTile>, 4> children;
	float x, y, w, h;
	int level;

	void createChildren() {
		auto w2 = w / 2.0;
		auto h2 = h / 2.0;

		children[0] = std::make_shared<TerrainTile>(x - w2, y - h2, w2, h2, level + 1);
		children[1] = std::make_shared<TerrainTile>(x + w2, y - h2, w2, h2, level + 1);
		children[2] = std::make_shared<TerrainTile>(x - w2, y + h2, w2, h2, level + 1);
		children[3] = std::make_shared<TerrainTile>(x + w2, y + h2, w2, h2, level + 1);
	}

	bool isLeaf() {
		return children[0] == nullptr;
	}

	bool inside(float _x, float _y) {
		return _x >= x - w / 2.0 && _x <= x + w / 2.0 &&
			_y >= y - h / 2.0 && _y <= y + h / 2.0;
	}

	float distance(float _x, float _y) {
		return std::sqrt((x - _x) * (x - _x) + (y - _y) * (y - _y));
	}
};


//
//	QuadTree class
//

class QuadTree {
public:
	// constructor
	QuadTree(float x, float y, float w, float h, float mts, glm::vec3 camera) {
		root = std::make_shared<TerrainTile>(x, y, w, h, 0);
		minTileSize = mts;
		setup(root, camera.x, camera.z);
	}

	// get list of all terrain tiles (i.e. all the leaves in the tree)
	void getTerrainTiles(std::vector<std::shared_ptr<TerrainTile>>& tiles, std::shared_ptr<TerrainTile> tile = nullptr) {
		// our root tile is the default
		if (!tile) {
			tile = root;
		}

		// if we hit a leaf, add it to the list
		if (tile->isLeaf()) {
			tiles.push_back(tile);

		} else {
			// we hit a branch so we need to check all children
			getTerrainTiles(tiles, tile->children[0]);
			getTerrainTiles(tiles, tile->children[1]);
			getTerrainTiles(tiles, tile->children[2]);
			getTerrainTiles(tiles, tile->children[3]);
		}
	}

private:
	// minimum tile size
	float minTileSize;

	// our root tile
	std::shared_ptr<TerrainTile> root;

	// setup tiles recursively based on camera position
	void setup(std::shared_ptr<TerrainTile> tile, float x, float y) {
		// we're done dividing tiles when the distance is too great or we have reached the lowest level
		if (tile->distance(x, y) < tile->w && tile->w > minTileSize) {
			// split tile into 4 children
			tile->createChildren();

			// recursively setup al children
			setup(tile->children[0], x, y);
			setup(tile->children[1], x, y);
			setup(tile->children[2], x, y);
			setup(tile->children[3], x, y);
		}
	}
};


//
//	OtTerrainClass::OtTerrainClass
//

OtTerrainClass::OtTerrainClass() {
}


//
//	OtTerrainClass::~OtTerrainClass
//

OtTerrainClass::~OtTerrainClass() {
}


//
//	OtTerrainClass::init
//

OtObject OtTerrainClass::init(size_t count, OtObject* parameters) {
	if (count == 1) {

	} else if (count != 0) {
		OtExcept("[Terrain] constructor expects 0 or 1 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtTerrainClass::render
//

void OtTerrainClass::render(int view, OtCamera camera, glm::mat4 parentTransform) {
	// build a quad tree based on the camera's position
	QuadTree quadtree = QuadTree(0, 0, range, range, minimum, camera->getPosition());

	// get a list of all relevant terrain tiles
	std::vector<std::shared_ptr<TerrainTile>> tiles;
	quadtree.getTerrainTiles(tiles);
}


//
//	OtTerrainClass::getMeta
//

OtType OtTerrainClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtTerrainClass>("Terrain", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtTerrainClass::init));
	}

	return type;
}


//
//	OtTerrainClass::create
//

OtTerrain OtTerrainClass::create() {
	OtTerrain terrain = std::make_shared<OtTerrainClass>();
	terrain->setType(getMeta());
	return terrain;
}
