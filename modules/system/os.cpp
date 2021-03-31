//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/libuv.h"
#include "ot/function.h"
#include "ot/array.h"
#include "ot/dict.h"

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
//	OtOSClass::sysname
//

std::string OtOSClass::sysname() {
	uv_utsname_t info;
	auto status = uv_os_uname(&info);
	UV_CHECK_ERROR("uv_os_uname", status);
	return std::string(info.sysname);
}


//
//	OtOSClass::release
//

std::string OtOSClass::release() {
	uv_utsname_t info;
	auto status = uv_os_uname(&info);
	UV_CHECK_ERROR("uv_os_uname", status);
	return std::string(info.release);
}


//
//	OtOSClass::version
//

std::string OtOSClass::version()  {
	uv_utsname_t info;
	auto status = uv_os_uname(&info);
	UV_CHECK_ERROR("uv_os_uname", status);
	return std::string(info.version);
}


//
//	OtOSClass::machine
//

std::string OtOSClass::machine()  {
	uv_utsname_t info;
	auto status = uv_os_uname(&info);
	UV_CHECK_ERROR("uv_os_uname", status);
	return std::string(info.machine);
}


//
//	OtOSClass::uptime
//

double OtOSClass::uptime() {
	double uptime;
	auto status = uv_uptime(&uptime);
	UV_CHECK_ERROR("uv_uptime", status);
	return uptime;
}


//
//	OtOSClass::hostname
//

std::string OtOSClass::hostname() {
	char hostname[UV_MAXHOSTNAMESIZE];
	size_t length;
	auto status = uv_os_gethostname(hostname, &length);
	UV_CHECK_ERROR("uv_uptime", status);
	return std::string(hostname, length);
}


//
//	OtOSClass::totalMemory
//

long OtOSClass::totalMemory() {
	return uv_get_total_memory();
}


//
//	OtOSClass::freeMemory
//

long OtOSClass::freeMemory() {
	return uv_get_free_memory();
}


//
//	OtOSClass::cores
//

OtObject OtOSClass::cores() {
	uv_cpu_info_t* info;
	int count;
	auto status = uv_cpu_info(&info, &count);
	UV_CHECK_ERROR("uv_cpu_info", status);

	OtArray result = OtArrayClass::create();

	for (auto c = 0; c < count; c++) {
		OtDict core = OtDictClass::create();
		core->setEntry("model", OtStringClass::create(info[c].model));
		core->setEntry("speed", OtIntegerClass::create(info[c].speed));
		core->setEntry("user", OtIntegerClass::create(info[c].cpu_times.user));
		core->setEntry("nice", OtIntegerClass::create(info[c].cpu_times.nice));
		core->setEntry("sys", OtIntegerClass::create(info[c].cpu_times.sys));
		core->setEntry("idle", OtIntegerClass::create(info[c].cpu_times.idle));
		core->setEntry("irq", OtIntegerClass::create(info[c].cpu_times.irq));
		result->add(core);
	}

	uv_free_cpu_info(info, count);
	return result;
}


//
//	OtOSClass::networks
//

OtObject OtOSClass::networks() {
	uv_interface_address_t* info;
	int count;
	auto status = uv_interface_addresses(&info, &count);
	UV_CHECK_ERROR("uv_interface_addresses", status);

	OtArray result = OtArrayClass::create();
	char buffer[PATH_MAX];

	for (auto c = 0; c < count; c++) {
		OtDict network = OtDictClass::create();
		network->setEntry("name", OtStringClass::create(info[c].name));
		network->setEntry("internal", OtBooleanClass::create(info[c].is_internal));

		if (info[c].address.address4.sin_family == AF_INET) {
			status = uv_ip4_name(&info[c].address.address4, buffer, PATH_MAX);
			UV_CHECK_ERROR("uv_ip4_name", status);
			network->setEntry("address", OtStringClass::create(buffer));
			network->setEntry("family", OtStringClass::create("IPV4"));

		} else if (info[c].address.address4.sin_family == AF_INET6) {
			status = uv_ip6_name(&info[c].address.address6, buffer, PATH_MAX);
			UV_CHECK_ERROR("address6", status);
			network->setEntry("address", OtStringClass::create(buffer));
			network->setEntry("family", OtStringClass::create("IPV6"));
		}

		result->add(network);
	}

	uv_free_interface_addresses(info, count);
	return result;
}


//
//	OtOSClass::sleep
//

void OtOSClass::sleep(long milliseconds) {
	uv_sleep(milliseconds);
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

		type->set("sysname", OtFunctionClass::create(&OtOSClass::sysname));
		type->set("release", OtFunctionClass::create(&OtOSClass::release));
		type->set("version", OtFunctionClass::create(&OtOSClass::version));
		type->set("machine", OtFunctionClass::create(&OtOSClass::machine));
		type->set("uptime", OtFunctionClass::create(&OtOSClass::uptime));
		type->set("hostname", OtFunctionClass::create(&OtOSClass::hostname));

		type->set("cores", OtFunctionClass::create(&OtOSClass::cores));
		type->set("networks", OtFunctionClass::create(&OtOSClass::networks));

		type->set("totalMemory", OtFunctionClass::create(&OtOSClass::totalMemory));
		type->set("freeMemory", OtFunctionClass::create(&OtOSClass::freeMemory));

		type->set("sleep", OtFunctionClass::create(&OtOSClass::sleep));
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
