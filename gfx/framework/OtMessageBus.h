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
#include <queue>
#include <string>
#include <vector>

#include <OtSingleton.h>


//
//	OtMessageBus
//

class OtMessageBus : OtSingleton<OtMessageBus> {
public:
	// lister for messages on the bus
	static inline void listen(std::function<void (const std::string&)> listener) {
		instance().listeners.push_back(listener);
	}

	// send a message to all the listeners
	static inline void send(const std::string& message) {
		instance().messages.push(message);
	}

	// process all the messages (by sending them to all the listeners)
	static inline void process() {
		auto& bus = instance();

		while (!bus.messages.empty()) {
			for (auto& listener : bus.listeners) {
				listener(bus.messages.front());
			}

			bus.messages.pop();
		}
	}

	// clear the bus
	static inline void clear() {
		auto& bus = instance();
		bus.listeners.clear();
		bus.messages = {};
	}

private:
	// list of bus listeners
	std::vector<std::function<void (const std::string&)>> listeners;

	// list of message to be dispatched
	std::queue<std::string> messages;
};
