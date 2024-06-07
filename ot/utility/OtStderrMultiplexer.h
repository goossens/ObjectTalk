//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>

#include "OtException.h"
#include "OtSingleton.h"


//
//	OtStderrMultiplexer
//

class OtStderrMultiplexer : public OtSingleton<OtStderrMultiplexer> {
public:
	// multiplex information over the stderr stream
	void multiplex(int type, const std::string& message);
	void multiplex(OtException& exception);

	// demultiplex stderr stream
	void demuliplex(
		std::string input,
		std::function<void(const std::string& message)> normal,
		std::function<void(int type, const std::string& message)> log,
		std::function<void(OtException& e)> except);

private:
	// buffer to collect multiplexed data
	std::string buffer;
	bool inMessage = false;

	// process multiplexed data and call callbacks
	void process(
		std::function<void(int type, const std::string& message)> log,
		std::function<void(OtException& e)> except);
};
