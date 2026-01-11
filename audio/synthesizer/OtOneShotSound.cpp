//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "nlohmann/json.hpp"

#include "OtOneShotSound.h"


//
//	OtOneShotSound::isReady
//

bool OtOneShotSound::isReady() {
	// handle status changes for sound file
	if (sound.isValid() && (asset.isNull() || asset.isMissing() || asset.isInvalid())) {
		sound.clear();

	} else if (asset.isReady() && asset->getOneShotFile() != sound) {
		sound = asset->getOneShotFile();
	}

	// return current status
	return asset.isReady();
}


//
//	OtOneShotSound::renderSound
//

bool OtOneShotSound::renderSound() {
	bool changed = false;

	if (asset.renderUI("Sound")) {
		if (asset.isNull()) {
			sound.clear();

		} else if (asset.isReady()) {
			sound = asset->getOneShotFile();

		} else {
			sound.clear();
		}

		changed |= true;
	}

	return changed;
}


//
//	OtOneShotSound::serialize
//

void OtOneShotSound::serialize(nlohmann::json* data, std::string* basedir) {
	OtSynthSound::serialize(data, basedir);
	(*data)["soundFile"] = OtAssetSerialize(asset.getPath(), basedir);
}


//
//	OtOneShotSound::deserialize
//

void OtOneShotSound::deserialize(nlohmann::json* data, std::string* basedir) {
	OtSynthSound::deserialize(data, basedir);
	asset = OtAssetDeserialize(data, "soundFile", basedir);
}
