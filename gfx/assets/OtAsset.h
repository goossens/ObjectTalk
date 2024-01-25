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

#include "OtAssert.h"
#include "OtFormat.h"
#include "OtLog.h"

#include "OtAssetBase.h"
#include "OtAssetManager.h"
#include "OtUi.h"


//
//	OtAsset
//
//	Assets are owned and managed by the OtAssetManager
//	This class simply provides a pointer to an OtAssetBase subclass instance
//	Each asset only exists ones but multiple instances can refer to the same asset
//
//	Assets can be files on disk or virtual files that only exist in memory
//	Virtual assets have a path starting with "virtual:"
//

template <typename T>
class OtAsset {
public:
	// constructor
	OtAsset() = default;

	// clear the asset reference
	inline void clear() {
		ptr = nullptr;
	}

	// assignment (which also loads the asset)
	inline OtAsset& operator=(const std::string& path) {
		ptr = OtAssetManager::instance()->acquire<T>(path);
		return *this;
	}

	// load/save the asset
	inline void load(const std::string& path) {
		ptr = OtAssetManager::instance()->acquire<T>(path);
	}

	inline void load(const std::string& path, std::function<void()> callback) {
		ptr = OtAssetManager::instance()->acquire<T>(path, callback);
	}
	inline void save() {
		OtAssert(ptr);
		OtAssetManager::instance()->save(ptr);
	}

	inline void saveAs(const std::string& path) {
		OtAssert(ptr);
		OtAssetManager::instance()->saveAs(ptr, path);
	}

	// get the current path
	inline std::string getPath() {
		OtAssert(ptr);
		return ptr->getPath();
	}

	// access the actual asset
	inline T* operator->() { return ptr; }
	inline const T* operator->() const { return ptr; }
	inline T& operator*() { return *ptr; }
	inline const T& operator*() const { return *ptr; }

	// check asset status
	inline bool isNull() { return !ptr || ptr->isNull(); }
	inline bool isLoading() { return ptr && ptr->isLoading(); }
	inline bool isMissing() { return ptr && ptr->isMissing(); }
	inline bool isInvalid() { return ptr && ptr->isInvalid(); }
	inline bool isReady() { return ptr && ptr->isReady(); }
	inline bool isVirtual() { return ptr && OtPathIsVirtual(ptr->getPath()); }

	// register callbacks
	// if the callback returns false, that listener is automatically removed
	// if the callback returns true, it remains active
	inline OtAssetPreLoadListerner onPreLoad(std::function<bool()> cb) { OtAssert(ptr); return ptr->onPreLoad(cb); }
	inline OtAssetPostLoadListerner onPostLoad(std::function<bool()> cb) { OtAssert(ptr); return ptr->onPostLoad(cb); }
	inline OtAssetPreSaveListerner onPreSave(std::function<bool()> cb) { OtAssert(ptr); return ptr->onPreSave(cb); }
	inline OtAssetPostSaveListerner onPostSave(std::function<bool()> cb) { OtAssert(ptr); return ptr->onPostSave(cb); }
	inline OtAssetChangedListerner onChange(std::function<bool()> cb) { OtAssert(ptr); return ptr->onChange(cb); }
	inline OtAssetRenamedListerner onReload(std::function<bool()> cb) { OtAssert(ptr); return ptr->onReload(cb); }

	// cancel callbakcs
	inline void cancelListener(OtAssetPreLoadListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetPostLoadListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetPreSaveListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetPostSaveListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetChangedListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetRenamedListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }

	// render UI to show/select an asset path
	inline bool renderUI(const char* label) {
		std::string tmpPath = ptr ? ptr->getPath() : "";

		if (OtUiFileSelector(label, tmpPath)) {
			ptr = OtAssetManager::instance()->acquire<T>(tmpPath);
			return true;

		} else {
			return false;
		}
	}

private:
	// pointer to the actual asset
	T* ptr = nullptr;
};
