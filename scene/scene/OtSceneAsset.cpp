//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//


#include "OtException.h"
#include "OtLog.h"

#include "OtSceneAsset.h"


//
//	OtSceneAsset::OtSceneAsset
//

OtSceneAsset::OtSceneAsset() {
	// create the scene
	scene = std::make_unique<OtScene>();
}


//
//	OtSceneAsset::load
//

OtAssetBase::State OtSceneAsset::load() {
	try {
		// load the scene
		scene->load(path);
		return State::ready;

	} catch ([[maybe_unused]] const OtException& exception) {
		OtLogWarning("Can't load scene [{}]: {}", path, exception.what());
		return State::invalid;
	}
}
