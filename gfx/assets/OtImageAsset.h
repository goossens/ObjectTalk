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
#include "OtImage.h"


//
//	OtImageAsset
//

class OtImageAsset : public OtAssetBase {
public:
	// access the image
	inline OtImage& getImage() { return image; }
	inline void setImage(OtImage& i) { image = i; state = State::ready; }
	inline void clearImage() { image.clear(); state = State::missing; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".jpg,.jpeg,.png,.hdr,.exr";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override;

private:
	// the actual image
	OtImage image;
};
