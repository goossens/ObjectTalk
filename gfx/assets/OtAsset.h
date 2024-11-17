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
	// sanity check
	static_assert(std::is_base_of<OtAssetBase, T>::value, "Instance is not derived from OtAssetBase");

	// constructors
	OtAsset() = default;

	inline OtAsset(T* instance) {
		ptr = instance;
		ptr->reference();
		follow();
	}

	inline OtAsset(const OtAsset& asset) {
		if (asset.ptr) {
			ptr = asset.ptr;
			virtualMode = asset.virtualMode;
			ptr->reference();
			follow();
		}
	}

	template <typename Ts>
	OtAsset(const OtAsset<Ts>& asset) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert asset types");

		if (asset.ptr) {
			ptr = dynamic_cast<T*>(asset.ptr);
			virtualMode = asset.virtualMode;
			ptr->reference();
			follow();
		}
	}

	inline OtAsset(OtAsset&& asset) {
		if (asset.ptr) {
			ptr = asset.ptr;
			virtualMode = asset.virtualMode;
			onChangeCallback = asset.onChangeCallback;
			follow();

			asset.unfollow();
			asset.ptr = nullptr;
			asset.onChangeCallback = nullptr;
		}
	}

	template <typename Ts>
	OtAsset(OtAsset<Ts>&& asset) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert asset types");

		if (asset.ptr) {
			ptr = asset.ptr;
			virtualMode = asset.virtualMode;
			onChangeCallback = asset.onChangeCallback;
			follow();

			asset.unfollow();
			asset.ptr = nullptr;
			asset.onChangeCallback = nullptr;
		}
	}

	// destructor
	inline ~OtAsset() {
		if (ptr) {
			unfollow();
			ptr->dereference();
		}
	}

	// clear the asset reference
	inline void clear() {
		if (ptr) {
			unfollow();
			ptr->dereference();
			ptr = nullptr;
			notifyChanged();
		}

		virtualMode = false;
	}

	// assignment (which also loads the asset)
	inline OtAsset& operator=(const std::string& path) {
		if (!ptr || (ptr && ptr->getPath() != path)) {
			load(path);
		}

		return *this;
	}

	// other assignment operators
	OtAsset& operator=(const OtAsset& asset) {
		clear();

		if (asset.ptr) {
			ptr = asset.ptr;
			virtualMode = asset.virtualMode;
			onChangeCallback = nullptr;
			ptr->reference();
			follow();
		}

		return *this;
	}

	template <typename Ts>
	OtAsset& operator=(const OtAsset<Ts>& asset) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert asset types");
		clear();

		if (asset.ptr) {
			ptr = dynamic_cast<T*>(asset.ptr);
			virtualMode = asset.virtualMode;
			onChangeCallback = nullptr;
			ptr->reference();
			follow();
		}

		return *this;
	}

	// move assignment operators
	OtAsset& operator=(OtAsset&& asset) {
		clear();

		if (asset.ptr) {
			ptr = asset.ptr;
			virtualMode = asset.virtualMode;
			onChangeCallback = asset.onChangeCallback;
			follow();

			asset.unfollow();
			asset.ptr = nullptr;
			asset.onChangeCallback = nullptr;
		}

		return *this;
	}

	template <typename Ts>
	OtAsset& operator=(OtAsset<Ts>&& asset) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert asset types");
		clear();

		if (asset.ptr) {
			ptr = dynamic_cast<T*>(asset.ptr);
			virtualMode = asset.virtualMode;
			onChangeCallback = asset.onChangeCallback;
			follow();

			asset.ptr = nullptr;
			asset.onChangeCallback = nullptr;
			asset->unfollow();
		}

		return *this;
	}

	// load the asset
	inline void load(const std::string& path) {
		clear();

		if (path != "") {
			ptr = OtAssetManager::acquire<T>(path);
			ptr->reference();
			follow();
		}
	}

	// get the current path
	inline std::string getPath() { return ptr ? ptr->getPath() : ""; };

	// access the actual asset
	inline T* operator->() { OtAssert(ptr); return ptr; }
	inline const T* operator->() const { OtAssert(ptr); return ptr; }
	inline T& operator*() { OtAssert(ptr); return *ptr; }
	inline const T& operator*() const { OtAssert(ptr); return *ptr; }

	// check asset status
	inline bool isNull() { return !ptr || ptr->isNull(); }
	inline bool isLoading() { return ptr && ptr->isLoading(); }
	inline bool isMissing() { return ptr && ptr->isMissing(); }
	inline bool isInvalid() { return ptr && ptr->isInvalid(); }
	inline bool isLoaded() { return ptr && ptr->isLoaded(); }
	inline bool isReady() { return ptr && ptr->isReady(); }
	inline bool isVirtual() { return ptr && ptr->isVirtual(); }
	inline bool canHandleVirtual() { return T::canHandleVirtual; }
	inline const char* getSupportedFileTypes() { return T::supportedFileTypes; }

	// OtAsset virtual mode
	bool getVirtualMode() { return virtualMode; }
	void setVirtualMode(bool vm) { virtualMode = vm; }

	// register callback to monitor changes
	inline void onChange(std::function<void()> cb) { onChangeCallback = cb; }

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

private:
	// pointer to the actual asset
	T* ptr = nullptr;
	bool virtualMode = false;

	// event management
	std::function<void()> onChangeCallback = nullptr;
	OtAssetListerner changedListerner;

	// follow/unfollow the actual asset
	void follow() {
		changedListerner = ptr->addListener([this]() {
			notifyChanged();
			return true;
		});

		notifyChanged();
	}

	void unfollow() {
		ptr->cancelListener(changedListerner);
	}

	// notify follower that changes have happend
	void notifyChanged() {
		if (onChangeCallback) {
			onChangeCallback();
		}
	}
};
