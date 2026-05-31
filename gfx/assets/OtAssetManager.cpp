//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "imgui.h"

#include "OtAssert.h"
#include "OtPath.h"

#include "OtAssetManager.h"
#include "OtThreadPool.h"
#include "OtUi.h"


//
//	OtAssetManager::~OtAssetManager
//

OtAssetManager::~OtAssetManager() {
	// sanity check
	OtAssert(assets.size() == 0);
}


//
//	OtAssetManager::startManager
//

void OtAssetManager::startManager() {
	// start the cleanup timer
	int status = uv_timer_init(uv_default_loop(), &cleanupTimerHandle);
	UV_CHECK_ERROR("uv_timer_init", status);
	cleanupTimerHandle.data = this;

	status = uv_timer_start(&cleanupTimerHandle, [](uv_timer_t* handle) {
		auto manager = (OtAssetManager*) handle->data;
		manager->clearUnusedAssets();
	}, 10000, 10000);

	UV_CHECK_ERROR("uv_timer_start", status);
}


//
//	OtAssetManager::stopManager
//

void OtAssetManager::stopManager() {
	// stop the cleanup timer
	int status = uv_timer_stop(&cleanupTimerHandle);
	UV_CHECK_ERROR("uv_timer_stop", status);

	uv_close((uv_handle_t*) &cleanupTimerHandle, nullptr);

	// first clear all unused assets
	clearUnusedAssets();

	// clear all the remaining assets
	for (const auto& [path, asset] : assets) {
		delete asset;
	}

	assets.clear();
}


//
//	OtAssetManager::renderManagerUI
//

void OtAssetManager::renderManagerUI() {
	// show spinner if we are currently loading assets
	if (loading != 0) {
		auto center = ImGui::GetWindowPos() + ImGui::GetWindowSize() / 2.0f;
		OtUi::spinner(center);
	}
}


//
//	OtAssetManager::scheduleLoad
//

void OtAssetManager::scheduleLoad(OtAssetBase* asset) {
	// sanity check
	OtAssert(asset->loaderEventHandle == nullptr);

	// set a callback to catch load completion
	asset->loaderEventHandle = new uv_async_t;
	asset->loaderEventHandle->data = asset;

	auto status = uv_async_init(uv_default_loop(), asset->loaderEventHandle, [](uv_async_t* handle){
		// were we succesful?
		auto asset = (OtAssetBase*) handle->data;

		if (asset->isReady()) {
			// yes, notify subscribers
			asset->publisher.changed();
		}

		// cleanup
		uv_close((uv_handle_t*) asset->loaderEventHandle, [](uv_handle_t* handle) {
			auto asset = (OtAssetBase*) handle->data;
			delete (uv_fs_event_t*) handle;
			asset->loaderEventHandle = nullptr;
		});
	});

	UV_CHECK_ERROR("uv_async_init", status);

	// schedule asset for loading
	asset->state = OtAssetBase::State::loading;
	loading++;

	OtThreadPool::run([this, asset]() {
		asset->errorMessage.clear();
		asset->state = asset->load();
		loading--;
		auto status = uv_async_send(asset->loaderEventHandle);
		UV_CHECK_ERROR("uv_async_send", status);
	});
}


//
//	OtAssetManager::clearUnusedAssets
//

void OtAssetManager::clearUnusedAssets() {
	// possibly need to loop multiple times due to asset dependencies
	size_t lastCleared;
	size_t cleared = 0;

	do {
		lastCleared = cleared;
		cleared = 0;

		for (auto it = assets.begin(); it != assets.end();) {
			auto asset = it->second;

			if (asset->getReferences() == 0 && !asset->isLoading()) {
				delete asset;
				it = assets.erase(it);
				cleared++;

			} else {
				it++;
			}
		}
	} while (cleared != lastCleared);
}
