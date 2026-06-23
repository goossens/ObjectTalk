//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>


//
//	OtFileWatcher
//

class OtFileWatcher {
public:
	// destructor
	~OtFileWatcher() { unfollow(); }

	// possible file events
	enum class Event {
		created,
		updated,
		deleted
	};

	// follow a (new) file
	void follow(const std::string& path, std::function<void(Event)> callback);

	// unfollow the file
	void unfollow();

private:
	// properties
	std::string path;
	std::function<void(Event)> callback;
	bool exists;
	std::chrono::time_point<std::chrono::system_clock> refreshTime;
	std::filesystem::file_time_type lastUpdateTime;
	bool following = false;

	// engine
	friend class OtFramework;
	static void run();
	void check();
	static inline std::vector<OtFileWatcher*> watchers;
};
