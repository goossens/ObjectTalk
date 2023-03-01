//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtSystem.h"


//
//	OtOS
//

class OtOSClass;
using OtOS = OtObjectPointer<OtOSClass>;

class OtOSClass : public OtSystemClass {
public:
	// see if environment variable exists
	bool hasenv(const std::string& name);

	// get environment variable
	std::string getenv(const std::string& name);

	// set environment variable
	void setenv(const std::string& name, const std::string& value);

	// unset environment variable
	void unsetenv(const std::string& name);

	// get system information
	std::string sysname();
	std::string release();
	std::string version();
	std::string machine();
	double uptime();
	std::string hostname();

	OtObject cores();
	OtObject networks();

	int64_t totalMemory();
	int64_t freeMemory();

	// generate a UUID
	std::string uuid();

	// get number of seconds since epoch
	double clock();

	// sleep for specified number of milliseconds
	void sleep(int64_t milliseconds);

	// get parts of the current datetime
	int getYear();
	int getMonth();
	int getDay();
	int getHours();
	int getMinutes();
	int getSeconds();
	int getDayOfWeek();
	int getDayOfYear();
	bool isDST();

	// handle event loop for servers
	void runServer();
	void stopServer();

	// get type definition
	static OtType getMeta();
};
