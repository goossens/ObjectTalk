//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
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
	void renderUI();

	// see if we are currently loading anything
	inline bool isLoading() { return loading || queue.size(); }

	// acquire an asset
	template<typename T>
	inline T* acquire(const std::string& path) {
		static_assert(std::is_base_of<OtAssetBase, T>::value, "Class is not derived from OtAssetBase");
		return dynamic_cast<T*>(lookup(path));
	}

private:
	// find asset instance
	OtAssetBase* lookup(const std::string& path);

	// create a dummy asset
	OtAssetBase* createDummy(const std::string& path, OtAssetBase::AssetState state);

	// thread running the async actions
	std::thread thread;
	bool running = false;
	bool loading = false;

	// keeping track of loading jobs
	OtConcurrentQueue<OtAssetBase*> queue;

	// the registry of loaded assets
	std::unordered_map<std::string, OtAssetBase*> assets;
};
