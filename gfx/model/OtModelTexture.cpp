//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "fmt/format.h"

#include "OtModelTexture.h"


//
//	OtModelTexture::load
//

void OtModelTexture::load(size_t modelID, size_t textureID, const aiTexture* texture) {
	// create a virtual asset for the texture
	asset = fmt::format("virtual:model{}.{}", modelID, textureID);

	// create texture from embedded image (loading is asynchronous as models are loaded in separate thread)
	if (texture->mHeight == 0) {
		// image is compressed
		asset->getTexture().load(texture->pcData, static_cast<size_t>(texture->mWidth), true);

	} else {
		// image is uncompressed
		asset->getTexture().load(
			static_cast<int>(texture->mWidth),
			static_cast<int>(texture->mHeight),
			OtTexture::rgba8Texture,
			texture->pcData,
			true);
	}

	// this is required to let the asset manager know the texture is not missing
	asset->setTextureReady();
}
