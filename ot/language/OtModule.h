//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <functional>

#include "OtInternal.h"


//
//	OtModule
//

class OtModuleClass;
using OtModule = OtObjectPointer<OtModuleClass>;

class OtModuleClass : public OtInternalClass {
public:
	// load the module
	void load(const std::filesystem::path& path);
	void load(const std::filesystem::path& root, const std::string& code);

	// get type definition
	static OtType getMeta();

	// import a module (either load from disk or get from the memory cash)
	static OtModule import(const std::string& path);

private:
	// list of directories to search for modules in
	static void buildModulePath();

	// determine full path name for module
	static std::filesystem::path checkPath(std::filesystem::path path);
	static std::filesystem::path getFullPath(std::filesystem::path path);
};


//
//	OtModuleRegister
//

class OtModuleRegister {
public:
	// constructors
	OtModuleRegister() = delete;
	OtModuleRegister(const char* name, std::function<void(OtModule)> creator);
	OtModuleRegister(const OtModuleRegister&) = delete; // no copy constructor
	OtModuleRegister& operator=(const OtModuleRegister&) = delete; // no copy assignment
	OtModuleRegister(OtModuleRegister&&) = delete;
	OtModuleRegister& operator=(OtModuleRegister&&) = delete;
};
