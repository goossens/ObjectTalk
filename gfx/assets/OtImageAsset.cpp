//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtLog.h"

#include "OtImageAsset.h"


//
//	OtImageAsset::load
//

OtAssetBase::State OtImageAsset::load() {
	try {
		// try to load the image
		image.load(path);
		return State::ready;

	} catch ([[maybe_unused]] const OtException& exception) {
		OtLogWarning("Can't load image [{}]: {}", path, exception.what());
		return State::invalid;
	}
}
