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

#include "OtAssert.h"

#include "OtAssetBase.h"
#include "OtAssetManager.h"
#include "OtAssetSelector.h"
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
	// destructor
	~OtAsset() {
		clear();
	}

	// clear the asset reference
	inline void clear() {
		if (ptr) {
			ptr->dereference();
			ptr = nullptr;
		}
	}

	// assignment (which also loads the asset)
	inline OtAsset& operator=(const std::string& path) {
		load(path);
		return *this;
	}

	// load the asset
	inline void load(const std::string& path, std::function<void()> callback=nullptr) {
		if (ptr) {
			clear();
		}

		if (path != "") {
			ptr = OtAssetManager::instance()->acquire<T>(path, callback);
			ptr->reference();
		}
	}

	// save the asset
	inline void save() {
		OtAssert(ptr);
		OtAssetManager::instance()->save(ptr);
	}

	// save the asset under a new name
	inline void saveAs(const std::string& path) {
		OtAssert(ptr);
		OtAssetManager::instance()->saveAs(ptr, path);
	}

	// get the current path
	inline std::string getPath() { return ptr ? ptr->getPath() : ""; };

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
	inline bool isVirtual() { return ptr && ptr->isVirtual(); }
	inline bool canHandleVirtual() { return T::canHandleVirtual; }
	inline const char* getSupportedFileTypes() { return T::supportedFileTypes; }

	// handle virtual mode
	bool getVirtualMode() { return virtualMode; }
	void setVirtualMode(bool vm) { virtualMode = vm; }

	// register callbacks
	// if the callback returns false, it is automatically cancelled
	// if the callback returns true, it remains active
	inline OtAssetPreLoadListerner onPreLoad(std::function<bool()> cb) { OtAssert(ptr); return ptr->onPreLoad(cb); }
	inline OtAssetPostLoadListerner onPostLoad(std::function<bool()> cb) { OtAssert(ptr); return ptr->onPostLoad(cb); }
	inline OtAssetPreSaveListerner onPreSave(std::function<bool()> cb) { OtAssert(ptr); return ptr->onPreSave(cb); }
	inline OtAssetPostSaveListerner onPostSave(std::function<bool()> cb) { OtAssert(ptr); return ptr->onPostSave(cb); }
	inline OtAssetChangedListerner onChange(std::function<bool()> cb) { OtAssert(ptr); return ptr->onChange(cb); }
	inline OtAssetRenamedListerner onReload(std::function<bool()> cb) { OtAssert(ptr); return ptr->onReload(cb); }

	// cancel callbacks
	inline void cancelListener(OtAssetPreLoadListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetPostLoadListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetPreSaveListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetPostSaveListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetChangedListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }
	inline void cancelListener(OtAssetRenamedListerner listener) { OtAssert(ptr); ptr->unlisten(listener); }

	// render UI to show/select an asset path
	inline bool renderUI(const char* label, std::function<void(const std::string&)> creator=nullptr) {
		virtualMode |= isVirtual();

		OtAssetSelector::Info info;
		info.label = label;
		info.path = ptr ? ptr->getPath() : "";
		info.supportedFileTypes = T::supportedFileTypes;
		info.hasEditor = T::hasEditor;
		info.virtualMode = canHandleVirtual() ? &virtualMode : nullptr;
		info.creator = creator;

		if (OtAssetSelector::renderUI(info)) {
			load(info.path);
			return true;

		} else {
			return false;
		}
	}

	// render UI to only allow virtual assets
	inline bool renderVirtualUI(const char* label) {
		OtAssert(canHandleVirtual());
		virtualMode = true;

		OtAssetSelector::Info info;
		info.label = label;
		info.path = ptr ? ptr->getPath() : "";
		info.virtualOnly = true;
		info.virtualMode = &virtualMode;

		if (OtAssetSelector::renderUI(info)) {
			load(info.path);
			return true;

		} else {
			return false;
		}
	}

private:
	// pointer to the actual asset
	T* ptr = nullptr;
	bool virtualMode = false;
};
