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
#include <vector>

#include "OtSingleton.h"


//
//	OtFrameworkAtExit
//

class OtFrameworkAtExit : OtSingleton<OtFrameworkAtExit> {
public:
	// register function to be called at exit
	static inline void add(std::function<void(void)> callback) {
		instance().callbacks.push_back(callback);
	}

	// run all exit functions
	static inline void run() {
		for (auto& callback : instance().callbacks) {
			callback();
		}
	}

private:
	// callbacks
	std::vector<std::function<void(void)>> callbacks;
};
