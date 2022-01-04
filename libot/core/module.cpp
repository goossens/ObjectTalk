//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <system_error>

#include "ot/exception.h"
#include "ot/registry.h"
#include "ot/singleton.h"
#include "ot/text.h"
#include "ot/source.h"
#include "ot/libuv.h"

#include "ot/string.h"
#include "ot/module.h"
#include "ot/bytecode.h"
#include "ot/compiler.h"


//
//	file extension for binary modules
//

#if defined(_WIN32)
const std::string libext = ".dll";

#else
const std::string libext = ".so";
#endif


//
//	Module search path
//

static std::vector<std::filesystem::path> modulePath;
static std::vector<std::filesystem::path> localPath;


//
//	OtModuleRegistry
//

class OtModuleRegistry : public OtSingleton<OtModuleRegistry>, public OtRegistry<OtModule> {
};


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

	// see if module exists without a path prepended yet an extension appended?
	} else if (std::filesystem::exists(path.replace_extension(".ot"))) {
		return std::filesystem::canonical(path.replace_extension(".ot"));

	} else if (std::filesystem::exists(path.replace_extension(libext))) {
		return std::filesystem::canonical(path.replace_extension(libext));

	} else {
		return std::filesystem::path();
	}
}


//
//	OtModuleClass::getFullPath
//

std::filesystem::path OtModuleClass::getFullPath(std::filesystem::path path) {
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
//	OtModuleClass::loadBinaryModule
//

void OtModuleClass::loadBinaryModule(std::filesystem::path path, OtModule module) {
#if defined(_WIN32)
	HMODULE lib = LoadLibrary((LPCSTR) path.string().c_str());

	if (!lib) {
		DWORD error = GetLastError();
		std::string message = std::system_category().message(error);
		OtExcept("Can't import module [%s], error [%s]", path.string().c_str(), message.c_str());
	}

	void (*init)(OtObject) = (void (*)(OtObject)) GetProcAddress(lib, "init");

#else
	void* lib = dlopen(path.string().c_str(), RTLD_LAZY | RTLD_GLOBAL);

	if (!lib) {
		OtExcept("Can't import module [%s], error [%s]", path.string().c_str(), dlerror());
	}

	void (*init)(OtObject) = (void (*)(OtObject)) dlsym(lib, "init");
#endif

	if (!init) {
		OtExcept("Module [%s] does not have an [init] function", path.string().c_str());
	}

	// initialize module
	(*init)(module);
}


//
//	OtModuleClass::loadSourceModule
//

void OtModuleClass::loadSourceModule(std::filesystem::path path, OtModule module) {
	// temporarily add module path to search path so relative imports are found
	localPath.push_back(path.parent_path());

	// load source code
	std::ifstream stream(path.string().c_str());
	std::stringstream buffer;
	buffer << stream.rdbuf();
	stream.close();

	// compile and run module code
	OtCompiler compiler;
	OtSource source = OtSourceClass::create(path.string(), buffer.str());
	OtByteCode bytecode = compiler.compileModule(source, module);
	OtVM::execute(bytecode);

	// restore search path
	localPath.pop_back();
}


//
//	OtModuleClass::getMeta
//

OtType OtModuleClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtModuleClass>("Module", OtInternalClass::getMeta());
	}

	return type;
}


//
//	OtModuleClass::create
//

OtModule OtModuleClass::create(const std::string& name) {
	// see if this is an "internal" module that was already registered
	if (OtModuleRegistry::instance().has(name)) {
		return OtModuleRegistry::instance().get(name);

	} else {
		// build module path (if required)
		if (modulePath.size() == 0) {
			buildModulePath();
		}

		// get full path to module
		std::filesystem::path fullPath = getFullPath(name);

		// ensure module exists
		if (fullPath.empty()) {
			OtExcept("Can't find module [%s]", name.c_str());
		}

		// see if module is already loaded
		if (OtModuleRegistry::instance().has(fullPath.string())) {
			return OtModuleRegistry::instance().get(fullPath.string());

		} else {
			// create a new module
			OtModule module = std::make_shared<OtModuleClass>();
			module->setType(getMeta());

			// add it to registry for reuse
			OtModuleRegistry::instance().set(fullPath.string(), module);

			// setup module's meta data
			module->set("__FILE__", OtStringClass::create(fullPath.string()));
			module->set("__DIR__", OtStringClass::create(fullPath.parent_path().string()));

			// handle different module types
			if (fullPath.extension() == libext) {
				loadBinaryModule(fullPath, module);

			} else {
				loadSourceModule(fullPath, module);
			}

			return module;
		}
	}
}


//
//	OtModuleClass::internal
//

OtModule OtModuleClass::internal(const std::string& name) {
	// create a new module
	OtModule module = std::make_shared<OtModuleClass>();
	module->setType(getMeta());

	// add it to registry for reuse
	OtModuleRegistry::instance().set(name, module);
	return module;
}
