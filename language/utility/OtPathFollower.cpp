//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#include "OtPathFollower.h"


//
//	OtPathFollower::follow
//

void OtPathFollower::follow(const std::string& p, std::function<void()> cb) {
	// cleanup first (if required)
	if (following) {
		unfollow();
	}

	// remember properties
	path = p;
	callback = cb;

	// get last update time for file
	lastUpdateTime = std::filesystem::last_write_time(path);

	// create a new event handle
	fsEventHandle = new uv_fs_event_t;
	fsEventHandle->data = static_cast<void*>(this);

	int status = uv_fs_event_init(uv_default_loop(), fsEventHandle);
	UV_CHECK_ERROR("uv_fs_event_init", status);

	status = uv_fs_event_start(
		fsEventHandle,
		[](uv_fs_event_t* handle, const char* /* filename */, int events, int /* status */) {
			// get pointer to follower object
			auto follower = (OtPathFollower*) handle->data;

			// we need to track file version dates ourselves as libuv sometimes
			// calls us twice for the same file change
			std::filesystem::file_time_type ftime = std::filesystem::last_write_time(follower->path);

			// call callback (if required)
			if (follower->callback && (events & UV_CHANGE) && (ftime > follower->lastUpdateTime)) {
				follower->lastUpdateTime = ftime;
				follower->callback();
			}
		},
		path.c_str(),
		0);

	following = true;
}


//
//	OtPathFollower::unfollow
//

void OtPathFollower::unfollow() {
	if (following) {
		int status = uv_fs_event_stop(fsEventHandle);
		UV_CHECK_ERROR("uv_fs_event_stop", status);

		uv_close((uv_handle_t*) fsEventHandle, [](uv_handle_t* handle) {
			delete (uv_fs_event_t*) handle;
		});

		path.clear();
		callback = nullptr;
		following = false;
	}
}
