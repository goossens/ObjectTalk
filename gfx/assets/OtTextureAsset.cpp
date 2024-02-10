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

#include "OtTextureAsset.h"


//
//	OtTextureAsset::load
//

OtAssetBase::AssetState OtTextureAsset::load() {
	try {
		// try to load the texture
		texture.loadFromFile(path);
		return readyState;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load texture [%s]: %s", path.c_str(), exception.what()));
		return invalidState;
	}
}
