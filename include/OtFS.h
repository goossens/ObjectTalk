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
//	OtFS
//

class OtFSClass;
typedef std::shared_ptr<OtFSClass> OtFS;


//
//	OtFSClass
//

class OtFSClass : public OtOSClass
{
public:
	OtFSClass() {}

	// get current working directory
	OtObject getcwd() { return OtPathClass::create(std::filesystem::current_path()); }

	// get list of files in specified directory
	OtObject ls(const std::string& path)
	{
		// get content of directory
		OtArray result = OtArrayClass::create();

		for(auto& p: std::filesystem::directory_iterator(path))
			result->push_back(OtPathClass::create(p));

		return result;
	}

	// get file size
	size_t filesize(const std::string& path) { return std::filesystem::file_size(path); }

	// remove file
	void rm(const std::string& path) { std::filesystem::remove(path); }

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtFSClass>("FS", OtOSClass::getMeta());

			type->set("getcwd", OtFunctionCreate(&OtFSClass::getcwd));
			type->set("ls", OtFunctionCreate(&OtFSClass::ls));
			type->set("filesize", OtFunctionCreate(&OtFSClass::filesize));
			type->set("rm", OtFunctionCreate(&OtFSClass::rm));
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
