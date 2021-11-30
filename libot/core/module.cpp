//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#if defined(_WIN32)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>

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
//	OtModuleRegistry
//

class OtModuleRegistry : public OtSingleton<OtModuleRegistry>, public OtRegistry<OtModule> {
};


//
//	OtModuleClass::import
//

void OtModuleClass::import(const std::string& name) {
	// build module path (if required)
	if (modulePath.size() == 0) {
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
		char buffer[PATH_MAX];
		size_t length = PATH_MAX;
		auto status = uv_exepath(buffer, &length);
		UV_CHECK_ERROR("uv_exepath", status);
		std::string home(buffer, length);

		// use lib directory
		auto exec = std::filesystem::path(home);
		auto root = std::filesystem::canonical(exec).parent_path().parent_path();
		auto lib = root.append("lib").append("ot");

		if (std::filesystem::is_directory(lib)) {
			modulePath.push_back(lib);
		}
	}

	std::filesystem::path pathName = std::filesystem::path(name);
	std::filesystem::path module;

#if defined(_WIN32)
	std::string libext = ".dll";

#else
	std::string libext = ".so";
#endif

	// see if the module exists without a path prepended?
	if (std::filesystem::exists(pathName)) {
		module = pathName;

	// see if the module exists without a path prepended yet an extension appended?
	} else if (std::filesystem::exists(pathName.replace_extension(".ot"))) {
		module = pathName.replace_extension(".ot");

	} else if (std::filesystem::exists(pathName.replace_extension(libext))) {
		module = pathName.replace_extension(libext);
	}

	// find module on path (if still required)
	for (size_t i = 0; i < modulePath.size() && module.empty(); i++) {
		auto path = modulePath[i].append(name);

		if (std::filesystem::exists(path)) {
			module = std::filesystem::canonical(path);

		} else if (std::filesystem::exists(path.replace_extension(".ot"))) {
			module = std::filesystem::canonical(path.replace_extension(".ot"));

		} else if (std::filesystem::exists(path.replace_extension(libext))) {
			module = std::filesystem::canonical(path.replace_extension(libext));
		}
	}

	if (!module.empty()) {
		// setup module's meta data
		auto filePath = std::filesystem::canonical(module);
		set("__FILE__", OtStringClass::create(filePath.string()));
		set("__DIR__", OtStringClass::create(filePath.parent_path().string()));

		if (module.extension() == libext) {
#if defined(_WIN32)
			HMODULE lib = LoadLibrary(module.c_str());

			if (!lib) {
				OtExcept("Can't import module [%s], error [%d]", name.c_str(), GetLastError());
			}

			void (*init)(OtObject) = (void (*)(OtObject)) GetProcAddress(lib, "init");

#else
			void* lib = dlopen(module.c_str(), RTLD_LAZY);

			if (!lib) {
				OtExcept("Can't import module [%s], error [%s]", name.c_str(), dlerror());
			}

			void (*init)(OtObject) = (void (*)(OtObject)) dlsym(lib, "init");
#endif

			if (!init) {
				OtExcept("Module [%s] does not have an [init] function", name.c_str());
			}

			(*init)(shared());

		} else {
			std::ifstream stream(module.c_str());
			std::stringstream buffer;
			buffer << stream.rdbuf();
			stream.close();

			OtCompiler compiler;
			OtSource source = OtSourceClass::create(name, buffer.str());
			OtByteCode bytecode = compiler.compileModule(source, cast<OtModuleClass>());

			OtVM::execute(bytecode);
		}

	} else {
		OtExcept("Can't find module [%s]", name.c_str());
	}
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

OtModule OtModuleClass::create() {
	OtModule module = std::make_shared<OtModuleClass>();
	module->setType(getMeta());
	return module;
}