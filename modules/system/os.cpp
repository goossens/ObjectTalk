//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <uv.h>

#include "ot/function.h"

#include "os.h"


//
//	OtOSClass::hasenv
//

bool OtOSClass::hasenv(const std::string& name) {
	size_t size = 256;
	char* value = (char*) malloc(size);
	int result = uv_os_getenv(name.c_str(), value, &size);
	free(value);
	return result != UV_ENOENT;
}


//
//	OtOSClass::getenv
//

std::string OtOSClass::getenv(const std::string& name) {
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


//
//	OtOSClass::setenv
//

void OtOSClass::setenv(const std::string& name, const std::string& value) {
	uv_os_setenv(name.c_str(), value.c_str());
}


//
//	OtOSClass::unsetenv
//

void OtOSClass::unsetenv(const std::string& name) {
	uv_os_unsetenv(name.c_str());
}


//
//	OtOSClass::getMeta
//

OtType OtOSClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtOSClass>("OS", OtSystemClass::getMeta());

		type->set("hasenv", OtFunctionClass::create(&OtOSClass::hasenv));
		type->set("getenv", OtFunctionClass::create(&OtOSClass::getenv));
		type->set("setenv", OtFunctionClass::create(&OtOSClass::setenv));
		type->set("unsetenv", OtFunctionClass::create(&OtOSClass::unsetenv));
	}

	return type;
}


//
//	OtOSClass::create
//

OtOS OtOSClass::create() {
	OtOS fs = std::make_shared<OtOSClass>();
	fs->setType(getMeta());
	return fs;
}
