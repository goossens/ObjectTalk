//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtFormat.h"
#include "OtLog.h"

#include "OtAssetFactory.h"
#include "OtTextureAsset.h"


//
//	Register texture types
//

static OtAssetFactoryRegister<OtTextureAsset> jpgRegistration{".jpg"};
static OtAssetFactoryRegister<OtTextureAsset> jpegRegistration{".jpeg"};
static OtAssetFactoryRegister<OtTextureAsset> pngRegistration{".png"};
static OtAssetFactoryRegister<OtTextureAsset> hdrRegistration{".hdr"};


//
//	OtTextureAsset::load
//

bool OtTextureAsset::load() {
	try {
		// try to load the texture
		texture.loadFromFile(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load texture [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}
