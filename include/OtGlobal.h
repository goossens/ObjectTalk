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
//	OtGlobal
//

class OtGlobalClass;
typedef std::shared_ptr<OtGlobalClass> OtGlobal;

class OtGlobalClass : public OtInternalClass {
public:
	// constructor (create a default ObjectTalk context)
	OtGlobalClass() {
		// add default constants
		set("true", OtBooleanClass::create(true));
		set("false", OtBooleanClass::create(false));
		set("null", nullptr);

		// add default functions

		// assert()
		set("assert", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject {
			if (c != 1) {
				OT_EXCEPT("Function [assert] expects 1 parameter, %d given", c);
			}

			std::string assertion = p[0]->operator std::string();
			OtCompiler compiler;
			OtCode code = compiler.compile(assertion);
			OtObject result = code->operator ()(context);

			if (!result->operator bool()) {
				OT_EXCEPT("Assertion [%s] failed", assertion.c_str());
			}

			return nullptr;
		}));

		// import()
		set("import", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject
		{
			if (c != 1) {
				OT_EXCEPT("Function [import] expects 1 parameter, %d given", c);
			}

			OtModule module = OtModuleClass::create();
			return module->import(p[0]->operator std::string());
		}));

		// print()
		set("print", OtFunctionClass::create([] (OtObject, size_t c, OtObject* p)->OtObject {
			for (size_t i = 0; i < c; i++) {
				std::cout << (std::string) *p[i];
			}

			std::cout << std::endl;
			return nullptr;
		}));

		// add default classes
		set("Object", OtClassClass::create(OtObjectClass::getMeta()));

		set("Primitive", OtClassClass::create(OtPrimitiveClass::getMeta()));
		set("Boolean", OtClassClass::create(OtBooleanClass::getMeta()));
		set("Integer", OtClassClass::create(OtIntegerClass::getMeta()));
		set("Real", OtClassClass::create(OtRealClass::getMeta()));
		set("String", OtClassClass::create(OtStringClass::getMeta()));
		set("Function", OtClassClass::create(OtFunctionClass::getMeta()));

		set("Collection", OtClassClass::create(OtCollectionClass::getMeta()));
		set("Array", OtClassClass::create(OtArrayClass::getMeta()));
		set("Dict", OtClassClass::create(OtDictClass::getMeta()));
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtGlobalClass>("Global", OtInternalClass::getMeta());
		}

		return type;
	}

	// create a new object
	static OtGlobal create() {
		OtGlobal module = std::make_shared<OtGlobalClass>();
		module->setType(getMeta());
		return module;
	}
};
