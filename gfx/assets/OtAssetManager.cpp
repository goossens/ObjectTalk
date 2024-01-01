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
#include "OtFormat.h"
#include "OtLog.h"
#include "OtNumbers.h"

#include "OtAssetFactory.h"
#include "OtAssetManager.h"


//
//	OtAssetManager::start
//

void OtAssetManager::start() {
	// sanity check
	OtAssert(running == false);

	// start a new thread
	thread = std::thread([this]() {
		running = true;

		while (running) {
			queue.wait();
			loading = true;
			auto asset = queue.pop();

			// load next asset
			if (asset) {
				asset->assetState = OtAssetBase::loadingState;
				asset->assetState = asset->load(asset->assetPath) ? OtAssetBase::readyState : OtAssetBase::invalidState;

			} else {
				// a null asset means we shut down the loader
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

	// clear all the assets
	for (const auto& [path, asset] : assets) {
		delete asset;
	}

	assets.clear();
}


//
//	OtAssetManager::renderGUI
//

void OtAssetManager::renderGUI() {
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
//	OtAssetManager::lookup
//

OtAssetBase* OtAssetManager::lookup(const std::filesystem::path& path) {
	// ensure path points to a valid file
	if (std::filesystem::is_regular_file(path)) {
		// see if we already have loaded this asset?
		auto fullPath = std::filesystem::canonical(path);

		if (assets.find(fullPath) == assets.end()) {
			// nope, let's instantiate it now and schedule it for loading
			auto instance = OtAssetFactory::instance()->instantiate(fullPath);

			// ensure the factory could create an asset for the path
			if (instance) {
				instance->assetPath = fullPath;
				instance->assetState = OtAssetBase::scheduledState;
				assets[fullPath] = instance;
				queue.push(instance);
				return instance;

			} else {
				// unknown asset type, create a dummy asset
				OtLogWarning(OtFormat("Unknown asset type [%s]", fullPath.string().c_str()));
				auto dummy = new OtAssetBase();
				dummy->assetPath = fullPath;
				dummy->assetState = OtAssetBase::invalidState;
				assets[fullPath] = dummy;
				return dummy;
			}

		} else {
			// asset already exists, use it
			return assets[path];
		}

	} else if (assets.find(path) == assets.end()) {
		// create a dummy asset
		OtLogWarning(OtFormat("Asset [%s] not found", path.string().c_str()));
		auto dummy = new OtAssetBase();
		dummy->assetPath = path;
		dummy->assetState = OtAssetBase::missingState;
		assets[path] = dummy;
		return dummy;

	} else {
		// dummy entry already exists
		return assets[path];
	}
}
