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

#include "OtGeometryAsset.h"


//
//	OtGeometryAsset::load
//

OtAssetBase::State OtGeometryAsset::load() {
	try {
		// try to load the geometry
		geometry.load(path);
		return State::ready;

	} catch (const OtException& exception) {
		OtLogWarning("Can't load geometry [{}]: {}", path, exception.what());
		return State::invalid;
	}
}
