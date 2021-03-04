//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


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
				throw OtException(OtFormat("Function [assert] expects 1 parameter, %d given", c));
			}

			std::string assertion = p[0]->operator std::string();
			OtCompiler compiler;
			OtCode code = compiler.compile(assertion);
			OtObject result = code->operator ()(context);

			if (!result->operator bool()) {
				throw OtException(OtFormat("Assertion [%s] failed", assertion.c_str()));
			}

			return nullptr;
		}));

		// import()
		set("import", OtFunctionClass::create([] (OtObject context, size_t c, OtObject* p)->OtObject
		{
			if (c != 1) {
				throw OtException(OtFormat("Function [import] expects 1 parameter, %d given", c));
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
