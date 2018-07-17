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
//	OtCodeFunction
//

class OtCodeFunctionClass;
typedef std::shared_ptr<OtCodeFunctionClass> OtCodeFunction;


//
//	OtCodeFunctionClass
//

class OtCodeFunctionClass : public OtFunctionClass
{
public:
	// constructor
	OtCodeFunctionClass() {}
	OtCodeFunctionClass(OtCode c) { code = c; }

	// call code
	OtObject operator () (OtObject c, size_t n, OtObject* p) { return code->operator ()(c, n, p); }

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtCodeFunctionClass>("CodeFunction", OtFunctionClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtCodeFunctionClass::operator ()));
		}

		return type;
	}

	// create a new object
	static OtObject create(OtCode c) { return std::make_shared<OtCodeFunctionClass>(c)->setType(getMeta()); }

private:
	OtCode code;
};
