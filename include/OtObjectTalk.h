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
//	OtObjectTalk
//

class OtObjectTalk {
public:
	// create a default ObjectTalk context
	static OtObject createDefaultContext() {
		// create context
		OtObject context = OtObjectClass::create();

		// add default constants
		context->set("true", OtBooleanClass::create(true));
		context->set("false", OtBooleanClass::create(false));
		context->set("null", nullptr);

		// add default functions
		context->set("assert", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject {
			if (c != 1) {
				OT_EXCEPT("Function [assert] expects 1 parameter, %d given", c);
			}

			std::string assertion = p[0]->operator std::string();
			OtObject result = processText(assertion, context);

			if (!result->operator bool()) {
				OT_EXCEPT("Assertion [%s] failed", assertion.c_str());
			}

			return nullptr;
		}));

		context->set("import", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject
		{
			if (c != 1) {
				OT_EXCEPT("Function [import] expects 1 parameter, %d given", c);
			}

			return importModule(p[0]->operator std::string(), context);
		}));

		context->set("run", OtFunctionClass::create([] (OtObject, size_t c, OtObject* p)->OtObject {
			if (c != 1) {
				OT_EXCEPT("Function [run] expects 1 parameter, %d given", c);
			}

			return runFile(p[0]->operator std::string());
		}));

		context->set("print", OtFunctionClass::create([] (OtObject, size_t c, OtObject* p)->OtObject {
			for (size_t i = 0; i < c; i++) {
				std::cout << (std::string) *p[i];
			}

			std::cout << std::endl;
			return nullptr;
		}));

		// add default classes
		context->set("Object", OtClassClass::create(OtObjectClass::getMeta()));

		context->set("Internal", OtClassClass::create(OtInternalClass::getMeta()));
		context->set("Class", OtClassClass::create(OtClassClass::getMeta()));

		context->set("Primitive", OtClassClass::create(OtPrimitiveClass::getMeta()));
		context->set("Boolean", OtClassClass::create(OtBooleanClass::getMeta()));
		context->set("Integer", OtClassClass::create(OtIntegerClass::getMeta()));
		context->set("Real", OtClassClass::create(OtRealClass::getMeta()));
		context->set("String", OtClassClass::create(OtStringClass::getMeta()));
		context->set("Function", OtClassClass::create(OtFunctionClass::getMeta()));

		context->set("Collection", OtClassClass::create(OtCollectionClass::getMeta()));
		context->set("Array", OtClassClass::create(OtArrayClass::getMeta()));
		context->set("Dict", OtClassClass::create(OtDictClass::getMeta()));

		// return default context
		return context;
	}

	static inline std::vector<std::filesystem::path> modulePath;

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

	// compile and run ObjectTalk text
	static OtObject processText(const std::string& text, OtObject context) {
		OtCompiler compiler;
		OtCode code = compiler.compile(text);
		return code->operator ()(context);
	}

	// compile and import an ObjectTalk module into the specified context
	static OtObject importModule(const std::string& file, OtObject context) {
		// find module
		std::filesystem::path module;

		for (size_t i = 0; i < modulePath.size() && module.empty(); i++) {
			auto path = modulePath[i].append(file);

			if (std::filesystem::exists(path)) {
				module = std::filesystem::canonical(path);

			} else if (std::filesystem::exists(path.replace_extension(".ot"))) {
				module = std::filesystem::canonical(path.replace_extension(".ot"));

			} else if (std::filesystem::exists(path.replace_extension(".so"))) {
				module = std::filesystem::canonical(path.replace_extension(".so"));
			}
		}

		if (!module.empty()) {
			if (module.extension() == ".ot") {
				std::ifstream stream(module.c_str());
				std::stringstream buffer;
				buffer << stream.rdbuf();

				return processText(buffer.str(), context);

			} else if (module.extension() == ".so") {
#if defined(WINVER)
#else
				void* lib = dlopen(module.c_str(), RTLD_LAZY);

				if (!lib) {
					OT_EXCEPT("Can't import module [%s], error [%s]", file.c_str(), dlerror());
				}

				void (*init)(OtObject) = (void (*)(OtObject)) dlsym(lib, "init");

				if (!init) {
					OT_EXCEPT("No [init] function in module [%s]", file.c_str());
				}

				(*init)(context);
#endif

				return nullptr;
			}
		}

		OT_EXCEPT("Can't import module [%s]", file.c_str());
	}

	// compile and run an ObjectTalk file in a new context
	static OtObject runFile(const std::string& path) {
		OtObject context = createDefaultContext();
		context->set("__FILE__", OtStringClass::create(path));
		return importModule(path, context);
	}

private:
	// constructor
	OtObjectTalk() = default;
};
