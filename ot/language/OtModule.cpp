//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>
#include <fstream>
#include <functional>
#include <sstream>
#include <system_error>

#include "OtByteCode.h"
#include "OtCompiler.h"
#include "OtException.h"
#include "OtLibuv.h"
#include "OtModule.h"
#include "OtRegistry.h"
#include "OtSingleton.h"
#include "OtSource.h"
#include "OtString.h"
#include "OtText.h"


//
//	OtModuleRegistry
//

struct OtModuleRegistryEntry {
	std::function<void(OtModule)> creator;
	OtModule module;
};

class OtModuleRegistry : public OtSingleton<OtModuleRegistry>, public OtRegistry<OtModuleRegistryEntry> {
};


//
//	Module search path
//

static std::vector<std::filesystem::path> modulePath;
static std::vector<std::filesystem::path> localPath;


//
//	OtModuleClass::registerInternal
//

void OtModuleClass::registerInternal(const std::string& name, std::function<void(OtModule)> creator)
{
	OtModuleRegistryEntry entry;
	entry.creator = creator;
	OtModuleRegistry::instance()->set(name, entry);
}


//
//	OtModuleClass::load
//

void OtModuleClass::load(const std::filesystem::path& path) {
	// get full path of module
	auto fullPath = getFullPath(path);

	// ensure module exists
	if (fullPath.empty()) {
		OtError("Can't find module [%s]", path.string().c_str());
	}

	// start with a clean slate
	unsetAll();

	// add metadata to module
	set("__FILE__", OtString::create(fullPath.string()));
	set("__DIR__", OtString::create(fullPath.parent_path().string()));

	// load source code
	std::ifstream stream(fullPath.string().c_str());
	std::stringstream buffer;
	buffer << stream.rdbuf();
	stream.close();

	// compile and run module code
	OtCompiler compiler;
	OtSource source = OtSourceClass::create(fullPath.string(), buffer.str());

	localPath.push_back(fullPath.parent_path());

	try {
		auto bytecode = compiler.compileSource(source, OtModule(this));
		OtVM::instance()->execute(bytecode);

	} catch (const OtException& e) {
		localPath.pop_back();
		throw e;
	}

	localPath.pop_back();
}


//
//	OtModuleClass::buildModulePath
//

void OtModuleClass::buildModulePath() {
	// use local directory as default
	modulePath.push_back(std::filesystem::canonical("."));

	// use OT_PATH environment variable if provided
	auto path = std::getenv("OT_PATH");

	if (path) {
		std::vector<std::string> parts;
		OtText::split(path, parts,';');

		for (auto part = parts.begin(); part != parts.end(); ++part) {
			modulePath.push_back(*part);
		}
	}

	// figure out where we live
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_exepath(buffer, &length);
	UV_CHECK_ERROR("uv_exepath", status);
	std::string home(buffer, length);

	// use lib directory
	auto exec = std::filesystem::path(home);
	auto root = std::filesystem::canonical(exec).parent_path().parent_path();
	auto lib = root / "lib" / "ot";

	if (std::filesystem::is_directory(lib)) {
		modulePath.push_back(lib);
	}
}


//
//	OtModuleClass::checkPath
//

std::filesystem::path OtModuleClass::checkPath(std::filesystem::path path) {
	// see if path points to a regular file
	if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
		return std::filesystem::canonical(path);

	// see if module exists without a path prepended and extension appended?
	} else {
		auto pathWithExt = path.replace_extension(".ot");

		if (std::filesystem::exists(pathWithExt) && std::filesystem::is_regular_file(pathWithExt)) {
			return std::filesystem::canonical(pathWithExt);

		} else {
			return std::filesystem::path();
		}
	}
}


//
//	OtModuleClass::getFullPath
//

std::filesystem::path OtModuleClass::getFullPath(std::filesystem::path path) {
	// build module path (if required)
	if (modulePath.size() == 0) {
		buildModulePath();
	}

	// see if name addresses a module
	std::filesystem::path fullName = checkPath(path);

	// find module on path (if still required)
	for (size_t i = 0; i < modulePath.size() && fullName.empty(); i++) {
		fullName = checkPath(modulePath[i] / path);
	}

	// find module in local path (if still required)
	if (fullName.empty() && localPath.size()) {
		for (int i = (int) localPath.size() - 1; i >= 0 && fullName.empty(); i--) {
			fullName = checkPath(localPath[i] / path);
		}
	}

	return fullName;
}


//
//	OtModuleClass::getMeta
//

OtType OtModuleClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtModuleClass>("Module", OtInternalClass::getMeta());
	}

	return type;
}


//
//	OtModuleClass::import
//

OtModule OtModuleClass::import(const std::string& name) {
	// see if this is an "internal" module
	auto registry = OtModuleRegistry::instance();

	if (registry->has(name)) {
		OtModuleRegistryEntry& entry = registry->at(name);

		// instantiate the module if required
		if (!entry.module) {
			entry.module = OtModule::create();
			entry.creator(entry.module);
		}

		// return the module
		return entry.module;

	} else {
		// create a new module and load it
		OtModule module = OtModule::create();
		module->load(name);
		return module;
	}
}
