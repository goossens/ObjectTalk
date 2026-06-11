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

#include "BS_thread_pool.hpp"

#include "OtSingleton.h"


//
//	OtThreadPool
//

class OtThreadPool : OtSingleton<OtThreadPool> {
public:
	// submit a task to run in thread and return a std::future
	template <typename T>
	static std::future<T> submit(std::function<T()> task) {
		return instance().threadpool.submit_task(task);
	}

	// run a task in a thread
	static void run(std::function<void()> task) {
		instance().threadpool.detach_task(task);
	}

	// wait for all tasks to be finished
	static void wait() {
		instance().threadpool.wait();
	}

private:
	// the actual threadpool
	BS::thread_pool<> threadpool;
};
