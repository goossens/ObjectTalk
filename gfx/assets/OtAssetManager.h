//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <thread>
#include <unordered_map>

#include "OtConcurrentQueue.h"
#include "OtSingleton.h"

#include "OtAssetBase.h"


//
//	OtAssetManager
//

class OtAssetManager : public OtSingleton<OtAssetManager> {
public:
	// start/stop the asset manager
	void start();
	void stop();

	// render a "spinner" if we are loading
	void renderGUI();

	// see if we are currently loading anything
	bool isLoading() { return loading || queue.size(); }

	// acquire an asset
	template<typename T>
	T* acquire(const std::filesystem::path& path) {
		static_assert(std::is_base_of<OtAssetBase, T>::value, "Class is not derived from OtAssetBase");
		return dynamic_cast<T*>(lookup(path));
	}

private:
	// find asset instance
	OtAssetBase* lookup(const std::filesystem::path& path);

	// thread running the async actions
	std::thread thread;
	bool running = false;
	bool loading = false;

	// keeping track of loading jobs
	OtConcurrentQueue<OtAssetBase*> queue;

	// the registry of loaded assets
	struct PathHash {
		std::size_t operator()(const std::filesystem::path& p) const noexcept {
			return std::filesystem::hash_value(p);
		}
	};

	std::unordered_map<std::filesystem::path, OtAssetBase*, PathHash> assets;
};
