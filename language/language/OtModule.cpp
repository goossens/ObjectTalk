//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdlib>
#include <fstream>
#include <sstream>

#include "OtByteCode.h"
#include "OtCompiler.h"
#include "OtException.h"
#include "OtModule.h"
#include "OtPathObject.h"
#include "OtPath.h"
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
	set("__FILE__", OtPathObject::create(path));
	set("__DIR__", OtPathObject::create(OtPath::getParent(path)));

	// compile and run module code
	OtCompiler compiler;
	OtSource source = OtSourceClass::create(path, code);

	localPath.push_back(OtPath::getParent(path));

	try {
		auto bytecode = compiler.compileSource(source, OtModule(this));
		OtVM::execute(bytecode);

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
	// use current directory as default
	modulePath.push_back(OtPath::getCWD());

	// use OT_PATH environment variable if provided
	auto path = std::getenv("OT_PATH");

	if (path) {
		std::vector<std::string> parts;
		OtText::split(path, parts, ';');

		for (auto part = parts.begin(); part != parts.end(); ++part) {
			modulePath.push_back(OtPath::getCanonical(*part));
		}
	}

	// use lib directory
	auto root = OtPath::getParent(OtPath::getParent(OtPath::getExecutable()));
	auto lib = OtPath::join(OtPath::join(root, "lib"), "ot");

	if (OtPath::isDirectory(lib)) {
		modulePath.push_back(lib);
	}
}


//
//	OtModuleClass::checkPath
//

std::string OtModuleClass::checkPath(const std::string& path) {
	// see if path points to a regular file
	if (OtPath::isRegularFile(path)) {
		return OtPath::getCanonical(path);

	// see if module exists with an extension appended?
	} else {
		auto pathWithExt = OtPath::replaceExtension(path, ".ot");

		if (OtPath::isRegularFile(pathWithExt)) {
			return OtPath::getCanonical(pathWithExt);

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

	// find module on paths (if still required)
	for (size_t i = 0; i < modulePath.size() && fullName.empty(); i++) {
		fullName = checkPath(OtPath::join(modulePath[i], path));
	}

	// find module in local paths (if still required)
	if (fullName.empty() && localPath.size()) {
		for (int i = (int) localPath.size() - 1; i >= 0 && fullName.empty(); i--) {
			fullName = checkPath(OtPath::join(localPath[i], path));
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
	auto& registry = OtModuleRegistry::instance();

	if (registry.has(name)) {
		OtModuleRegistryEntry& entry = registry.at(name);

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
//	OtModuleClass::addPath
//

void OtModuleClass::addPath(const std::string& path) {
	// build module path (if required)
	if (modulePath.size() == 0) {
		buildModulePath();
	}

	// add the path
	modulePath.push_back(path);
}


//
//	OtModuleClass::clear
//

void OtModuleClass::clear() {
	// clear the module cache
	OtModuleRegistry::instance().iterateValues([](OtModuleRegistryEntry& entry) {
		entry.module = nullptr;
	});
}


//
//	OtModuleRegistration::OtModuleRegistration
//

OtModuleRegistration::OtModuleRegistration(const char* name, std::function<void(OtModule)> creator) {
	OtModuleRegistryEntry entry;
	entry.creator = creator;
	OtModuleRegistry::instance().set(name, entry);
}
