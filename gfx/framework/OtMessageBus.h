//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <queue>
#include <string>
#include <vector>

#include <OtSingleton.h>


//
//	OtMessageBus
//

class OtMessageBus : public OtSingleton<OtMessageBus> {
public:
	// lister for messages on the bus
	void listen(std::function<void (const std::string&)> listener) {
		listeners.push_back(listener);
	}

	// send a message to all the listeners
	void send(const std::string& message) {
		messages.push(message);
	}

	// process all the messages (by sending them to all the listeners)
	void process() {
		while (!messages.empty()) {
			for (auto& listener : listeners) {
				listener(messages.front());
			}

			messages.pop();
		}
	}

	// clear the bus
	void clear() {
		listeners.clear();
		messages = {};
	}

private:
	// list of bus listeners
	std::vector<std::function<void (const std::string&)>> listeners;

	// list of message to be dispatched
	std::queue<std::string> messages;
};
