//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtMapRaster
//

class OtMapRaster {
public:
	OtMapRaster() : x(0.0), y(0.0) {}
	OtMapRaster(double _x, double _y) : x(_x), y(_y) {}

	OtMapRaster operator+(const OtMapRaster& rh) { return OtMapRaster(x + rh.x, y + rh.y); }
	OtMapRaster operator-(const OtMapRaster& rh) { return OtMapRaster(x - rh.x, y - rh.y); }

	double x;
	double y;
};
