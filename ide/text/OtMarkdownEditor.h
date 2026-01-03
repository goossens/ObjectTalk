//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtTextEditor.h"


//
//	OtMarkdownEditor
//

class OtMarkdownEditor : public OtTextEditor {
public:
	// constructor
	OtMarkdownEditor();

	// get editor status
	inline bool isRunnable() override { return false; }
	inline std::string getExtension() override { return extension; }
	static constexpr const char* extension = ".md";
};
