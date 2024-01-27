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
#include "OtFormat.h"
#include "OtLog.h"

#include "OtTextAsset.h"


//
//	OtTextAsset::load
//

bool OtTextAsset::load() {
	try {
		// try to load the text
		std::ifstream stream(path.c_str());

		stream.seekg(0, std::ios::end);
		text.reserve(stream.tellg());
		stream.seekg(0, std::ios::beg);

		text.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
		stream.close();

		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't load text from [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}


//
//	OtTextAsset::save
//

bool OtTextAsset::save() {
	try {
		// try to save the text
		std::ofstream stream(path.c_str());
		stream << text;
		stream.close();
		return true;

	} catch (const OtException& exception) {
		OtLogWarning(OtFormat("Can't save text to [%s]: %s", path.c_str(), exception.what()));
		return false;
	}
}
