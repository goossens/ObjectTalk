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
//	OtBoundFunctionClass
//

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
			type = OtTypeClass::create<OtBoundFunctionClass>(L"BoundFunction", OtInternalClass::getMeta());
			type->set(L"__call__", OtFunctionClass::create(&OtBoundFunctionClass::operator ()));
		}

		return type;
	}

	// create a new object
	static OtObject create(OtObject o, OtObject f) { return std::make_shared<OtBoundFunctionClass>(o, f)->setType(getMeta()); }

private:
	OtObject object;
	OtObject function;
};


//
//	OtMemberReferenceClass
//

class OtMemberReferenceClass : public OtInternalClass {
public:
	OtMemberReferenceClass() = default;
	OtMemberReferenceClass(OtObject o, const std::wstring& m) : object(o), member(m) {}

	OtObject deref() {
		OtObject result = object->get(member);

		if (result && (result->isKindOf(L"Function") || result->isKindOf(L"CodeFunction"))) {
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
			type = OtTypeClass::create<OtMemberReferenceClass>(L"MemberReference", OtInternalClass::getMeta());
			type->set(L"__deref__", OtFunctionCreate(&OtMemberReferenceClass::deref));
			type->set(L"__assign__", OtFunctionCreate(&OtMemberReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtObject create(OtObject o, const std::wstring& m) { return std::make_shared<OtMemberReferenceClass>(o, m)->setType(getMeta()); }

private:
	OtObject object;
	std::wstring member;
};


//
//	OtObjectClass::getMeta
//

OtType OtObjectClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtObjectClass>(L"Object");

		type->set(L"__member__", OtFunctionCreate(std::function<OtObject(OtObject, std::wstring)>([] (OtObject o, std::wstring n)->OtObject {
			return OtMemberReferenceClass::create(o, n);
		})));

		type->set(L"boolean", OtFunctionCreate(&OtObjectClass::operator bool));
		type->set(L"integer", OtFunctionCreate(&OtObjectClass::operator long));
		type->set(L"real", OtFunctionCreate(&OtObjectClass::operator double));
		type->set(L"string", OtFunctionCreate(&OtObjectClass::operator std::wstring));

		type->set(L"has", OtFunctionCreate(&OtObjectClass::has));
		type->set(L"get", OtFunctionCreate(&OtObjectClass::get));
		type->set(L"set", OtFunctionCreate(&OtObjectClass::set));

		type->set(L"getClass", OtFunctionCreate(std::function<OtObject(OtObject)>([] (OtObject o)->OtObject{
			return OtClassClass::create(o->getType());
		})));

		type->set(L"isKindOf", OtFunctionCreate(std::function<bool(OtObject, const std::wstring&)>([] (OtObject o, const std::wstring& n)->bool {
			return o->isKindOf(n);
		})));
	}

	return type;
}
