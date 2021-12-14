//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "system.h"


//
//	OtOS
//

class OtOSClass;
typedef std::shared_ptr<OtOSClass> OtOS;

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

	long totalMemory();
	long freeMemory();

	// get number of seconds since epoch
	double clock();

	// sleep for specified number of milliseconds
	void sleep(long milliseconds);

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

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtOS create();
};
