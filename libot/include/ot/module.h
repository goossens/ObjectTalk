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
	static void buildModulePath();

	// determine full path name for module
	static std::filesystem::path checkPath(std::filesystem::path path);
	static std::filesystem::path getFullPath(std::filesystem::path path);

	// actually load the module (by type)
	static void loadBinaryModule(std::filesystem::path path, OtModule module);
	static void loadSourceModule(std::filesystem::path path, OtModule module);
};


//
//	Deal with Windows linking nonsense
//

#if defined(_WIN32)
#define OT_MODULE extern "C" __declspec(dllexport)
#else
#define OT_MODULE extern "C"
#endif
