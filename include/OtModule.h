//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


//
//	OtModule
//

class OtModuleClass;
typedef std::shared_ptr<OtModuleClass> OtModule;

class OtModuleClass : public OtInternalClass {
public:
	// constructor
	OtModuleClass() = default;

	// import module and return it
	OtObject import(const std::string& name) {
		load(name);
		return getSharedPtr();
	}

	// // load module and run in specified context
	OtObject run(const std::string& name, OtObject context=nullptr) {
		setParent(context);
		return load(name);
	}

	// build module path
	static void buildModulePath(const char* argv[]) {
		modulePath.clear();

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

		// use lib directory
		auto exec = std::filesystem::path(argv[0]);
		auto root = std::filesystem::canonical(exec).parent_path().parent_path();
		auto lib = root.append("lib").append("ot");

		if (std::filesystem::is_directory(lib)) {
			modulePath.push_back(lib);
		}
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtModuleClass>("Module", OtInternalClass::getMeta());
		}

		return type;
	}

	// create a new object
	static OtModule create() {
		OtModule module = std::make_shared<OtModuleClass>();
		module->setType(getMeta());
		return module;
	}

private:
	// list of directories to search for modules in
	static inline std::vector<std::filesystem::path> modulePath;

	// load a module
	OtObject load(const std::string& name) {
		std::filesystem::path module;

		// find module
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

			if (module.extension() == ".ot") {
				std::ifstream stream(module.c_str());
				std::stringstream buffer;
				buffer << stream.rdbuf();
				set("__SRC__", OtStringClass::create(buffer.str()));

				OtCompiler compiler;
				OtCode code = compiler.compile(buffer.str());
				return code->operator ()(getSharedPtr());

			} else if (module.extension() == ".so") {
#if defined(WINVER)
#else
				void* lib = dlopen(module.c_str(), RTLD_LAZY);

				if (!lib) {
					OT_EXCEPT("Can't import module [%s], error [%s]", name.c_str(), dlerror());
				}

				void (*init)(OtObject) = (void (*)(OtObject)) dlsym(lib, "init");

				if (!init) {
					OT_EXCEPT("No [init] function in module [%s]", name.c_str());
				}

				(*init)(getSharedPtr());
#endif

				return nullptr;
			}
		}

		OT_EXCEPT("Can't import module [%s]", name.c_str());
	}
};
