//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtBoundFunction
//

class OtBoundFunctionClass;
typedef std::shared_ptr<OtBoundFunctionClass> OtBoundFunction;

class OtBoundFunctionClass : public OtInternalClass {
public:
	// constructor
	OtBoundFunctionClass() = default;
	OtBoundFunctionClass(OtObject o, OtObject f) : object(o), function(f) {}

	// call bound function
	OtObject operator () (OtObject context, size_t count, OtObject* parameters) {
		OtObject pars[count + 1];
		pars[0] = object;

		if (count) {
			std::copy(parameters, parameters + count, pars + 1);
		}

		return function->operator ()(context, count + 1, pars);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtBoundFunctionClass>("BoundFunction", OtInternalClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtBoundFunctionClass::operator ()));
		}

		return type;
	}

	// create a new object
	static OtBoundFunction create(OtObject o, OtObject f) {
		OtBoundFunction bound = std::make_shared<OtBoundFunctionClass>(o, f);
		bound->setType(getMeta());
		return bound;
	}

private:
	OtObject object;
	OtObject function;
};


//
//	OtMemberReference
//

class OtMemberReferenceClass;
typedef std::shared_ptr<OtMemberReferenceClass> OtMemberReference;

class OtMemberReferenceClass : public OtInternalClass {
public:
	OtMemberReferenceClass() = default;
	OtMemberReferenceClass(OtObject o, const std::string& m) : object(o), member(m) {}

	OtObject deref() {
		OtObject result = object->get(member);

		if (result && (result->isKindOf("Function") || result->isKindOf("CodeFunction"))) {
			return OtBoundFunctionClass::create(object, result);

		} else {
			return result;
		}
	}

	OtObject assign(OtObject value) {
		return object->set(member, value);
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtMemberReferenceClass>("MemberReference", OtInternalClass::getMeta());
			type->set("__deref__", OtFunctionCreate(&OtMemberReferenceClass::deref));
			type->set("__assign__", OtFunctionCreate(&OtMemberReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtMemberReference create(OtObject o, const std::string& m) {
		OtMemberReference member = std::make_shared<OtMemberReferenceClass>(o, m);
		member->setType(getMeta());
		return member;
	}

private:
	OtObject object;
	std::string member;
};


//
//	OtObjectClass::getMeta
//

OtType OtObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtObjectClass>("Object");

		type->set("__member__", OtFunctionCreate(std::function<OtObject(OtObject, std::string)>([] (OtObject o, std::string n)->OtObject {
			return OtMemberReferenceClass::create(o, n);
		})));

		type->set("boolean", OtFunctionCreate(std::function<bool(OtObject)>([] (OtObject o)->bool {
			return o->operator bool();
		})));

		type->set("integer", OtFunctionCreate(std::function<long(OtObject)>([] (OtObject o)->long {
			return o->operator long();
		})));

		type->set("real", OtFunctionCreate(std::function<double(OtObject)>([] (OtObject o)->double {
			return o->operator double();
		})));

		type->set("string", OtFunctionCreate(std::function<std::string(OtObject)>([] (OtObject o)->std::string {
			return o->operator std::string();
		})));

		type->set("json", OtFunctionCreate(std::function<std::string(OtObject)>([] (OtObject o)->std::string {
			return o->json();
		})));

		type->set("has", OtFunctionCreate(&OtObjectClass::has));

		type->set("getClass", OtFunctionCreate(std::function<OtObject(OtObject)>([] (OtObject o)->OtObject {
			return OtClassClass::create(o->getType());
		})));

		type->set("isKindOf", OtFunctionCreate(std::function<bool(OtObject, const std::string&)>([] (OtObject o, const std::string& n)->bool {
			return o->isKindOf(n);
		})));
	}

	return type;
}
