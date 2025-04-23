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

#include "OtModelAsset.h"


//
//	OtModelAsset::load
//

OtAssetBase::State OtModelAsset::load() {
	try {
		// try to load the model
		model.load(path);
		return State::ready;

	} catch (const OtException& exception) {
		errorMessage = exception.what();
		return State::invalid;
	}
}
