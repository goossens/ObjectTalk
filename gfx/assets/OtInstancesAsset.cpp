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

#include "OtInstancesAsset.h"


//
//	OtInstancesAsset::load
//

OtAssetBase::State OtInstancesAsset::load() {
	try {
		// try to load the instances
		instances.load(path);
		return State::ready;

	} catch ([[maybe_unused]] const OtException& exception) {
		errorMessage = exception.what();
		return State::invalid;
	}
}
