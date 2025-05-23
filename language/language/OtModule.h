//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>
#include <vector>

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
	void load(const std::string& path, const std::string& code);

	// get type definition
	static OtType getMeta();

	// import a module from disk or an internal generator
	static OtModule import(const std::string& path);

	// add a path to search for modules
	static void addPath(const std::string& path);

protected:
	// constructor
	OtModuleClass() = default;
	friend class OtObjectPointer<OtModuleClass>;

private:
	// clear the internal cache
	friend class OtFramework;
	static void clear();

	// list of directories to search for modules in
	static void buildModulePath();

	// determine full path name for module
	static std::string checkPath(const std::string& path);
	static std::string getFullPath(const std::string& path);

	// module search path
	inline static std::vector<std::string> modulePath;
	inline static std::vector<std::string> localPath;
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
