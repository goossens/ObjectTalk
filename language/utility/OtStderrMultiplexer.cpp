//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>

#include "nlohmann/json.hpp"
#include "OtException.h"
#include "OtStderrMultiplexer.h"


//
//	OtStderrMultiplexer::multiplex
//

void OtStderrMultiplexer::multiplex(OtLog::Type type, const std::string& message) {
	nlohmann::json json = {
		{"type", type},
		{"message", message}
	};

	send(MessageType::logMessage, json.dump());
}


//
//	OtStderrMultiplexer::demuliplexInput
//

void OtStderrMultiplexer::demuliplexInput(
	std::string input,
	std::function<void(const std::string& message)> normal,
	std::function<void(OtLog::Type type, const std::string& message)> log,
	std::function<void(const std::string& message)> debugger,
	std::function<void(OtException& e)> except) {

	// process entire input
	while (input.size()) {
		if (inMessage) {
			// see if we have the rest of the message
			auto etx = input.find('\x03');

			if (etx != std::string::npos) {
				// add to buffer and process it
				buffer += input.substr(0, etx);
				input = input.substr(etx + 1);
				inMessage = false;
				process(log, debugger, except);

			} else {
				// add chunk and wait for the end of the message
				buffer += input;
				input.clear();
			}

		} else {
			// see if we have the start of a new message
			auto stx = input.find('\x02');

			if (stx != std::string::npos) {
				// send first part as normal text
				normal(input.substr(0, stx));

				// see if we also have an ETX in the same chunk
				auto etx = input.find('\x03');

				if (etx != std::string::npos) {
					// extract the message and process it
					buffer = input.substr(stx + 1, etx - stx - 1);
					input = input.substr(etx + 1);
					process(log, debugger, except);

				} else {
					// extract the partial message
					buffer = input.substr(stx + 1);
					inMessage = true;
					input.clear();
				}

			} else {
				normal(input);
				input.clear();
			}
		}
	}
}


//
//	OtStderrMultiplexer::process
//

void OtStderrMultiplexer::process(
	std::function<void(OtLog::Type type, const std::string& message)> log,
	std::function<void(const std::string& message)> debugger,
	std::function<void(OtException& e)> except) {

	MessageType type = static_cast<MessageType>(buffer[0]);
	auto message = buffer.substr(1);

	if (type == MessageType::logMessage) {
		// deserialize the log message and report (if required)
		if (log) {
			auto json = nlohmann::json::parse(message);
			OtLog::Type type = json.value("type", OtLog::Type::debug);
			std::string logMessage = json.value("message", "");
			log(type, logMessage);
		}

	} else if (type == MessageType::debuggerMessage) {
		// report (if required)
		if (debugger) {
			debugger(message);
		}

	} else if (type == MessageType::exceptionMessage) {
		// deserialize the exception and report (if required)
		if (except) {
			OtException exception;
			exception.deserialize(message);
			except(exception);
		}
	}

	buffer.clear();
}
