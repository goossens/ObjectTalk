//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <functional>
#include <string>

#include "OtLibuv.h"


//
//	OtPathFollower
//

class OtPathFollower {
public:
	// constructor/destructor
	OtPathFollower() = default;

	OtPathFollower(const std::string& path, std::function<void()> callback) {
		follow(path, callback);
	}

	~OtPathFollower() {
		unfollow();
	}

	// reset the follower
	void clear() {
		unfollow();
	}

	// follow a (new) path
	void follow(const std::string& path, std::function<void()> callback);

	// unfollow the path
	void unfollow();

private:
	// properties
	std::string path;
	std::function<void()> callback;
	uv_fs_event_t* fsEventHandle = nullptr;
	std::filesystem::file_time_type lastUpdateTime;
	bool following = false;
};
