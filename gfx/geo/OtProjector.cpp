//	Another Wonderful Service-Oriented Multitouch Environment (AWESOME)
//	Copyright (C) 2010-2017 Johan A. Goossens
//
//	AWESOME is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	AWESOME is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with AWESOME. If not, see <http:#www.gnu.org/licenses/>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "glm/glm.hpp"

#include "OtNumbers.h"

#include "OtProjector.h"


//
//	OtProjector::flyTo
//

void OtProjector::flyTo(OtLatLon c, double r) {
//	MtAnimationAdd(&center.lat, 0, 0.5, center.lat, c.lat);
//	MtAnimationAdd(&center.lon, 0, 0.5, center.lon, c.lon);
//	MtAnimationAdd(&resolution, 0, 0.5, resolution, r);

	center = c;
	resolution = r;
	calculate();
}


//
//	OtProjector::latLonToRaster
//

OtMapRaster OtProjector::latLonToRaster(OtLatLon ll) {
	// convert latlon to the Web Mercator projection
	// see https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
	auto x = glm::radians(ll.lon);
	auto y = std::asinh(std::tan(glm::radians(ll.lat)));

	// project on unit square
	return OtMapRaster(x / std::numbers::pi, y / std::numbers::pi);
}


//
//	OtProjector::rasterToLatLon
//

OtLatLon OtProjector::rasterToLatLon(OtMapRaster raster) {
	// convert raster coordinate to the Web Mercator projection
	auto x = raster.x * std::numbers::pi;
	auto y = raster.y * std::numbers::pi;

	// convert to latlon
	return OtLatLon(glm::degrees(std::atan(std::sinh(y))), glm::degrees(x));
}


//
//	OtProjector::rasterToTile
//

OtMapTile OtProjector::rasterToTile(OtMapRaster raster, int zm) {
	auto tilesAcross = 1 << zm;
	auto x = static_cast<int>(std::floor(((raster.x + 1.0) * 0.5) * static_cast<double>(tilesAcross)));
	auto y = static_cast<int>(std::floor(((raster.y + 1.0) * 0.5) * static_cast<double>(tilesAcross)));
	return OtMapTile(zm, std::clamp(x, 0, tilesAcross - 1), std::clamp(y, 0, tilesAcross - 1));
}


//
//	OtProjector::screenToLatLon
//

OtLatLon OtProjector::screenToLatLon(ImVec2 screen) {
	auto screenOffset = screen - offset - ImVec2(size.x / 2.0f, size.y / 2.0f);
	OtMapRaster rasterOffset(static_cast<double>(screenOffset.x) * resolution, static_cast<double>(screenOffset.y) * resolution);
	auto rasterCenter = latLonToRaster(center);
	return rasterToLatLon(rasterCenter + rasterOffset);
}


//
//	OtProjector::visibleRasterBoundaries
//

void OtProjector::visibleRasterBoundaries(OtMapRaster& min, OtMapRaster& max) {
	min = minR;
	max = maxR;
}


//
//	OtProjector::tileScreenBoundaries
//

void OtProjector::tileScreenBoundaries(OtMapTile& tile, ImVec2& minS, ImVec2& maxS) {
	// determine boundaries in raster coordinates
	auto tileCount = static_cast<double>(1 << tile.zoom);
	auto tileWidth = 2.0 / tileCount;
	OtMapRaster minTR(tile.x / tileCount * 2.0 - 1.0, tile.y / tileCount * 2.0 - 1.0);
	OtMapRaster maxTR(minTR.x + tileWidth, minTR.y + tileWidth);

	// convert to screen coordinates
	auto rasterCenter = latLonToRaster(center);
	auto minRasterOffset = minTR - rasterCenter;
	auto maxRasterOffset = maxTR - rasterCenter;
	ImVec2 minScreenOffset(static_cast<float>(minRasterOffset.x / resolution), static_cast<float>(minRasterOffset.y / resolution));
	ImVec2 maxScreenOffset(static_cast<float>(maxRasterOffset.x / resolution), static_cast<float>(maxRasterOffset.y / resolution));

	auto screenCenter = offset + ImVec2(size.x / 2.0f, size.y / 2.0f);
	minS = screenCenter + minScreenOffset;
	maxS = screenCenter + maxScreenOffset;
}


//
//	OtProjector::calculate
//

void OtProjector::calculate() {
	// determine zoom level
	zoom = 0;

	for (auto c = 1; c < 20 && zoom == 0; c++) {
		if (resolution > (2.0 / static_cast<double>((1 << c) * tileSize))) {
			zoom = c - 1;
		}
	}

	// max zoom for OSM is 18, VE = 19
	zoom = std::clamp(zoom, 1, 18);

	// determine edges of visible map portion
	auto centerR = latLonToRaster(center);
	auto dx2 = resolution * static_cast<double>(size.x) / 2.0;
	auto dy2 = resolution * static_cast<double>(size.y) / 2.0;

	minR = OtMapRaster(
		std::clamp(centerR.x - dx2, -1.0, 1.0),
		std::clamp(centerR.y - dy2, -1.0, 1.0));

	maxR = OtMapRaster(
		std::clamp(centerR.x + dx2, -1.0, 1.0),
		std::clamp(centerR.y + dy2, -1.0, 1.0));
}
