//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtModule.h"

#include "OtTextAsset.h"


//
//	OtScriptAsset
//

class OtScriptAsset : public OtTextAsset {
public:
	// compile the script (text) into a module
	inline void compile() {
		module = OtModule::create();
		module->load(path);
	}

	// access the script module
	OtModule& getModule() { return module; }

	// asset properties
	static constexpr bool hasEditor = true;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".ot";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

private:
	// the compiled module
	OtModule module;
};
