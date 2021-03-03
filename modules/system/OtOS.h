//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtOS
//

class OtOSClass;
typedef std::shared_ptr<OtOSClass> OtOS;

class OtOSClass : public OtSystemClass {
public:
	// see if environment variable exists
	bool hasenv(const std::string& name) {
		size_t size = 256;
		char* value = (char*) malloc(size);
		int result = uv_os_getenv(name.c_str(), value, &size);
		free(value);
		return result != UV_ENOENT;
	}

	// get environment variable
	std::string getenv(const std::string& name) {
		size_t size = 256;
		char* value = (char*) malloc(size);
		int result = uv_os_getenv(name.c_str(), value, &size);

		if (result == UV_ENOBUFS) {
			value = (char*) realloc(value, size);
			result = uv_os_getenv(name.c_str(), value, &size);
		}

		if (result == UV_ENOENT) {
			return nullptr;
		}

		std::string v = value;
		free(value);
		return v;
	}

	// set environment variable
	void setenv(const std::string& name, const std::string& value) {
		uv_os_setenv(name.c_str(), value.c_str());
	}

	// unset environment variable
	void unsetenv(const std::string& name) {
		uv_os_unsetenv(name.c_str());
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtOSClass>("OS", OtSystemClass::getMeta());

			type->set("hasenv", OtFunctionCreate(&OtOSClass::hasenv));
			type->set("getenv", OtFunctionCreate(&OtOSClass::getenv));
			type->set("setenv", OtFunctionCreate(&OtOSClass::setenv));
			type->set("unsetenv", OtFunctionCreate(&OtOSClass::unsetenv));
		}

		return type;
	}

	// create a new object
	static OtOS create() {
		OtOS fs = std::make_shared<OtOSClass>();
		fs->setType(getMeta());
		return fs;
	}
};
