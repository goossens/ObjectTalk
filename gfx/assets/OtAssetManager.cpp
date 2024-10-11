//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "OtAssert.h"
#include "OtNumbers.h"

#include "OtAssetManager.h"
#include "OtPathTools.h"


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
	// wait for all threads to finish
	threadpool.wait();

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
	// see if we are currently loading assets
	if (loading != 0) {
		// spinner size
		static constexpr float spinnerRadius = 40.0f;
		static constexpr float spinnerMargin = 20.0f;
		static constexpr float spinnerSpeed = 7.0f;
		static constexpr float circleRadius = spinnerRadius / 10.0f;
		static constexpr int spinnerCircles = 12;

		// get window information
		auto drawList = ImGui::GetWindowDrawList();
		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();
		auto center = pos + size / 2.0f;

		static float time = 0.0f;
		time += ImGui::GetIO().DeltaTime;
		float offset = std::numbers::pi2 / spinnerCircles;

		for (int i = 0; i < spinnerCircles; i++) {
			float x = spinnerRadius * std::sin(offset * i);
			float y = spinnerRadius * std::cos(offset * i);
			float growth = std::max(0.0f, std::sin(time * spinnerSpeed - i * offset));

			ImVec4 color;
			ImVec4 dark = ImGui::GetStyleColorVec4(ImGuiCol_Border);
			ImVec4 light = ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered);
			color.x = light.x * growth + dark.x * (1.0f - growth);
			color.y = light.y * growth + dark.y * (1.0f - growth);
			color.z = light.z * growth + dark.z * (1.0f - growth);
			color.w = 1.0f;

			drawList->AddCircleFilled(ImVec2(center.x + x, center.y - y), circleRadius + growth * circleRadius * 0.5f, ImGui::GetColorU32(color));
		}
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
	asset->assetState = OtAssetBase::loadingState;
	loading++;

	threadpool.detach_task([this, asset]() {
		asset->assetState = asset->load();
		loading--;
		auto status = uv_async_send(asset->loaderEventHandle);
		UV_CHECK_ERROR("uv_async_send", status);
	});
}


//
//	OtAssetManager::clearUnusedAssets
//

void OtAssetManager::clearUnusedAssets() {
	// possibly need to loop multiple times due to asset dependancies
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
