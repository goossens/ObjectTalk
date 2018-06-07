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
//  OtBoundFunction
//

class OtBoundFunctionClass;
typedef std::shared_ptr<OtBoundFunctionClass> OtBoundFunction;


//
//  OtBoundFunctionClass
//

class OtBoundFunctionClass : public OtInternalClass
{
public:
	// constructor
	OtBoundFunctionClass() {}
	OtBoundFunctionClass(OtObject o, OtObject f) { object = o; function = f; }

	// call bound function
	OtObject operator () (OtObject context, size_t count, OtObject* parameters)
	{
		OtObject pars[count + 1];
		pars[0] = object;

		if (count)
			std::copy(parameters, parameters + count, pars + 1);

		return OtTypeClass::cast<OtFunctionClass>(function)->operator ()(context, count + 1, pars);
	}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtBoundFunctionClass>("BoundFunction", OtInternalClass::getMeta());
			type->set("__call__", OtFunctionClass::create(&OtBoundFunctionClass::operator ()));
		}

		return type;
	}

	// create a new object
	static OtObject create(OtObject o, OtObject f) { return std::make_shared<OtBoundFunctionClass>(o, f)->setType(getMeta()); }

private:
	OtObject object;
	OtObject function;
};
