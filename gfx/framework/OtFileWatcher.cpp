//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFileWatcher.h"


//
//	OtFileWatcher::follow
//

void OtFileWatcher::follow(const std::string& p, std::function<void(Event)> cb) {
	// remember properties
	path = p;
	callback = cb;

	// see if file exists
	exists = std::filesystem::exists(path);

	// get last update time for file
	if (exists) {
		lastUpdateTime = std::filesystem::last_write_time(path);
	}

	// set state
	refreshTime = std::chrono::system_clock::now() + std::chrono::seconds(1);

	if (!following) {
		watchers.emplace_back(this);
		following = true;
	}
}


//
//	OtFileWatcher::unfollow
//

void OtFileWatcher::unfollow() {
	if (following) {
		following = false;
		std::erase(watchers, this);
	}
}


//
//	OtFileWatcher::run
//

void OtFileWatcher::run() {
	auto now = std::chrono::system_clock::now();

	for (auto watcher : watchers) {
		if (now > watcher->refreshTime) {
			watcher->check();
			watcher->refreshTime = now + std::chrono::seconds(1);
		}
	}
}


//
//	OtFileWatcher::check
//

void OtFileWatcher::check() {
	if (exists) {
		if (!std::filesystem::exists(path)) {
			callback(Event::deleted);
			exists = false;

		} else {
			auto updateTime = std::filesystem::last_write_time(path);

			if (updateTime > lastUpdateTime) {
				callback(Event::updated);
				lastUpdateTime = updateTime;
			}
		}

	} else {
		if (std::filesystem::exists(path)) {
			callback(Event::created);
			exists = true;
		}
	}
}
