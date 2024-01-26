//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	// load the asset
	bool load() override;

	// access the texture
	inline OtTexture& getTexture() { return texture; }
	inline void setTexture(OtTexture& t) { texture = t; }

	// can the asset handle virtual paths?
	static constexpr bool canHandleVirtual = true;

private:
	// the actual texture
	OtTexture texture;
};
