//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
#include "OtPathTools.h"
#include "OtRegistry.h"
#include "OtSingleton.h"
#include "OtSource.h"
#include "OtString.h"
#include "OtText.h"
#include "OtVM.h"


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

static std::vector<std::string> modulePath;
static std::vector<std::string> localPath;


//
//	OtModuleClass::load
//

void OtModuleClass::load(const std::string& path) {
	// get full path of module
	auto fullPath = getFullPath(path);

	// ensure module exists
	if (fullPath.empty()) {
		OtError("Can't find module [{}]", path);
	}

	// load source code
	std::ifstream stream(fullPath.c_str());
	std::stringstream buffer;
	buffer << stream.rdbuf();
	stream.close();

	// compile and run module code
	load(fullPath, buffer.str());
}


//
//	OtModuleClass::load
//

void OtModuleClass::load(const std::string& path, const std::string& code) {
	// start with a clean slate
	unsetAll();

	// add metadata to module
	set("__FILE__", OtString::create(path));
	set("__DIR__", OtString::create(OtPathGetParent(path)));

	// compile and run module code
	OtCompiler compiler;
	OtSource source = OtSourceClass::create(path, code);

	localPath.push_back(OtPathGetParent(path));

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
	modulePath.push_back(".");

	// use OT_PATH environment variable if provided
	auto path = std::getenv("OT_PATH");

	if (path) {
		std::vector<std::string> parts;
		OtText::split(path, parts, ';');

		for (auto part = parts.begin(); part != parts.end(); ++part) {
			modulePath.push_back(*part);
		}
	}

	// figure out where we live
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_exepath(buffer, &length);
	UV_CHECK_ERROR("uv_exepath", status);
	std::string exec(buffer, length);

	// use lib directory
	auto root = OtPathGetParent(OtPathGetParent(exec));
	auto lib = OtPathJoin(OtPathJoin(root, "lib"), "ot");

	if (OtPathIsDirectory(lib)) {
		modulePath.push_back(lib);
	}
}


//
//	OtModuleClass::checkPath
//

std::string OtModuleClass::checkPath(const std::string& path) {
	// see if path points to a regular file
	if (OtPathIsRegularFile(path)) {
		return path;

	// see if module exists without a path prepended and extension appended?
	} else {
		auto pathWithExt = OtPathReplaceExtension(path, ".ot");

		if (OtPathIsRegularFile(pathWithExt)) {
			return pathWithExt;

		} else {
			return std::string();
		}
	}
}


//
//	OtModuleClass::getFullPath
//

std::string OtModuleClass::getFullPath(const std::string& path) {
	// build module path (if required)
	if (modulePath.size() == 0) {
		buildModulePath();
	}

	// see if name addresses a module
	std::string fullName = checkPath(path);

	// find module on path (if still required)
	for (size_t i = 0; i < modulePath.size() && fullName.empty(); i++) {
		fullName = checkPath(OtPathJoin(modulePath[i], path));
	}

	// find module in local path (if still required)
	if (fullName.empty() && localPath.size()) {
		for (int i = (int) localPath.size() - 1; i >= 0 && fullName.empty(); i--) {
			fullName = checkPath(OtPathJoin(localPath[i], path));
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


//
//	OtModuleRegistration::OtModuleRegistration
//

OtModuleRegistration::OtModuleRegistration(const char* name, std::function<void(OtModule)> creator) {
	OtModuleRegistryEntry entry;
	entry.creator = creator;
	OtModuleRegistry::instance()->set(name, entry);
}
