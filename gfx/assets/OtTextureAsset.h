//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAsset.h"
#include "OtAssetBase.h"
#include "OtTexture.h"


//
//	OtTextureAsset
//

class OtTextureAsset : public OtAssetBase {
public:
	// access the texture
	inline OtTexture& getTexture() { return texture; }
	inline void setTexture(OtTexture& t) { texture = t; state = State::ready; }
	inline void clearTexture() { texture.clear(); state = State::missing; }

	// asset properties
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".jpg,.jpeg,.png,.hdr";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override;

private:
	// the actual texture
	OtTexture texture;
};
