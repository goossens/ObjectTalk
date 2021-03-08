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

class OtGlobalClass : public OtContextClass {
public:
	// constructor (create a default ObjectTalk context)
	OtGlobalClass() {
		// add default constants
		set("true", OtBooleanClass::create(true));
		set("false", OtBooleanClass::create(false));
		set("null", nullptr);

		// add default functions
		set("assert", OtFunctionClass::create(&OtGlobalClass::doAssert));
		set("import", OtFunctionClass::create(&OtGlobalClass::import));
		set("print", OtFunctionClass::create(&OtGlobalClass::print));

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

	// check assertion
	static void doAssert(bool condition) {
		if (!condition) {
			throw OtException("Assertion error");
		}
	}

	// import module
	static OtObject import(const std::string name) {
		OtModule module = OtModuleClass::create();
		return module->import(name);
	}

	// print to STDOUT
	static OtObject print(OtContext context, size_t count, OtObject* parameters) {
		for (size_t i = 0; i < count; i++) {
			std::cout << (std::string) *parameters[i];
		}

		std::cout << std::endl;
		return nullptr;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtGlobalClass>("Global", OtContextClass::getMeta());
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
