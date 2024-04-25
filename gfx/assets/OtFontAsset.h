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
#include "OtFont.h"


//
//	OtFontAsset
//

class OtFontAsset : public OtAssetBase {
public:
	// access the font
	inline OtFont& getFont() { return font; }
	inline void setFont(OtFont& f) { font = f; assetState = readyState; }
	inline void clearFont() { font.clear(); assetState = missingState; }

	// asset properties
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".ttf";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	AssetState load() override;

private:
	// the actual font
	OtFont font;
};
