//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <vector>

#include "OtAssert.h"
#include "OtText.h"

#include "OtAssetBase.h"
#include "OtPathTools.h"


//
//	OtAssetBase::~OtAssetBase
//

OtAssetBase::~OtAssetBase() {
	if (following) {
		unfollow();
	}

	OtAssert(references == 0);
}


//
//	OtAssetBase::getStateName
//

const char* OtAssetBase::getStateName() {
	static const char* names[] = {
		"Null",
		"Loading",
		"Missing",
		"Invalid",
		"Loaded",
		"Ready"
	};

	return names[assetState];
}


//
//	OtAssetBase::supportsFileType
//

bool OtAssetBase::supportsFileType(const std::string& ext) {
	std::vector<std::string> extensions;
	OtText::split(getSupportedFileTypes(), extensions, ',');
	return std::find(extensions.begin(), extensions.end(), ext) != extensions.end();
}


//
//	OtAssetBase::follow
//

void OtAssetBase::follow() {
	OtAssert(following == false);

	fsEventHandle = new uv_fs_event_t;
	fsEventHandle->data = (void*) this;

	int status = uv_fs_event_init(uv_default_loop(), fsEventHandle);
	UV_CHECK_ERROR("uv_fs_event_init", status);

	status = uv_fs_event_start(
		fsEventHandle,
		[](uv_fs_event_t* handle, const char* filename, int events, int status) {
			auto asset = (OtAssetBase*) handle->data;

			if (events & UV_CHANGE) {
				asset->changed();
			}

			if (events & UV_RENAME) {
				asset->renamed(filename);
			}
		},
		path.c_str(),
		0);

	following = true;
}


//
//	OtAssetBase::unfollow
//

void OtAssetBase::unfollow() {
	OtAssert(following == true);

	int status = uv_fs_event_stop(fsEventHandle);
	UV_CHECK_ERROR("uv_fs_event_stop", status);

	uv_close((uv_handle_t*) fsEventHandle, [](uv_handle_t* handle) {
		auto asset = (OtAssetBase*) handle->data;
		delete (uv_fs_event_t*) handle;
		asset->fsEventHandle = nullptr;
	});

	following = false;
}


//
//	OtAssetBase::initializeMissing
//

void OtAssetBase::initializeMissing(const std::string& p) {
	if (following) {
		unfollow();
	}

	path = p;
	assetState = OtAssetBase::missingState;
}


//
//	OtAssetBase::initializeInvalid
//

void OtAssetBase::initializeInvalid(const std::string& p) {
	if (following) {
		unfollow();
	}

	path = p;
	assetState = OtAssetBase::invalidState;
}


//
//	OtAssetBase::initializeReady
//

void OtAssetBase::initializeReady(const std::string& p) {
	if (following) {
		unfollow();
	}

	path = p;
	assetState = OtAssetBase::readyState;
}


//
//	OtAssetBase::preLoad
//

void OtAssetBase::preLoad(const std::string& p) {
	// unfollow previous assets if required
	if (following) {
		unfollow();
	}

	// remember path and set new state
	path = p;
	assetState = OtAssetBase::loadingState;

	// notify followers
	publisher.preLoad();

	// set a callback to catch load completion
	// we need async here because loading happens in a dfferent thread
	loaderEventHandle = new uv_async_t;
	loaderEventHandle->data = this;

	auto status = uv_async_init(uv_default_loop(), loaderEventHandle, [](uv_async_t* handle){
		// were we succesful?
		auto asset = (OtAssetBase*) handle->data;

		if (asset->isReady()) {
			// yes, notify followers and follow asset
			asset->publisher.postLoad();
			asset->follow();
		}

		// cleanup
		uv_close((uv_handle_t*) asset->loaderEventHandle, [](uv_handle_t* handle) {
			auto asset = (OtAssetBase*) handle->data;
			delete (uv_fs_event_t*) handle;
			asset->loaderEventHandle = nullptr;
		});
	});

	UV_CHECK_ERROR("uv_async_init", status);
}


//
//	OtAssetBase::postLoad
//

void OtAssetBase::postLoad(AssetState state) {
	// given that the asset manager loads stuff in a different thread,
	// we just let callback above handle notification in main thread
	assetState = state;

	// send notification if the asset is now completely ready (i.e. is not postprocessing)
	if (assetState == readyState) {
		auto status = uv_async_send(loaderEventHandle);
		UV_CHECK_ERROR("uv_async_send", status);
	}
}


//
//	OtAssetBase::preSave
//

void OtAssetBase::preSave() {
	publisher.preSave();
}

void OtAssetBase::preSave(const std::string& p) {
	if (path != p) {
		if (following) {
			unfollow();
		}

		follow();
	}

	path = p;
	publisher.preSave();
}


//
//	OtAssetBase::postSave
//

void OtAssetBase::postSave() {
	publisher.postSave();
}


//
//	OtAssetBase::changed
//

void OtAssetBase::changed() {
	publisher.changed();
}


//
//	OtAssetBase::renamed
//

void OtAssetBase::renamed(const std::string& path) {
	publisher.renamed(path);
}
