//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAsset.h"
#include "OtAssetBase.h"


//
//	OtTextAsset
//

class OtTextAsset : public OtAssetBase {
public:
	// access the text
	inline std::string& getText() { return text; }
	inline void setText(const std::string& t) { text = t; }

	// asset properties
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".txt";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override;

	// the actual text
	std::string text;
};
