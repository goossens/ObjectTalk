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

#include "internal.h"


//
//	OtModule
//

class OtModuleClass;
typedef std::shared_ptr<OtModuleClass> OtModule;

class OtModuleClass : public OtInternalClass {
public:
	// get type definition
	static OtType getMeta();

	// create a new object
	static OtModule create(const std::string& name);

private:
	// list of directories to search for modules in
	static std::vector<std::filesystem::path> modulePath;
	static std::vector<std::filesystem::path> localPath;
	static void buildModulePath();

	// determine full path name for module
	static std::filesystem::path checkPath(const std::string& name);
	static std::filesystem::path getFullPath(const std::string& name);

	// actually load the module (by type)
	static void loadBinaryModule(std::filesystem::path path, OtModule module);
	static void loadSourceModule(std::filesystem::path path, OtModule module);
};
