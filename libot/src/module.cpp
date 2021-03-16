//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#if defined(WINVER)
#else
#include <dlfcn.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>

#include <uv.h>

#include "ot/exception.h"
#include "ot/text.h"

#include "ot/string.h"
#include "ot/module.h"
#include "ot/code.h"
#include "ot/compiler.h"


#define UV_CHECK_ERROR(action, status) if (status < 0) OT_EXCEPT("libuv error in %s: %s", action, uv_strerror(status))


//
//	OtModuleClass::getMeta
//

OtType OtModuleClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtModuleClass>("Module", OtContextClass::getMeta());
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
//	OtModuleClass::load
//

OtObject OtModuleClass::load(const std::string& name) {
	// build module path (if required)
	if (modulePath.size() == 0) {
		// use local directory as default
		modulePath.push_back(std::filesystem::canonical("."));

		// use OT_PATH environment variable if provided
		auto path = std::getenv("OT_PATH");

		if (path) {
			std::vector<std::string> parts;
			OtTextSplit(path, parts,';');

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

	std::filesystem::path module;

	// see if the module exists without a path prepended?
	if (std::filesystem::exists(name)) {
		module = std::filesystem::path(name);

	// see if it's an absolute path?
	} else if (std::filesystem::path(name).replace_extension(".ot").is_absolute() &&
			std::filesystem::exists(std::filesystem::path(name).replace_extension(".ot"))) {
		module = std::filesystem::path(name).replace_extension(".ot");
	}

	// find module on path (if still required)
	for (size_t i = 0; i < modulePath.size() && module.empty(); i++) {
		auto path = modulePath[i].append(name);

		if (std::filesystem::exists(path)) {
			module = std::filesystem::canonical(path);

		} else if (std::filesystem::exists(path.replace_extension(".ot"))) {
			module = std::filesystem::canonical(path.replace_extension(".ot"));

		} else if (std::filesystem::exists(path.replace_extension(".so"))) {
			module = std::filesystem::canonical(path.replace_extension(".so"));
		}
	}

	if (!module.empty()) {
		// setup module's meta data
		auto filePath = std::filesystem::canonical(module);
		set("__FILE__", OtStringClass::create(filePath.string()));
		set("__DIR__", OtStringClass::create(filePath.parent_path().string()));

#if defined(WINVER)
		if (module.extension() == ".dll") {
			//TODO

#else
		if (module.extension() == ".so") {
			void* lib = dlopen(module.c_str(), RTLD_LAZY);

			if (!lib) {
				OT_EXCEPT("Can't import module [%s], error [%s]", name.c_str(), dlerror());
			}

			void (*init)(OtObject) = (void (*)(OtObject)) dlsym(lib, "init");
			OT_ASSERT(init);
			(*init)(getSharedPtr());
#endif

			return nullptr;

		} else {
			std::ifstream stream(module.c_str());
			std::stringstream buffer;
			buffer << stream.rdbuf();
			stream.close();

			OtCompiler compiler;
			OtCode code = compiler.compile(buffer.str());
			return code->operator ()(getSharedPtr()->cast<OtContextClass>());
		}
	}

	OT_EXCEPT("Can't import module [%s]", name.c_str());
}
