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
#include "OtImage.h"


//
//	OtImageAsset
//

class OtImageAsset : public OtAssetBase {
public:
	// access the image
	inline OtImage& getImage() { return image; }
	inline void setImage(OtImage& i) { image = i; }
	inline void clearImage() { image.clear(); }

	// asset properties
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".jpg,.jpeg,.png,.hdr";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	AssetState load() override;

private:
	// the actual image
	OtImage image;
};