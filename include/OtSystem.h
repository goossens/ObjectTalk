//	ObjectTalk Scripting Language
//	Copyright 1993-2019 Johan A. Goossens
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
//	OtSystem
//

class OtSystemClass;
typedef std::shared_ptr<OtSystemClass> OtSystem;


//
//	OtSystemClass
//

class OtSystemClass : public OtObjectClass
{
public:
	OtSystemClass() {}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;
		if (!type) { type = OtTypeClass::create<OtSystemClass>("System", OtObjectClass::getMeta()); }
		return type;
	}
};