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
	// load/save the asset
	bool load() override;
	bool save() override;

	// access the text
	inline std::string& getText() { return text; }
	inline void setText(const std::string& t) { text = t; }

protected:
	// the actual text
	std::string text;
};
