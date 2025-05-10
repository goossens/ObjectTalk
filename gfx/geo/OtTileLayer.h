//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//  Include files
//

#include <memory>
#include <string>

#include "OtLruCache.h"

#include "OtMapLayer.h"
#include "OtMapImageTile.h"
#include "OtProjector.h"


//
//  OtTileLayer class
//

class OtTileLayer : public OtMapLayer {
public:
	// constructor
	OtTileLayer(const std::string& h) : host(h) {}

	// render the layer
	void render(ImDrawList* drawlist, OtProjector& projector) override;

private:
	// properties
	std::string host;

	// private functions to manage tile cache
	std::shared_ptr<OtMapImageTile> getTile(const OtMapTile& tile);
	OtLruCache<size_t, std::shared_ptr<OtMapImageTile>, 64> cache;
};
