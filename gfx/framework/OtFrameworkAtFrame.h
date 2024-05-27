//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>
#include <vector>

#include "OtSingleton.h"


//
//	OtFrameworkAtFrame
//

class OtFrameworkAtFrame : public OtSingleton<OtFrameworkAtFrame> {
public:
	// register function to be called at specified frame
	inline void add(uint32_t frame, std::function<void(void)> callback) {
		callbacks.emplace_back(frame, callback);
	}

	// run all functions that are targeting this frame
	inline void run(uint32_t frame) {
		for (auto i = callbacks.begin(); i < callbacks.end();) {
			if (i->frame == frame) {
				i->callback();
				i = callbacks.erase(i);

			} else {
				i++;
			}
		}
	}

private:
	// callbacks
	struct Callback {
		Callback(uint32_t f, std::function<void(void)> c) : frame(f), callback(c) {}
		uint32_t frame;
		std::function<void(void)> callback;
	};

	std::vector<Callback> callbacks;
};
