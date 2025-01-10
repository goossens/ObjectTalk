//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
//	OtLog
//

class OtLog : OtSingleton<OtLog> {
public:
	// log entry types
	enum class Type {
		debug,
		info,
		warning,
		error,
		fatal
	};

	// log a message
	static inline void log(const char* filename, int lineno, Type type, const std::string& message) {
		instance().logMessage(filename, lineno, type, message);
	}

	template<typename... ARGS>
	static inline void log(const char* filename, int lineno, Type type, const char* format, ARGS... args) {
		auto message = fmt::format(format, args...);
		instance().logMessage(filename, lineno, type, message);
	}

	// set logging targets
	static inline void setStderrLogging(bool flag) { instance().logToStderr = flag; }

	static inline void setFileLogging(const std::string& name) {
		auto& ofs = instance().ofs;
		if (ofs.is_open()) { ofs.close(); }
		if (!name.empty()) { ofs.open(name); }
	}

private:
	// log the message
	void logMessage(const char* filename, int lineno, Type type, const std::string& message);

	// logging targets
	bool logToStderr = true;
	std::ofstream ofs;
};


//
//	Macros
//

#define OtLogMessage(type, ...) (OtLog::log(__FILE__, __LINE__, type, __VA_ARGS__))

#if OT_DEBUG
#define OtLogDebug(...) OtLogMessage(OtLog::Type::debug, __VA_ARGS__)
#define OtLogInfo(...) OtLogMessage(OtLog::Type::info, __VA_ARGS__)
#define OtLogWarning(...) OtLogMessage(OtLog::Type::warning, __VA_ARGS__)
#else
#define OtLogDebug(...)
#define OtLogInfo(...)
#define OtLogWarning(...)
#endif

#define OtLogError(...) OtLogMessage(OtLog::Type::error, __VA_ARGS__)
#define OtLogFatal(...) OtLogMessage(OtLog::Type::fatal, __VA_ARGS__)
