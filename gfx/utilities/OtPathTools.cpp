//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtPathTools.h"


//
//	OtPathFollower::follow
//

void OtPathFollower::follow(const std::string& p, std::function<void()> cb) {
	// cleaup first (if required)
	if (following) {
		unfollow();
	}

	// remember properties
	path = p;
	callback = cb;

	// create a new event handle
	fsEventHandle = new uv_fs_event_t;
	fsEventHandle->data = (void*) this;

	int status = uv_fs_event_init(uv_default_loop(), fsEventHandle);
	UV_CHECK_ERROR("uv_fs_event_init", status);

	status = uv_fs_event_start(
		fsEventHandle,
		[](uv_fs_event_t* handle, const char* filename, int events, int status) {
			// call callback (if required)
			auto follower = (OtPathFollower*) handle->data;

			if (follower->callback && events & UV_CHANGE) {
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
			auto follower = (OtPathFollower*) handle->data;
			delete (uv_fs_event_t*) handle;
			follower->fsEventHandle = nullptr;
		});

		path.clear();
		callback = nullptr;
		following = false;
	}
}
