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
#include "OtLog.h"
#include "OtNumbers.h"

#include "OtAssetManager.h"
#include "OtPathTools.h"


//
//	OtAssetManager::start
//

OtAssetManager::~OtAssetManager() {
	// sanity check
	OtAssert(running == false);
	OtAssert(assets.size() == 0);
}


//
//	OtAssetManager::start
//

void OtAssetManager::start() {
	// sanity check
	OtAssert(running == false);

	// start the cleanup timer
	int status = uv_timer_init(uv_default_loop(), &cleanupTimerHandle);
	UV_CHECK_ERROR("uv_timer_init", status);
	cleanupTimerHandle.data = this;

	status = uv_timer_start(&cleanupTimerHandle, [](uv_timer_t* handle) {
		auto manager = (OtAssetManager*) handle->data;
		manager->clearUnusedAssets();
	}, 10000, 10000);

	UV_CHECK_ERROR("cleanupTimerHandle", status);

	// start a new thread
	thread = std::thread([this]() {
		running = true;

		while (running) {
			queue.wait();
			loading = true;
			auto asset = queue.pop();

			// load next asset
			if (asset) {
				asset->postLoad(asset->load());

			} else {
				// a null asset means we shutdown the loader
				running = false;
			}

			loading = false;
		}
	});
}

//
//	OtAssetManager::stop
//

void OtAssetManager::stop() {
	// sanity check
	OtAssert(running == true);

	// wake up runner and wait until thread terminates
	queue.push(nullptr);
	thread.join();

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
//	OtAssetManager::renderUI
//

void OtAssetManager::renderUI() {
	// see if we are currently loading assets
	if (loading || !queue.empty()) {
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
//	OtAssetManager::scheduleAssetForLoading
//

void OtAssetManager::scheduleAssetForLoading(OtAssetBase* asset) {
	// see if asset exists
	auto path = asset->getPath();

	if (OtPathIsRegularFile(path)) {
		// ensure file extension is supported by asset type
		if (asset->supportsFileType(OtPathGetExtension(path))) {
			// schedule asset for loading
			queue.push(asset);
			asset->preLoad(path);

		} else {
			OtLogWarning("Asset [{}] refers to unsupported type", path);
			asset->markInvalid();
		}

	} else {
		OtLogWarning("Asset [{}] not found", path);
		asset->markMissing();
	}
}


//
//	OtAssetManager::each
//

void OtAssetManager::each(std::function<void(OtAssetBase*)> callback) {
	for (const auto& [path, asset] : assets) {
		callback(asset);
	}
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
