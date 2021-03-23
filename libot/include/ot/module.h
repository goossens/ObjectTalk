//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "context.h"


//
//	OtModule
//

class OtModuleClass;
typedef std::shared_ptr<OtModuleClass> OtModule;

class OtModuleClass : public OtContextClass {
public:
	// import module
	void import(const std::string& name);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtModule create();

private:
	// list of directories to search for modules in
	std::vector<std::filesystem::path> modulePath;
};
