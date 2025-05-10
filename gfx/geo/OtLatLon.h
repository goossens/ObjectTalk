//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	OtLatLon
//

class OtLatLon {
public:
	OtLatLon() : lat(0.0), lon(0.0) {}
	OtLatLon(double lt, double ln) : lat(lt), lon(ln) {}

	double lat;
	double lon;
};
