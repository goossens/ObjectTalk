//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.



//
// OtContextReferenceClass
//

class OtContextReferenceClass;
typedef std::shared_ptr<OtContextReferenceClass> OtContextReference;

class OtContextReferenceClass : public OtInternalClass {
public:
	OtContextReferenceClass() = default;
	OtContextReferenceClass(const std::string& m) : member(m) {}

	OtObject deref(OtContext context, size_t, OtObject*) { return context->get(member); }
	OtObject assign(OtContext context, size_t, OtObject* value) { return context->set(member, *value); }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtContextReferenceClass>("ContextReference", OtInternalClass::getMeta());
			type->set("__deref__", OtFunctionClass::create(&OtContextReferenceClass::deref));
			type->set("__assign__", OtFunctionClass::create(&OtContextReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtContextReference create(const std::string& n) {
		OtContextReference reference = std::make_shared<OtContextReferenceClass>(n);
		reference->setType(getMeta());
		return reference;
	}

private:
	std::string member;
};
