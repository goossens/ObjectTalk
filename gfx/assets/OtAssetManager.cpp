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
#include "OtLibuv.h"
#include "OtLog.h"
#include "OtNumbers.h"

#include "OtAssetFactory.h"
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

	// start a new thread
	thread = std::thread([this]() {
		running = true;

		while (running) {
			queue.wait();
			loading = true;
			auto asset = queue.pop();

			// load next asset
			if (asset) {
				asset->postLoad(asset->load() ? OtAssetBase::readyState : OtAssetBase::invalidState);

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

	// clear all the assets
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
//	OtAssetManager::findAsset
//

OtAssetBase* OtAssetManager::findAsset(const std::string& path, std::function<void()> callback) {
	// see if we are already tracking this asset
	if (assets.find(path) != assets.end()) {
		if (callback) {
			scheduleLoadedCallback(callback);
		}

		return assets[path];

	// is this a virtual asset (i.e. a named asset that only exists in memory)
	// or a new asset that hasn't been saved yet
	} else if (OtPathIsVirtual(path) || OtPathIsUntitled(path)) {
		auto asset = OtAssetFactory::instance()->instantiate(path);

		// is this a supported asset type
		if (asset) {
			asset->create(path);
			assets[path] = asset;

			if (callback) {
				scheduleLoadedCallback(callback);
			}

			return asset;

		} else {
			// unknown asset type, create a dummy asset
			OtLogWarning(OtFormat("Unknown asset type [%s]", path.c_str()));
			return createDummy(path, OtAssetBase::invalidState);
		}

	// is this an existing file
	} else if (OtPathIsRegularFile(path)) {
		// ensure the factory can create an asset for this path
		auto asset = OtAssetFactory::instance()->instantiate(path);

		if (asset) {
			// track new asset, put it onto the loading queue and send notification
			assets[path] = asset;
			queue.push(asset);
			asset->preLoad(path);

			// register for load completion event so we can call callback
			if (callback) {
				asset->onPostLoad([this, callback]() {
					scheduleLoadedCallback(callback);
					return false;
				});
			}

			return asset;

		} else {
			// unknown asset type, create a dummy asset
			OtLogWarning(OtFormat("Unknown asset type [%s]", path.c_str()));
			return createDummy(path, OtAssetBase::invalidState);
		}

	} else {
		// create a dummy asset
		OtLogWarning(OtFormat("Asset [%s] not found", path.c_str()));
		return createDummy(path, OtAssetBase::missingState);
	}
}


//
//	OtAssetManager::scheduleLoadedCallback
//

void OtAssetManager::scheduleLoadedCallback(std::function<void()> callback) {
	struct Handle {
		uv_async_t async;
		std::function<void()> callback;
	};

	auto callbackHandle = new Handle;
	callbackHandle->callback = callback;

	auto status = uv_async_init(uv_default_loop(), (uv_async_t*) callbackHandle, [](uv_async_t* handle){
		// execute callback on completion
		((Handle*) handle)->callback();

		// cleanup
		uv_close((uv_handle_t*) handle, [](uv_handle_t* handle) {
			delete (Handle*) handle;
		});
	});

	UV_CHECK_ERROR("uv_async_init", status);

	status = uv_async_send((uv_async_t*) callbackHandle);
	UV_CHECK_ERROR("uv_async_send", status);
}


//
//	OtAssetManager::save
//

void OtAssetManager::save(OtAssetBase* asset) {
	asset->preSave();
	asset->save();
	asset->postSave();
}


//
//	OtAssetManager::saveAs
//

void OtAssetManager::saveAs(OtAssetBase* asset, const std::string &newName) {
	asset->preSave(newName);
	asset->save();
	asset->postSave();
}


//
//	OtAssetManager::createDummy
//

OtAssetBase *OtAssetManager::createDummy(const std::string& path, OtAssetBase::AssetState state) {
	auto dummy = new OtAssetBase();
	dummy->path = path;
	dummy->assetState = state;
	assets[path] = dummy;
	return dummy;
}
