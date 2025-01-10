//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAsset.h"
#include "OtTextAsset.h"


//
//	OtNodesAsset
//

class OtNodesAsset : public OtTextAsset {
public:
	// asset properties
	static constexpr bool hasEditor = true;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".otn";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }
};
