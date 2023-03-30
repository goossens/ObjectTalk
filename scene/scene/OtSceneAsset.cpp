//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
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
#include "OtSceneAsset.h"


//
//	Register texture types
//

static OtAssetFactoryRegister<OtSceneAsset> otsRegistration{".ots"};


//
//	OtSceneAsset::load
//

bool OtSceneAsset::load(const std::filesystem::path &path) {
	try {
		// create and load the scene
		scene = std::make_unique<OtScene>();
		scene->load(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load scene [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}
