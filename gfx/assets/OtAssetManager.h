//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <string>
#include <thread>
#include <unordered_map>

#include "OtConcurrentQueue.h"
#include "OtHash.h"
#include "OtLibuv.h"
#include "OtLog.h"
#include "OtSingleton.h"

#include "OtAssetBase.h"
#include "OtPathTools.h"


//
//	OtAssetManager
//

class OtAssetManager : public OtSingleton<OtAssetManager> {
public:
	// destructor
	~OtAssetManager();

	// start/stop the asset manager
	void start();
	void stop();

	// render a "spinner" if we are loading
	void renderUI();

	// see if we are currently loading anything
	inline bool isLoading() { return loading || queue.size(); }

	// see if a certain asset is already loaded
	template<typename T>
	inline bool isLoaded(const std::string& path) {
		Key key{typeid(T).hash_code(), path};
		return assets.find(key) != assets.end();
	}

	// acquire an asset
	template<typename T>
	inline T* acquire(const std::string& path) {
		// sanity check
		static_assert(std::is_base_of<OtAssetBase, T>::value, "Class is not derived from OtAssetBase");

		// calculate registry key
		Key key{typeid(T).hash_code(), path};

		// is this a known asset?
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
			if (OtPathIsVirtual(path)) {
				// yes, just mark it as missing for now
				asset->assetState = OtAssetBase::missingState;

			} else {
				// ensure path exists
				if (OtPathIsRegularFile(path)) {
					// ensure file extension is supported by asset type
					if (asset->supportsFileType(OtPathGetExtension(path))) {
						// asset loading is asynchronous
						scheduleLoad(asset);

						// also follow path so we can detect file changes
						asset->follower.follow(path, [this, asset]() {
							scheduleLoad(asset);
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
	void each(std::function<void(OtAssetBase*)> callback);

private:
	// schedule an asset for loading
	void scheduleLoad(OtAssetBase* asset);

	// clear unused assets
	void clearUnusedAssets();

	// thread running the async actions
	std::thread thread;
	bool running = false;
	bool loading = false;

	// keeping track of loading jobs
	OtConcurrentQueue<OtAssetBase*> queue;

	// timer to run the "garbage collector"
	uv_timer_t cleanupTimerHandle;

	// the key to the asset registry
	struct Key {
		// constructor
		Key(size_t t, const std::string& p) : type(t), path(p) {}

		// check for equality
		bool operator==(const Key &other) const { return type == other.type && path == other.path; }

		// properties
		size_t type;
		std::string path;
	};

	struct KeyHasher {
		std::size_t operator()(const Key& k) const { return OtHash(k.type, k.path); }
	};

	// the registry of loaded assets
	std::unordered_map<Key, OtAssetBase*, KeyHasher> assets;
};
