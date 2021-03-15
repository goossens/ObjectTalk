//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/exception.h"
#include "ot/object.h"
#include "ot/function.h"
#include "ot/class.h"
#include "ot/reference.h"


//
//	OtObjectClass::hasMember
//

bool OtObjectClass::hasMember(const std::string& name) {
	return members && members->count(name);
}


//
//	OtObjectClass::getMember
//

OtObject OtObjectClass::getMember(const std::string& name) {
	return members->operator [] (name);
}


//
//	OtObjectClass::has
//

bool OtObjectClass::has(const std::string& name) {
	if (hasMember(name)) {
		return true;
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(name)) {
			return true;
		}
	}

	return false;
}


//
//	OtObjectClass::set
//

OtObject OtObjectClass::set(const std::string& name, OtObject value) {
	if (!members) {
		members = std::make_shared<std::unordered_map<std::string, OtObject>>();
	}

	members->operator [] (name) = value;
	return value;
}


//
//	OtObjectClass::get
//

OtObject OtObjectClass::get(const std::string& name) {
	if (hasMember(name)) {
		return getMember(name);
	}

	for (auto t = type; t; t = t->getParent()) {
		if (t->has(name)) {
			return t->get(name);
		}
	}

	OT_EXCEPT("Unknown member [%s] in instance of class [%s]", name.c_str(), type->getName().c_str());
	return nullptr;
}


//
//	OtObjectClass::method
//

OtObject OtObjectClass::method(const std::string& m, OtContext c, size_t n, OtObject* p) {
	OtObject pars[n + 1];
	pars[0] = getSharedPtr();
	std::copy(p, p + n, pars + 1);
	return get(m)->operator ()(c, n + 1, pars);
}


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
	OtObject operator () (OtContext context, size_t count, OtObject* parameters) {
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

class OtMemberReferenceClass : public OtReferenceClass {
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
			type = OtTypeClass::create<OtMemberReferenceClass>("MemberReference", OtReferenceClass::getMeta());
			type->set("__deref__", OtFunctionClass::create(&OtMemberReferenceClass::deref));
			type->set("__assign__", OtFunctionClass::create(&OtMemberReferenceClass::assign));
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
//	OtObjectClass::member
//

OtObject OtObjectClass::member(const std::string& name) {
	return OtMemberReferenceClass::create(getSharedPtr(), name);
}


//
//	OtObjectClass::getClass
//

OtClass OtObjectClass::getClass() {
	return OtClassClass::create(getType());
}


//
//	OtObjectClass::getMeta
//

OtType OtObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtObjectClass>("Object", nullptr);

		type->set("boolean", OtFunctionClass::create(&OtObjectClass::toBoolean));
		type->set("integer", OtFunctionClass::create(&OtObjectClass::toInteger));
		type->set("real", OtFunctionClass::create(&OtObjectClass::toReal));
		type->set("string", OtFunctionClass::create(&OtObjectClass::toString));
		type->set("json", OtFunctionClass::create(&OtObjectClass::toJSON));

		type->set("has", OtFunctionClass::create(&OtObjectClass::has));
		type->set("__member__", OtFunctionClass::create(&OtObjectClass::member));
		type->set("eraseMember", OtFunctionClass::create(&OtObjectClass::eraseMember));
		type->set("clearMembers", OtFunctionClass::create(&OtObjectClass::clearMembers));

		type->set("getClass", OtFunctionClass::create(&OtObjectClass::getClass));
		type->set("isKindOf", OtFunctionClass::create(&OtObjectClass::isKindOf));
	}

	return type;
}


//
//	OtObjectClass::create
//

OtObject OtObjectClass::create() {
	OtObject object = std::make_shared<OtObjectClass>();
	object->setType(getMeta());
	return object;
}
