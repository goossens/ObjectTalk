//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <iostream>

#include "OtException.h"
#include "OtStderrMultiplexer.h"


//
//	OtStderrMultiplexer::demuliplexInput
//

void OtStderrMultiplexer::demuliplexInput(
	std::string input,
	std::function<void(const std::string& message)> normal,
	std::function<void(OtLog::Type type, const std::string& message)> log,
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
				process(log, except);

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
					process(log, except);

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
	std::function<void(OtException& e)> except) {

	if (buffer[0] == 0) {
		// deserialize the log message and report
		log(static_cast<OtLog::Type>(buffer[1]), buffer.substr(2));

	} else if (buffer[0] == 1) {
		// deserialize the exception and report
		OtException exception;
		exception.deserialize(buffer.substr(1));
		except(exception);
	}

	buffer.clear();
}
