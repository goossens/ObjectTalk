//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	static void multiplex(OtLog::Type type, const std::string& message);
	static inline void multiplex(const std::string& message) { send(MessageType::debuggerMessage, message); }
	static inline void multiplex(OtException& exception) { send(MessageType::exceptionMessage, exception.serialize()); }

	// de-multiplex stderr stream
	static inline void deMultiplex(
		std::string input,
		std::function<void(const std::string& message)> normal,
		std::function<void(OtLog::Type type, const std::string& message)> log,
		std::function<void(const std::string& message)> debugger,
		std::function<void(OtException& e)> except) {

		instance().deMultiplexInput(input, normal, log, debugger, except);
	}

private:
	// message types
	enum class MessageType {
		logMessage,
		debuggerMessage,
		exceptionMessage
	};

	// buffer to collect multiplexed data
	std::string buffer;
	bool inMessage = false;

	// send multiplexed line on stderr
	static void send(MessageType type, const std::string& message) {
		std::cerr << '\x02' << static_cast<char>(type) << message << '\x03' << std::flush;
	}

	// de-multiplex stderr stream
	void deMultiplexInput(
		std::string input,
		std::function<void(const std::string& message)> normal,
		std::function<void(OtLog::Type type, const std::string& message)> log,
		std::function<void(const std::string& message)> debugger,
		std::function<void(OtException& e)> except);

	// process multiplexed data and call callbacks
	void process(
		std::function<void(OtLog::Type type, const std::string& message)> log,
		std::function<void(const std::string& message)> debugger,
		std::function<void(OtException& e)> except);
};
