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

void OtModuleClass::registerInternal(const std::string &name, std::function<void(OtModule)> creator)
{
	OtModuleRegistryEntry entry;
	entry.creator = creator;
	OtModuleRegistry::instance()->set(name, entry);
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
	if (std::filesystem::exists(path)) {
		return std::filesystem::canonical(path);

	// see if module exists without a path prepended and extension appended?
	} else if (std::filesystem::exists(path.replace_extension(".ot"))) {
		return std::filesystem::canonical(path.replace_extension(".ot"));

	} else {
		return std::filesystem::path();
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
		fullName = checkPath(localPath.back() / path);
	}

	return fullName;
}


//
//	OtModuleClass::load
//

void OtModuleClass::load(const std::string& filename) {
	// get full path of module (temporarily adding parent path to catch relative imports)
	localPath.push_back(std::filesystem::path(filename).parent_path());
	auto fullPath = getFullPath(filename);
	localPath.pop_back();

	// ensure module exists
	if (fullPath.empty()) {
		OtExcept("Can't find module [%s]", filename.c_str());
	}

	// start with a clean slate
	unsetAll();

	// add metadata to module
	set("__FILE__", OtStringClass::create(fullPath.string()));
	set("__DIR__", OtStringClass::create(fullPath.parent_path().string()));

	// load source code
	std::ifstream stream(fullPath.string().c_str());
	std::stringstream buffer;
	buffer << stream.rdbuf();
	stream.close();

	// compile and run module code
	OtCompiler compiler;
	OtSource source = OtSourceClass::create(fullPath.string(), buffer.str());
	OtByteCode bytecode = compiler.compileModule(source, cast<OtModuleClass>());
	OtVM::instance()->execute(bytecode);
}


//
//	OtModuleClass::getMeta
//

OtType OtModuleClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtModuleClass>("Module", OtInternalClass::getMeta());
	}

	return type;
}


//
//	OtModuleClass::create
//

OtModule OtModuleClass::create() {
	OtModule module = std::make_shared<OtModuleClass>();
	module->setType(getMeta());
	return module;
}


//
//	OtModuleClass::import
//

OtModule OtModuleClass::import(const std::string& filename) {
	// see if this is an "internal" module
	std::shared_ptr<OtModuleRegistry> registry = OtModuleRegistry::instance();

	if (registry->has(filename)) {
		OtModuleRegistryEntry& entry = registry->at(filename);

		// instantiate the module if required
		if (!entry.module) {
			entry.module = std::make_shared<OtModuleClass>();
			entry.module->setType(getMeta());
			entry.creator(entry.module);
		}

		// return the module
		return entry.module;

	} else {
		// create a new module and load it
		OtModule module = create();
		module->load(filename);
		return module;
	}
}
