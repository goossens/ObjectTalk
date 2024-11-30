//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <fstream>
#include <string>

#include "fmt/format.h"

#include "OtException.h"
#include "OtSingleton.h"


//
//	OtLogger
//

class OtLogger : OtSingleton<OtLogger> {
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
	static inline void log(const char* filename, int lineno, int type, const std::string& message) {
		instance().logMessage(filename, lineno, type, message);
	}

	template<typename... ARGS>
	static inline void log(const char* filename, int lineno, int type, const char* format, ARGS... args) {
		auto message = fmt::format(format, args...);
		instance().logMessage(filename, lineno, type, message);
	}

	// set logging targets
	static inline void stderrLogging(bool flag) { instance().logToStderr = flag; }

	static inline void fileLogging(const std::string& name) {
		auto& ofs = instance().ofs;
		if (ofs.is_open()) { ofs.close(); }
		if (!name.empty()) { ofs.open(name); }
	}

private:
	// log the message
	void logMessage(const char* filename, int lineno, int type, const std::string& message);

	// logging targets
	bool logToStderr = true;
	std::ofstream ofs;
};


//
//	Macros
//

#define OtLog(type, ...) (OtLogger::log(__FILE__, __LINE__, type, __VA_ARGS__))

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
