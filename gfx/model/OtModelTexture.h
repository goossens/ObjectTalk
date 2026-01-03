//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "assimp/texture.h"

#include "OtAsset.h"
#include "OtTextureAsset.h"


//
//	OtModelTexture
//

class OtModelTexture {
public:
	// load the texture
	void load(size_t modelID, size_t textureID, const aiTexture* texture);

private:
	friend class OtModel;

	// properties
	OtAsset<OtTextureAsset> asset;
};
