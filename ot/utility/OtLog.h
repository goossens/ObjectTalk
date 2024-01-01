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

#define OtLog(type, message) (OtLogger::instance()->log(__FILE__, __LINE__, type, message))

#if OT_DEBUG
#define OtLogDebug(message) OtLog(OtLogger::debug, message)
#define OtLogInfo(message) OtLog(OtLogger::info, message)
#define OtLogWarning(message) OtLog(OtLogger::warning, message)
#else
#define OtLogDebug(message)
#define OtLogInfo(message)
#define OtLogWarning(message)
#endif

#define OtLogError(message) OtLog(OtLogger::error, message)
#define OtLogFatal(message) OtLog(OtLogger::fatal, message)
