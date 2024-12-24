//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtException.h"
#include "OtLog.h"

#include "OtShapeAsset.h"


//
//	OtShapeAsset::load
//

OtAssetBase::State OtShapeAsset::load() {
	try {
		// try to load the shape
		shape.load(path);
		return State::ready;

	} catch (const OtException& exception) {
		OtLogWarning("Can't load shape [{}]: {}", path, exception.what());
		return State::invalid;
	}
}
