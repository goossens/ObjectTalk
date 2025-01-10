//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <iostream>
#include <string>

#include "OtException.h"
#include "OtLog.h"
#include "OtSingleton.h"


//
//	OtStderrMultiplexer
//

class OtStderrMultiplexer : OtSingleton<OtStderrMultiplexer> {
public:
	// multiplex information over the stderr stream
	static inline void multiplex(OtLog::Type type, const std::string& message) {
		std::cerr << '\x02' << '\x00' << static_cast<char>(type) << message << '\x03' << std::flush;
	}

	static inline void multiplex(OtException& exception) {
		std::cerr << '\x02' << '\x01' << exception.serialize() << '\x03' << std::flush;
	}

	// demultiplex stderr stream
	static inline void demuliplex(
		std::string input,
		std::function<void(const std::string& message)> normal,
		std::function<void(OtLog::Type type, const std::string& message)> log,
		std::function<void(OtException& e)> except) { instance().demuliplexInput(input, normal, log, except); }

private:
	// buffer to collect multiplexed data
	std::string buffer;
	bool inMessage = false;

	// demultiplex stderr stream
	void demuliplexInput(
		std::string input,
		std::function<void(const std::string& message)> normal,
		std::function<void(OtLog::Type type, const std::string& message)> log,
		std::function<void(OtException& e)> except);

	// process multiplexed data and call callbacks
	void process(
		std::function<void(OtLog::Type type, const std::string& message)> log,
		std::function<void(OtException& e)> except);
};
