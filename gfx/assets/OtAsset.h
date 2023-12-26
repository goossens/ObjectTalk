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

#include "OtAssetManager.h"
#include "OtUi.h"


//
//	OtAsset
//

template <typename T>
class OtAsset {
public:
	// constructor
	OtAsset() = default;

	inline OtAsset(const std::filesystem::path& p) {
		path = p;
		acquire();
	}

	// clear the asset refence
	inline void clear() {
		path.clear();
		ptr = nullptr;
	}

	// access the path
	inline std::filesystem::path& getPath() { return path; }

	// assignment
	inline OtAsset& operator=(const std::filesystem::path& p) {
		path = p;
		acquire();
		return *this;
	}

	// access to actual asset
	inline T* operator->() { return ptr; }
	inline const T* operator->() const { return ptr; }
	inline T& operator*() { return *ptr; }
	inline const T& operator*() const { return *ptr; }

	// check asset state
	inline bool isNull() { return !ptr || ptr->isNull(); }
	inline bool isScheduled() { return ptr && ptr->isScheduled(); }
	inline bool isLoading() { return ptr && ptr->isLoading(); }
	inline bool isMissing() { return ptr && ptr->isMissing(); }
	inline bool isReady() { return ptr && ptr->isReady(); }

	// render GUI to show/select an asset
	inline bool renderGUI(const char* label) {
		if (OtUiFileSelector(label, path)) {
			acquire();
			return true;

		} else {
			return false;
		}
	}

private:
	// asset path
	std::filesystem::path path;

	// pointer to the actual asset
	T* ptr = nullptr;

	// acquire the asset from the asset manager
	inline void acquire() {
		// when an empty path is requested, reset the asset
		if (path.empty()) {
			ptr = nullptr;

		} else {
			ptr = OtAssetManager::instance()->acquire<T>(path);
		}
	}
};
