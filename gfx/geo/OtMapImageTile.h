//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//  Include files
//

#include "imgui.h"

#include "OtAsset.h"
#include "OtTextureAsset.h"

#include "OtMapTile.h"
#include "OtProjector.h"


//
//  OtMapImageTile
//

class OtMapImageTile {
public:
	// constructor
	OtMapImageTile(OtMapTile t, const std::string& url) : tile(t) {
		texture = url;
	}

	// render tile
	void render(ImDrawList* drawlist, OtProjector& projector);

	// opaque means that the image is loaded and ready to be rendered
	inline bool isOpaque() { return texture.isReady(); }

private:
	OtMapTile tile;
	OtAsset<OtTextureAsset> texture;
};
