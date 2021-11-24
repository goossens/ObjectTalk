//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <condition_variable>
#include <mutex>
#include <queue>


//
//	OtConcurrentQueue
//

template <typename T>
class OtConcurrentQueue {
public:
	// constructor
	OtConcurrentQueue() = default;

	// clear queue
	void clear() {
		std::unique_lock<std::mutex> mlock(mutex);
		queue = {};
		mlock.unlock();
		cv.notify_one();
	}

	// put item on queue
	void push(const T& item) {
		std::unique_lock<std::mutex> mlock(mutex);
		queue.push(item);
		mlock.unlock();
		cv.notify_one();
	}

	// see if queue has entries
	bool empty() {
		std::unique_lock<std::mutex> mlock(mutex);
		auto empty = queue.empty();
		mlock.unlock();
		return empty;
	}

	// pop item from queue and wait if queue is empty
	T pop() {
		std::unique_lock<std::mutex> mlock(mutex);

		while (queue.empty()) {
			cv.wait(mlock);
		}

		auto val = queue.front();
		queue.pop();
		mlock.unlock();
		cv.notify_one();
		return val;
	}

private:
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable cv;
};
