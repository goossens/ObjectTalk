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

bool OtSceneAsset::load() {
	try {
		// load the scene
		scene->load(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load scene [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}


//
//	OtSceneAsset::save
//

bool OtSceneAsset::save() {
	try {
		// save the scene
		scene->save(path);
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't save scene [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}
