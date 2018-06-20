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
//  OtCollection
//

class OtCollectionClass;
typedef std::shared_ptr<OtCollectionClass> OtCollection;


//
//  OtCollectionClass
//

class OtCollectionClass : public OtObjectClass
{
public:
	OtCollectionClass() {}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;
        if (!type) { type = OtTypeClass::create<OtCollectionClass>("Collection", OtObjectClass::getMeta()); }
		return type;
	}
};
