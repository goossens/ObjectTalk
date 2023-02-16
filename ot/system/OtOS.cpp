//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <chrono>
#include <ctime>
#include <random>

#include "OtArray.h"
#include "OtDict.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtLibuv.h"
#include "OtOS.h"


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
	size_t length = UV_MAXHOSTNAMESIZE;
	auto status = uv_os_gethostname(hostname, &length);
	UV_CHECK_ERROR("uv_os_gethostname", status);
	return std::string(hostname, length);
}


//
//	OtOSClass::totalMemory
//

int64_t OtOSClass::totalMemory() {
	return uv_get_total_memory();
}


//
//	OtOSClass::freeMemory
//

int64_t OtOSClass::freeMemory() {
	return uv_get_free_memory();
}


//
//	OtOSClass::uuid
//

std::string OtOSClass::uuid() {
	static std::random_device device;
	static std::mt19937 generator(device());
	static std::uniform_int_distribution<uint64_t> distribution(0, 15);

	const char* digits = "0123456789abcdef";
	std:: string result;

	for (auto dash : { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 }) {
		if (dash) {
			result += "-";
		}

		result += digits[distribution(generator)];
		result += digits[distribution(generator)];
	}

	return result;
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
		result->append(core);
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
	char buffer[256];

	for (auto c = 0; c < count; c++) {
		OtDict network = OtDictClass::create();
		network->setEntry("name", OtStringClass::create(info[c].name));
		network->setEntry("internal", OtBooleanClass::create(info[c].is_internal));

		if (info[c].address.address4.sin_family == AF_INET) {
			status = uv_ip4_name(&info[c].address.address4, buffer, 256);
			UV_CHECK_ERROR("uv_ip4_name", status);
			network->setEntry("address", OtStringClass::create(buffer));
			network->setEntry("family", OtStringClass::create("IPV4"));

		} else if (info[c].address.address4.sin_family == AF_INET6) {
			status = uv_ip6_name(&info[c].address.address6, buffer, 256);
			UV_CHECK_ERROR("address6", status);
			network->setEntry("address", OtStringClass::create(buffer));
			network->setEntry("family", OtStringClass::create("IPV6"));
		}

		result->append(network);
	}

	uv_free_interface_addresses(info, count);
	return result;
}


//
//	OtOSClass::clock
//

double OtOSClass::clock() {
	return (double) std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count() / 1000.0;
}


//
//	OtOSClass::sleep
//

void OtOSClass::sleep(int64_t milliseconds) {
	uv_sleep((unsigned int) milliseconds);
}


//
//	OtOSClass::getYear
//

int OtOSClass::getYear() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_year;
}


//
//	OtOSClass::getMonth
//

int OtOSClass::getMonth() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_mon;
}


//
//	OtOSClass::getDay
//

int OtOSClass::getDay() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_mday;
}


//
//	OtOSClass::getHours
//

int OtOSClass::getHours() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_hour;
}


//
//	OtOSClass::getMinutes
//

int OtOSClass::getMinutes() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_min;
}


//
//	OtOSClass::getSeconds
//

int OtOSClass::getSeconds() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_sec;
}


//
//	OtOSClass::getDayOfWeek
//

int OtOSClass::getDayOfWeek() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_wday;
}


//
//	OtOSClass::getDayOfYear
//

int OtOSClass::getDayOfYear() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_yday;
}


//
//	OtOSClass::isDST
//

bool OtOSClass::isDST() {
	time_t rawtime;
	time(&rawtime);
	struct tm* timeinfo = localtime(&rawtime);
	return timeinfo->tm_isdst > 0;
}


//
//	OtOSClass::registerServer
//

void OtOSClass::registerServer(
		std::function<void(void)> run,
		std::function<void(OtException e)> error,
		std::function<void(void)> stop) {
	serverRunner = run;
	serverError = error;
	serverStopper = stop;
}


//
//	OtOSClass::runServer
//

void OtOSClass::runServer() {
	if (serverRunner) {
		serverRunner();

	} else {
		OtExcept("This platform does not support servers");
	}
}


//
//	OtOSClass::errorServer
//

void OtOSClass::errorServer(OtException e) {
	if (serverError) {
		serverError(e);

	} else {
		OtExcept("This platform does not support servers");
	}
}


//
//	OtOSClass::stopServer
//

void OtOSClass::stopServer() {
	if (serverStopper) {
		serverStopper();

	} else {
		OtExcept("This platform does not support servers");
	}
}


//
//	OtOSClass::registerGUI
//

void OtOSClass::registerGUI(
		std::function<void(void)> run,
		std::function<void(OtException e)> error,
		std::function<void(void)> stop) {
	guiRunner = run;
	guiError = error;
	guiStopper = stop;
}


//
//	OtOSClass::runGUI
//

void OtOSClass::runGUI() {
	if (guiRunner) {
		guiRunner();

	} else {
		OtExcept("This platform does not have a GUI module");
	}
}


//
//	OtOSClass::errorGUI() {
//

void OtOSClass::errorGUI(OtException e) {
	if (guiError) {
		guiError(e);

	} else {
		OtExcept("This platform does not have a GUI module");
	}
}


//
//	OtOSClass::stopGUI
//

void OtOSClass::stopGUI() {
	if (guiStopper) {
		guiStopper();

	} else {
		OtExcept("This platform does not have a GUI module");
	}
}


//
//	OtOSClass::getMeta
//

OtType OtOSClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtOSClass>(
			"OS",
			OtSystemClass::getMeta(),
			[]() {
				return (OtObject) OtOSClass::instance();
			});

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
		type->set("uuid", OtFunctionClass::create(&OtOSClass::hostname));

		type->set("cores", OtFunctionClass::create(&OtOSClass::cores));
		type->set("networks", OtFunctionClass::create(&OtOSClass::networks));

		type->set("totalMemory", OtFunctionClass::create(&OtOSClass::totalMemory));
		type->set("freeMemory", OtFunctionClass::create(&OtOSClass::freeMemory));

		type->set("clock", OtFunctionClass::create(&OtOSClass::clock));
		type->set("sleep", OtFunctionClass::create(&OtOSClass::sleep));

		type->set("getYear", OtFunctionClass::create(&OtOSClass::getYear));
		type->set("getMonth", OtFunctionClass::create(&OtOSClass::getMonth));
		type->set("getDay", OtFunctionClass::create(&OtOSClass::getDay));
		type->set("getHours", OtFunctionClass::create(&OtOSClass::getHours));
		type->set("getMinutes", OtFunctionClass::create(&OtOSClass::getMinutes));
		type->set("getSeconds", OtFunctionClass::create(&OtOSClass::getSeconds));
		type->set("getDayOfWeek", OtFunctionClass::create(&OtOSClass::getDayOfWeek));
		type->set("getDayOfYear", OtFunctionClass::create(&OtOSClass::getDayOfYear));
		type->set("isDST", OtFunctionClass::create(&OtOSClass::isDST));

		type->set("runServer", OtFunctionClass::create(&OtOSClass::runServer));
		type->set("stopServer", OtFunctionClass::create(&OtOSClass::stopServer));

		type->set("runGUI", OtFunctionClass::create(&OtOSClass::runGUI));
		type->set("stopGUI", OtFunctionClass::create(&OtOSClass::stopGUI));
	}

	return type;
}


//
//	OtOSClass::create
//

OtOS OtOSClass::create() {
	return OtOSClass::instance();
}
