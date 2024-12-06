//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <atomic>
#include <functional>
#include <string>
#include <unordered_map>

#include "BS_thread_pool.hpp"

#include "OtHash.h"
#include "OtLibuv.h"
#include "OtLog.h"
#include "OtSingleton.h"
#include "OtText.h"

#include "OtAssetBase.h"
#include "OtPath.h"


//
//	OtAssetManager
//

class OtAssetManager : OtSingleton<OtAssetManager> {
public:
	// destructor
	~OtAssetManager();

	// start/stop the asset manager
	static inline void start() { instance().startManager(); }
	static inline void stop() { instance().stopManager(); }

	// render a "spinner" if we are loading
	static inline void renderUI() { instance().renderManagerUI(); }

	// see if we are currently loading anything
	static inline bool isLoading() { return instance().loading != 0; }

	// see if a certain asset is already loaded
	template<typename T>
	static inline bool isLoaded(const std::string& path) {
		Key key{typeid(T).hash_code(), path};
		auto& assets = instance().assets;
		return assets.find(key) != assets.end();
	}

	// acquire an asset
	template<typename T>
	static inline T* acquire(const std::string& path) {
		// sanity check
		static_assert(std::is_base_of<OtAssetBase, T>::value, "Class is not derived from OtAssetBase");

		// calculate registry key
		Key key{typeid(T).hash_code(), path};

		// is this a known asset?
		auto manager = &instance();
		auto& assets = manager->assets;
		auto i = assets.find(key);

		if (i != assets.end()) {
			// yes, return it and schedule a ready callback
			return dynamic_cast<T*>(i->second);

		} else {
			// no, asset must be created
			auto asset = new T;
			asset->path = path;
			assets[key] = asset;

			// is this a virtual asset (i.e. a named asset that only exists in memory)
			if (OtText::startsWith(path, "virtual:")) {
				// yes, just mark it as missing for now
				asset->assetState = OtAssetBase::missingState;

			} else {
				// ensure path exists
				if (OtPath::isRegularFile(path)) {
					// ensure file extension is supported by asset type
					if (asset->supportsFileType(OtPath::getExtension(path))) {
						// asset loading is asynchronous
						manager->scheduleLoad(asset);

						// also follow path so we can detect file changes
						asset->follower.follow(path, [manager, asset]() {
							manager->scheduleLoad(asset);
						});

					} else {
						OtLogWarning("Asset [{}] refers to unsupported type, expected [{}]", path, asset->getSupportedFileTypes());
						asset->assetState = OtAssetBase::invalidState;
					}

				} else {
					OtLogWarning("Asset [{}] not found", path);
					asset->assetState = OtAssetBase::missingState;
				}
			}

			return asset;
		}
	}

	// interate through all loaded assets
	static void each(std::function<void(OtAssetBase*)> callback) {
		for (const auto& [path, asset] : instance().assets) {
			callback(asset);
		}
	}

private:
	// start/stop the asset manager
	void startManager();
	void stopManager();

	// render a "spinner" if we are loading
	void renderManagerUI();

	// schedule an asset for loading
	void scheduleLoad(OtAssetBase* asset);

	// clear unused assets
	void clearUnusedAssets();

	// the thread pool to handle the asset loading
	BS::thread_pool threadpool;
	std::atomic<int> loading = 0;

	// timer to run the "garbage collector"
	uv_timer_t cleanupTimerHandle;

	// the key to the asset registry
	struct Key {
		// constructor
		Key(size_t t, const std::string& p) : type(t), path(p) {}

		// check for equality
		bool operator==(const Key& other) const { return type == other.type && path == other.path; }

		// properties
		size_t type;
		std::string path;
	};

	struct KeyHasher {
		std::size_t operator()(const Key& k) const { return OtHash::generate(k.type, k.path); }
	};

	// the registry of loaded assets
	std::unordered_map<Key, OtAssetBase*, KeyHasher> assets;
};
