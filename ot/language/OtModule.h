//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>

#include "OtInternal.h"


//
//	OtModule
//

class OtModuleClass;
using OtModule = OtObjectPointer<OtModuleClass>;

class OtModuleClass : public OtInternalClass {
public:
	// load module from disk or provided source code
	void load(const std::string& path);
	void load(const std::string& root, const std::string& code);

	// get type definition
	static OtType getMeta();

	// import a module from disk, our cache or from an internal generator
	static OtModule import(const std::string& path);

private:
	// list of directories to search for modules in
	static void buildModulePath();

	// determine full path name for module
	static std::string checkPath(const std::string& path);
	static std::string getFullPath(const std::string& path);
};


//
//	OtModuleRegistration (for internal modules)
//

class OtModuleRegistration {
public:
	// constructors
	OtModuleRegistration() = delete;
	OtModuleRegistration(const char* name, std::function<void(OtModule)> creator);
	OtModuleRegistration(const OtModuleRegistration&) = delete; // no copy constructor
	OtModuleRegistration& operator=(const OtModuleRegistration&) = delete; // no copy assignment
	OtModuleRegistration(OtModuleRegistration&&) = delete;
	OtModuleRegistration& operator=(OtModuleRegistration&&) = delete;
};
