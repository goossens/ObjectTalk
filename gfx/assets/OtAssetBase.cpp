//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtAssetBase.h"
#include "OtAssetManager.h"


//
//	OtAssetBase::saveAs
//

bool OtAssetBase::saveAs(const std::string &path) {
	OtAssetManager::instance()->renameAsset(path, assetPath);
	assetPath = path;
	save();
}
