//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "imgui.h"

#include "OtLatLon.h"
#include "OtMapRaster.h"
#include "OtMapTile.h"


//
//	OtProjector
//
//	The OtProjector class converts coordinates between various coordinate
//	systems. Below is a description of the coordinate types:
//
//	LatLon -	Latitude and longitude in decimal degrees using the WGS84
//				datum (EPSG:4326).
//
//	Tile -		Tile coordinates for each zoom level using the top-left
//				as the origin.
//
//	Raster -	Normalized screen raster coordinates using the intersection
//				between the Prime Meridian and the Equator as the origin with
//				positive numbers going east and north. Horizontal and
//				vertical coordinate are between -1.0 and 1.0;
//
//	Screen -	Screen coordinates used for rendering
//

class OtProjector {
public:
	// access properties
	inline void setWindowSize(ImVec2 s) { size = s; calculate(); }
	inline ImVec2 getWindowSize() { return size; }
	inline void setWindowOffset(ImVec2 o) { offset = o; calculate(); }
	inline ImVec2 getWindowOffset() { return offset; }

	inline void setCenter(OtLatLon c) { center = c; calculate(); }
	inline OtLatLon getCenter() { return center; }

	inline void setResolution(double r) { resolution = r; calculate(); }
	inline double getResolution() { return resolution; }
	inline int getZoom() { return zoom; }

	// animate center to specified position and resolution
	void flyTo(OtLatLon center, double resolution);

	// convert coordinates
	OtMapRaster latLonToRaster(OtLatLon ll);
	OtLatLon rasterToLatLon(OtMapRaster raster);
	OtMapTile rasterToTile(OtMapRaster raster, int zoom);
	OtLatLon screenToLatLon(ImVec2 screen);

	// get visible extends
	void visibleRasterBoundaries(OtMapRaster& minR, OtMapRaster& maxR);
	void tileScreenBoundaries(OtMapTile& tile, ImVec2& minS, ImVec2& maxS);

private:
	// calculate derived values
	void calculate();

	// properties
	int tileSize = 256; // in pixels
	ImVec2 size; // screen size
	ImVec2 offset; // screen offset

	OtLatLon center;
	double resolution;

	// derived values
	int zoom = 0;
	OtMapRaster minR, maxR;
};
