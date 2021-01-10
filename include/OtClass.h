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
//	OtClass
//

class OtClassClass;
typedef std::shared_ptr<OtClassClass> OtClass;


//
//	OtClassClass
//

class OtClassClass : public OtInternalClass {
public:
	// constructors
	OtClassClass() = default;
	OtClassClass(OtType t) : classType(t) {}

	// access member information
	std::wstring getName() { return classType->getName(); }
	bool hasParent() { return classType->getParent() != nullptr; }
	OtObject getParent() { return OtClassClass::create(classType->getParent()); }

	// create a sub class
	OtObject subType(const std::wstring& name) { return OtClassClass::create(classType->subType(name)); }

	// see if class is kind of
	bool isKindOf(const std::wstring& className) { return classType->isKindOf(className); }

	// special member acccess (so we can add methods to metaclass via class
	OtObject set(const std::wstring& name, OtObject value) { return classType->set(name, value); }

	// call operator
	OtObject operator () (OtObject context, size_t count, OtObject* parameters) {
		// create new instance
		OtObject value = classType->instantiate();

		// run possible init function
		if (value->has(L"__init__")) {
			OtObject pars[count + 1];
			pars[0] = value;

			for (size_t c = 0; c < count; c++) {
				pars[c + 1] = parameters[c];
			}

			value->get(L"__init__")->operator ()(context, count + 1, pars);
		}

		return value;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtClassClass>(L"Class", OtInternalClass::getMeta());
			type->set(L"__call__", OtFunctionClass::create(&OtClassClass::operator ()));
			type->set(L"getName", OtFunctionCreate(&OtClassClass::getName));
			type->set(L"hasParent", OtFunctionCreate(&OtClassClass::hasParent));
			type->set(L"getParent", OtFunctionCreate(&OtClassClass::getParent));
			type->set(L"subType", OtFunctionCreate(&OtClassClass::subType));
			type->set(L"isKindOf", OtFunctionCreate(&OtClassClass::isKindOf));
		}

		return type;
	}

	// create a new object
	static OtClass create(OtType type) {
		OtClass cls = std::make_shared<OtClassClass>(type);
		cls->setType(getMeta());
		return cls;
	}

protected:
	OtType classType;
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

OtObject OtObjectCreate(OtType type) { return OtClassClass::create(type); };
