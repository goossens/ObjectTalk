//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtLog.h"

#include "OtAssetFactory.h"
#include "OtTextureAsset.h"


//
//	Register texture types
//

static OtAssetFactoryRegister<OtTextureAsset> jpgRegistration{".jpg"};
static OtAssetFactoryRegister<OtTextureAsset> jpegRegistration{".jpeg"};
static OtAssetFactoryRegister<OtTextureAsset> hdrRegistration{".hdr"};
static OtAssetFactoryRegister<OtTextureAsset> pngRegistration{".png"};


//
//	OtTextureAsset::load
//

bool OtTextureAsset::load(const std::filesystem::path& path) {
	try {
		// try to load the texture
		texture.loadFromFile(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load texture [%s]: %s", path.string().c_str(), exception.what()));
		return false;
	}
}
