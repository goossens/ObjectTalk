//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#if _WIN32
#include <intrin.h>
#else
#include <signal.h>
#endif

#include "OtException.h"
#include "OtLog.h"
#include "OtPathTools.h"


//
//	Message types
//

static const char* types[] = {
	"debug",
	"info",
	"warning",
	"error",
	"fatal"
};


//
//	GetTimestamp
//

std::string GetTimestamp() {
	// get current time
	auto now = std::chrono::system_clock::now();
	auto nowTime = std::chrono::system_clock::to_time_t(now);
	auto localTime = localtime(&nowTime);

	// convert to a string
	char buffer[256];
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H-%M-%S", localTime);

	// add milliseconds
	int ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now).time_since_epoch().count() % 1000;
	snprintf(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer), ".%03d", ms);

	// return result
	return std::string(buffer);
}


//
//	OtLogger::log
//

void OtLogger::log(const char* filename, int lineno, int type, const std::string& message) {
	// get timestamp, filename and message type
	auto timestamp = GetTimestamp();
	auto shortname = OtPathGetFilename(filename);
	auto messageType = types[type];

	// send to STDERR (if required)
	if (logToStderr) {
		std::cerr << timestamp << " [" << messageType << "] " << shortname << " (" << lineno << "): " << message << std::endl;
	}

	// send to log file (if required)
	if (ofs.is_open()) {
		ofs << timestamp << " [" << messageType << "] " << shortname << "(" << lineno << "): " << message << std::endl;
	}

	// throw exception or terminate program (if required)
	if (type == error) {
		throw OtException(message);

	} else if (type == fatal) {

#if OT_DEBUG

#if _WIN32
	__debugbreak();
#else
	raise(SIGTRAP);
#endif

#else
		std::_Exit(EXIT_FAILURE);
#endif

	}
}


//
//	OtLogger::fileLogging
//

void OtLogger::fileLogging(const std::string& name)
{
	if (ofs.is_open()) {
		ofs.close();
	}

	if (!name.empty()) {
		ofs.open(name);
	}
}
