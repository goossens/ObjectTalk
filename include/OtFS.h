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
//  OtFS
//

class OtFSClass;
typedef std::shared_ptr<OtFSClass> OtFS;


//
//  OtFSClass
//

class OtFSClass : public OtOSClass
{
public:
	OtFSClass() {}

	OtObject getcwd()
	{
		char path[PATH_MAX];

		if (::getcwd(path, PATH_MAX) == NULL)
			OT_EXCEPT("Internal error in getcwd(): %s", strerror(errno));

		return OtPathClass::create(path);
	}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtFSClass>("FS", OtOSClass::getMeta());

			type->set("getcwd", OtFunctionCreate(&OtFSClass::getcwd));
		}

		return type;
	}

	// create a new object
	static OtFS create()
	{
		OtFS fs = std::make_shared<OtFSClass>();
		fs->setType(getMeta());
		return fs;
	}
};
