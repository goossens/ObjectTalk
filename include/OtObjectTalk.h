//	ObjectTalk Scripting Language
//	Copyright 1993-2020 Johan A. Goossens
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
		context->set(L"true", OtBooleanClass::create(true));
		context->set(L"false", OtBooleanClass::create(false));
		context->set(L"null", nullptr);

		// add default functions
		context->set(L"assert", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject {
			if (c != 1) {
				OT_EXCEPT(L"Function [assert] expects 1 parameter, %d given", c);
			}

			std::wstring assertion = p[0]->operator std::wstring();
			OtObject result = processText(assertion, context);

			if (!result->operator bool()) {
				OT_EXCEPT(L"Assertion [%ls] failed", assertion.c_str());
			}

			return nullptr;
		}));

		context->set(L"import", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject
		{
			if (c != 1) {
				OT_EXCEPT(L"Function [import] expects 1 parameter, %d given", c);
			}

			return importModule(p[0]->operator std::wstring(), context);
		}));

		context->set(L"run", OtFunctionClass::create([] (OtObject, size_t c, OtObject* p)->OtObject {
			if (c != 1) {
				OT_EXCEPT(L"Function [run] expects 1 parameter, %d given", c);
			}

			return runFile(p[0]->operator std::wstring());
		}));

		context->set(L"print", OtFunctionClass::create([] (OtObject, size_t c, OtObject* p)->OtObject {
			for (size_t i = 0; i < c; i++) {
				std::wcout << (std::wstring) *p[i];
			}

			std::wcout << std::endl;
			return nullptr;
		}));

		// add default classes
		context->set(L"Object", OtClassClass::create(OtObjectClass::getMeta()));

		context->set(L"Internal", OtClassClass::create(OtInternalClass::getMeta()));
		context->set(L"Class", OtClassClass::create(OtClassClass::getMeta()));

		context->set(L"Primitive", OtClassClass::create(OtPrimitiveClass::getMeta()));
		context->set(L"Boolean", OtClassClass::create(OtBooleanClass::getMeta()));
		context->set(L"Integer", OtClassClass::create(OtIntegerClass::getMeta()));
		context->set(L"Real", OtClassClass::create(OtRealClass::getMeta()));
		context->set(L"String", OtClassClass::create(OtStringClass::getMeta()));
		context->set(L"Function", OtClassClass::create(OtFunctionClass::getMeta()));

		context->set(L"Collection", OtClassClass::create(OtCollectionClass::getMeta()));
		context->set(L"Array", OtClassClass::create(OtArrayClass::getMeta()));
		context->set(L"Dict", OtClassClass::create(OtDictClass::getMeta()));

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
			std::vector<std::wstring> parts;
			OtTextSplit(OtTextToWide(path), parts, L';');

			for (auto part = parts.begin(); part != parts.end(); ++part) {
				modulePath.push_back(*part);
			}
		}

		// use lib directory
		auto exec = std::filesystem::path(argv[0]);
		auto bin = std::filesystem::canonical(exec).parent_path();
		auto lib = bin.parent_path().append("lib").append("ot");

		if (std::filesystem::is_directory(lib)) {
			modulePath.push_back(lib);
		}
	}

	// compile and run ObjectTalk text
	static OtObject processText(const std::wstring& text, OtObject context) {
		OtCompiler compiler;
		OtCode code = compiler.compile(text);
		return code->operator ()(context);
	}

	// compile and import an ObjectTalk module into the specified context
	static OtObject importModule(const std::wstring& file, OtObject context) {
		// find module
		std::filesystem::path module;

		for (size_t i = 0; i < modulePath.size() && module.empty(); i++) {
			auto path = modulePath[i].append(file);

			if (std::filesystem::exists(path)) {
				module = std::filesystem::canonical(path);

			} else if (std::filesystem::exists(path.replace_extension(L".ot"))) {
				module = std::filesystem::canonical(path.replace_extension(L".ot"));

			} else if (std::filesystem::exists(path.replace_extension(L".so"))) {
				module = std::filesystem::canonical(path.replace_extension(L".so"));
			}
		}

		if (!module.empty()) {
			if (module.extension() == ".ot") {
				std::wifstream stream(module.c_str());
				std::wstringstream buffer;
				buffer << stream.rdbuf();

				return processText(buffer.str(), context);

			} else if (module.extension() == ".so") {
#if defined(WINVER)
#else
				void* lib = dlopen(module.c_str(), RTLD_LAZY);

				if (!lib) {
					OT_EXCEPT(L"Can't import module [%ls], error [%s]", file.c_str(), dlerror());
				}

				void (*init)(OtObject) = (void (*)(OtObject)) dlsym(lib, "init");

				if (!init) {
					OT_EXCEPT(L"No [init] function in module [%ls]", file.c_str());
				}

				(*init)(context);
#endif

				return 0;
			}
		}

		OT_EXCEPT(L"Can't import module [%ls]", file.c_str());
	}

	// compile and run an ObjectTalk file in a new context
	static OtObject runFile(const std::wstring& path) {
		OtObject context = createDefaultContext();
		context->set(L"__FILE__", OtStringClass::create(path));
		return importModule(path, context);
	}

private:
	// constructor
	OtObjectTalk() {}
};
