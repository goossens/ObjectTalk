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


#include <uv.h>


//
//	OtOS
//

class OtOSClass;
typedef std::shared_ptr<OtOSClass> OtOS;


//
//	OtOSClass
//

class OtOSClass : public OtSystemClass
{
public:
	OtOSClass() {}

	// see if environment variable exists
	bool hasenv(const std::string& name)
	{
		size_t size = OT_MAX_BUFFER;
		char value[size];
		int result = uv_os_getenv(name.c_str(), value, &size);

		if (result == UV_ENOBUFS)
			OT_EXCEPT("Environment variable [%s] of size %d does not fit in buffer of size %d", name.c_str(), size, UV_ENOBUFS);

		return result != UV_ENOENT;
	}

	// get environment variable
	std::string getenv(const std::string& name)
	{
		size_t size = OT_MAX_BUFFER;
		char value[size];
		int result = uv_os_getenv(name.c_str(), value, &size);

		if (result == UV_ENOBUFS)
			OT_EXCEPT("Environment variable [%s] of size %d does not fit in buffer of size %d", name.c_str(), size, UV_ENOBUFS);

		if (result == UV_ENOENT)
			return 0;

		else
			return std::string(value);
	}

	// set environment variable
	void setenv(const std::string& name, const std::string& value)
	{
		uv_os_setenv(name.c_str(), value.c_str());
	}

	// unset environment variable
	void unsetenv(const std::string& name)
	{
		uv_os_unsetenv(name.c_str());
	}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtOSClass>("OS", OtSystemClass::getMeta());

			type->set("hasenv", OtFunctionCreate(&OtOSClass::hasenv));
			type->set("getenv", OtFunctionCreate(&OtOSClass::getenv));
			type->set("setenv", OtFunctionCreate(&OtOSClass::setenv));
			type->set("unsetenv", OtFunctionCreate(&OtOSClass::unsetenv));
		}

		return type;
	}

	// create a new object
	static OtOS create()
	{
		OtOS fs = std::make_shared<OtOSClass>();
		fs->setType(getMeta());
		return fs;
	}
};
