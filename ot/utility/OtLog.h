//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <fstream>
#include <string>

#include "fmt/format.h"

#include "OtSingleton.h"


//
//	OtLogger
//

class OtLogger : public OtSingleton<OtLogger> {
public:
	// log entry types
	enum {
		debug,
		info,
		warning,
		error,
		fatal
	};

	// log a message
	void log(const char* filename, int lineno, int type, const std::string& message);

	template<typename... ARGS>
	void log(const char* filename, int lineno, int type, const char* format, ARGS... args) {
		auto message = fmt::format(format, args...);
		log(filename, lineno, type, message);
	}

	// set options
	void setSubprocessMode(bool flag) { subprocessMode = flag; }

	// set logging targets
	void stderrLogging(bool flag) { logToStderr = flag; }
	void fileLogging(const std::filesystem::path& path);

private:
	// configuration
	bool subprocessMode = false;

	// logging targets
	bool logToStderr = true;
	std::ofstream ofs;
};


//
//	Macros
//

#define OtLog(type, ...) (OtLogger::instance()->log(__FILE__, __LINE__, type, __VA_ARGS__))

#if OT_DEBUG
#define OtLogDebug(...) OtLog(OtLogger::debug, __VA_ARGS__)
#define OtLogInfo(...) OtLog(OtLogger::info, __VA_ARGS__)
#define OtLogWarning(...) OtLog(OtLogger::warning, __VA_ARGS__)
#else
#define OtLogDebug(...)
#define OtLogInfo(...)
#define OtLogWarning(...)
#endif

#define OtLogError(...) OtLog(OtLogger::error, __VA_ARGS__)
#define OtLogFatal(...) OtLog(OtLogger::fatal, __VA_ARGS__)
