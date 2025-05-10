//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//  Include files
//

#include <vector>

#include "fmt/format.h"

#include "OtMapTile.h"
#include "OtTileLayer.h"


//
//  OtTileLayer::render
//

void OtTileLayer::render(ImDrawList* drawlist, OtProjector& projector) {
	// determine edges of visible map portion in raster coordinates
	OtMapRaster minR, maxR;
	projector.visibleRasterBoundaries(minR, maxR);

	// determine visible tiles
	std::vector<std::shared_ptr<OtMapImageTile>> tiles;
	bool done = false;

	// process all zoom levels until a complete (opaque) level is found
	for (int c = projector.getZoom(); c > 0 && !done; c--) {
		// get extent of visible tiles
		OtMapTile minT = projector.rasterToTile(minR, c);
		OtMapTile maxT = projector.rasterToTile(maxR, c);

		// see if a layer is complete
		bool incomplete = false;

		// process all visible tiles
		for (int x = minT.x; x <= maxT.x; x++) {
			for (int y = minT.y; y <= maxT.y; y++) {
				std::shared_ptr<OtMapImageTile> image = getTile(OtMapTile(c, x, y));
				tiles.push_back(image);
				incomplete |= !image->isOpaque();
			}
		}

		done = !incomplete;
	}

	// render all visible tiles (in reverse order)
	for (auto i = tiles.rbegin(); i != tiles.rend(); i++) {
		(*i)->render(drawlist, projector);
	}
}


//
//  OtTileLayer::getTile
//

std::shared_ptr<OtMapImageTile> OtTileLayer::getTile(const OtMapTile& tile) {
	// see if tile is in memory cache
	std::shared_ptr<OtMapImageTile> imageTile;

	if (cache.has(tile.hash)) {
		imageTile = cache.get(tile.hash);

	} else {
		// no, load new tile
		auto url = fmt::format("http://{}/{}/{}/{}.png", host, tile.zoom, tile.x, tile.y);
		imageTile = std::make_shared<OtMapImageTile>(tile, url);
		cache.set(tile.hash, imageTile);
	}

	return imageTile;
}
