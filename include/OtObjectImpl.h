//	ObjectTalk Scripting Language
//	Copyright 1993-2018 Johan A. Goossens
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
//
//  OtMemberReferenceClass
//

class OtMemberReferenceClass : public OtInternalClass
{
public:
	OtMemberReferenceClass() {}
	OtMemberReferenceClass(OtObject o, const std::string& m) { object = o; member = m; }

	OtObject deref()
	{
		OtObject result = object->get(member);

		if (result && result->isKindOf("Function"))
			return OtBoundFunctionClass::create(object, result);

		else
			return result;
	}

	OtObject assign(OtObject value)
	{
		return object->set(member, value);
	}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtMemberReferenceClass>("MemberReference", OtInternalClass::getMeta());
			type->set("__deref__", OtFunctionCreate(&OtMemberReferenceClass::deref));
			type->set("__assign__", OtFunctionCreate(&OtMemberReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtObject create(OtObject o, const std::string& m) { return std::make_shared<OtMemberReferenceClass>(o, m)->setType(getMeta()); }

private:
	OtObject object;
	std::string member;
};


//
//  OtObjectClass::getMeta
//

OtType OtObjectClass::getMeta()
{
	static OtType type = nullptr;

	if (!type)
	{
		type = OtTypeClass::create<OtObjectClass>("Object");

		type->set("__member__", OtFunctionCreate(
		std::function<OtObject(OtObject, std::string)>([] (OtObject o, std::string n)->OtObject
		{
			return OtMemberReferenceClass::create(o, n);
		})));

		type->set("boolean", OtFunctionCreate(&OtObjectClass::operator bool));
		type->set("integer", OtFunctionCreate(&OtObjectClass::operator long));
		type->set("real", OtFunctionCreate(&OtObjectClass::operator double));
		type->set("string", OtFunctionCreate(&OtObjectClass::operator std::string));

		type->set("has", OtFunctionCreate(&OtObjectClass::has));
		type->set("get", OtFunctionCreate(&OtObjectClass::get));
		type->set("set", OtFunctionCreate(&OtObjectClass::set));

		type->set("getClass", OtFunctionCreate(
		std::function<OtObject(OtObject)>([] (OtObject o)->OtObject
		{
			return OtClassClass::create(o->getType());
		})));

		type->set("isKindOf", OtFunctionCreate(
		std::function<bool(OtObject, const std::string&)>([] (OtObject o, const std::string& n)->bool
		{
			return o->isKindOf(n);
		})));
	}

	return type;
}
