//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
	// clear queue
	inline void clear() {
		std::unique_lock<std::mutex> mlock(mutex);
		queue = {};
	}

	// put item on queue
	inline void push(const T& item) {
		std::unique_lock<std::mutex> mlock(mutex);
		queue.push(item);
		mlock.unlock();
		cv.notify_one();
	}

	// return number of queue entries
	inline size_t size() {
		std::unique_lock<std::mutex> mlock(mutex);
		return queue.size();
	}

	// see if queue has entries
	inline bool empty() {
		std::unique_lock<std::mutex> mlock(mutex);
		return queue.empty();
	}

	// wait until queue is not empty
	inline void wait() {
		std::unique_lock<std::mutex> mlock(mutex);

		while (queue.empty()) {
			cv.wait(mlock);
		}
	}

	// pop item from queue and wait if queue is empty
	inline T pop() {
		std::unique_lock<std::mutex> mlock(mutex);

		while (queue.empty()) {
			cv.wait(mlock);
		}

		auto item = queue.front();
		queue.pop();
		return item;
	}

private:
	std::queue<T> queue;
	std::mutex mutex;
	std::condition_variable cv;
};
