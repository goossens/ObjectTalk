//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <fstream>

#include "OtException.h"
#include "OtLog.h"

#include "OtTextAsset.h"


//
//	OtTextAsset::load
//

OtAssetBase::AssetState OtTextAsset::load() {
	try {
		// try to load the text
		std::ifstream stream(path.c_str());

		stream.seekg(0, std::ios::end);
		text.reserve(stream.tellg());
		stream.seekg(0, std::ios::beg);

		text.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		stream.close();

		return readyState;

	} catch (const OtException& exception) {
		OtLogWarning("Can't load text from [{}]: {}", path, exception.what());
		return invalidState;
	}
}


//
//	OtTextAsset::save
//

OtAssetBase::AssetState OtTextAsset::save() {
	try {
		// try to save the text
		std::ofstream stream(path.c_str());
		stream << text;
		stream.close();
		return readyState;

	} catch (const OtException& exception) {
		OtLogWarning("Can't save text to [{}]: {}", path, exception.what());
		return invalidState;
	}
}
