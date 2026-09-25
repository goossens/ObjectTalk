//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtException.h"

#include "OtAsset.h"
#include "OtAssetBase.h"


//
//	OtTextAsset
//

class OtTextAsset : public OtAssetBase {
public:
	// access the text
	inline const std::string& getText() { return text; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".txt";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override {
		try {
			// try to load the text
			OtText::load(path, text);
			postProcess();
			return State::ready;

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

	// postprocess text (if required)
	virtual void postProcess() {};

	// the actual text
	std::string text;
};
