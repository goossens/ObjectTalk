//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "fmt/format.h"

#include "OtMapImageTile.h"


//
//	OtMapImageTile::render
//

void OtMapImageTile::render(ImDrawList* drawlist, OtProjector& projector) {
	ImVec2 tl, br;
	projector.tileScreenBoundaries(tile, tl, br);

	if (texture->isReady()) {
		drawlist->AddImage(texture->getTexture().getTextureID(), tl, br);
	}
}
