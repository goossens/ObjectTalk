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

#include "OtFontAsset.h"


//
//	OtFontAsset::load
//

OtAssetBase::State OtFontAsset::load() {
	try {
		// try to load the font
		font.load(path);
		return State::ready;

	} catch (const OtException& exception) {
		OtLogWarning("Can't load font [{}]: {}", path, exception.what());
		return State::invalid;
	}
}
