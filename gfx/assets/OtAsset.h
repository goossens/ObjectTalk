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

	OtAsset(const std::filesystem::path& p) {
		path = p;
		acquire();
	}

	// clear the asset refence
	void clear() {
		path.clear();
		ptr = nullptr;
	}

	// access the path
	std::filesystem::path& getPath() { return path; }

	// assignment
	OtAsset& operator=(const std::filesystem::path& p) {
		path = p;
		acquire();
		return *this;
	}

	// access to actual asset
	T* operator->() { return ptr; }
	const T* operator->() const { return ptr; }
	T& operator*() { return *ptr; }
	const T& operator*() const { return *ptr; }

	// check asset state
	bool isNull() { return !ptr || ptr->isNull(); }
	bool isScheduled() { return ptr && ptr->isScheduled(); }
	bool isLoading() { return ptr && ptr->isLoading(); }
	bool isMissing() { return ptr && ptr->isMissing(); }
	bool isReady() { return ptr && ptr->isReady(); }

	// render GUI to show/select an asset
	bool renderGUI(const char* label) {
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
	void acquire() {
		// when an empty path is requested, reset the asset
		if (path.empty()) {
			ptr = nullptr;

		} else {
			ptr = OtAssetManager::instance()->acquire<T>(path);
		}
	}
};
