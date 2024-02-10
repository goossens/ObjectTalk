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

#include "OtLibuv.h"
#include "OtPubSub.h"

#include "OtPathTools.h"


//
//	Message types
//

struct OtAssetPreLoad : OtMessage<> {};
struct OtAssetPostLoad : OtMessage<> {};
struct OtAssetPreSave : OtMessage<> {};
struct OtAssetPostSave : OtMessage<> {};
struct OtAssetChanged : OtMessage<> {};
struct OtAssetRenamed : OtMessage<std::string> {};


//
//	Listerner types
//

using OtAssetPreLoadListerner = OtPublisher<OtAssetPreLoad>::listener;
using OtAssetPostLoadListerner = OtPublisher<OtAssetPostLoad>::listener;
using OtAssetPreSaveListerner = OtPublisher<OtAssetPreSave>::listener;
using OtAssetPostSaveListerner = OtPublisher<OtAssetPostSave>::listener;
using OtAssetChangedListerner = OtPublisher<OtAssetChanged>::listener;
using OtAssetRenamedListerner = OtPublisher<OtAssetRenamed>::listener;


//
//	OtAssetBase
//

class OtAssetBase {
public:
	// destructor
	virtual ~OtAssetBase();

	// get the path
	inline std::string& getPath() { return path; }

	// get state
	inline bool isNull() { return assetState == nullState; }
	inline bool isLoading() { return assetState == loadingState; }
	inline bool isMissing() { return assetState == missingState; }
	inline bool isInvalid() { return assetState == invalidState; }
	inline bool isLoaded() { return assetState == loadedState; }
	inline bool isReady() { return assetState == readyState; }
	inline bool isVirtual() { return OtPathIsVirtual(path); }
	bool supportsFileType(const std::string& ext);

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool hasCreator = false;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = "";
	inline virtual const char* getSupportedFileTypes() { return supportedFileTypes; }

	// event handlers
	// if the callback returns false, that listener is automatically deactivated
	// if the callback returns true, it remains active
	inline OtAssetPreLoadListerner onPreLoad(std::function<bool()> cb) { return publisher.listen<OtAssetPreLoad>(cb); }
	inline OtAssetPostLoadListerner onPostLoad(std::function<bool()> cb) { return publisher.listen<OtAssetPostLoad>(cb); }
	inline OtAssetPreSaveListerner onPreSave(std::function<bool()> cb) { return publisher.listen<OtAssetPreSave>(cb); }
	inline OtAssetPostSaveListerner onPostSave(std::function<bool()> cb) { return publisher.listen<OtAssetPostSave>(cb); }
	inline OtAssetChangedListerner onChange(std::function<bool()> cb) { return publisher.listen<OtAssetChanged>(cb); }
	inline OtAssetRenamedListerner onReload(std::function<bool(const std::string&)> cb) { return publisher.listen<OtAssetRenamed>(cb); }

	inline void cancelListener(OtAssetPreLoadListerner listener) { publisher.unlisten(listener); }
	inline void cancelListener(OtAssetPostLoadListerner listener) { publisher.unlisten(listener); }
	inline void cancelListener(OtAssetPreSaveListerner listener) { publisher.unlisten(listener); }
	inline void cancelListener(OtAssetPostSaveListerner listener) { publisher.unlisten(listener); }
	inline void cancelListener(OtAssetChangedListerner listener) { publisher.unlisten(listener); }
	inline void cancelListener(OtAssetRenamedListerner listener) { publisher.unlisten(listener); }

	inline void reference() { references++; }
	inline void dereference() { references--; }

protected:
	// path to the asset
	std::string path;

	// state of the asset
	enum AssetState {
		nullState,
		loadingState,
		missingState,
		invalidState,
		loadedState,
		readyState
	} assetState = nullState;

	// functions to load/save the asset (to be implemented by derived classes)
	virtual inline AssetState load() { return nullState; }
	virtual inline AssetState save() { return nullState; }

	// internal housekeeping functions
	void initializeInvalid(const std::string& path);
	void initializeReady(const std::string& path);
	void preLoad(const std::string& path);
	void postLoad(AssetState state);
	void preSave();
	void preSave(const std::string& path);
	void postSave();
	void changed();
	void renamed(const std::string& path);

	inline void markMissing() { assetState = missingState; }
	inline void markInvalid() { assetState = invalidState; }

private:
	// give the asset manager full access
	friend class OtAssetManager;

	// reference count
	size_t references = 0;

	// loader event handling
	uv_async_t* loaderEventHandle = nullptr;

	// filesystem event handling
	uv_fs_event_t* fsEventHandle = nullptr;
	bool following = false;
	void follow();
	void unfollow();

	// our publisher to notify subscribers
	struct Publisher : OtPublisher<OtAssetPreLoad, OtAssetPostLoad, OtAssetPreSave, OtAssetPostSave, OtAssetChanged, OtAssetRenamed> {
		void preLoad() { notify<OtAssetPreLoad>(); }
		void postLoad() { notify<OtAssetPostLoad>(); }
		void preSave() { notify<OtAssetPreSave>(); }
		void postSave() { notify<OtAssetPostSave>(); }
		void changed() { notify<OtAssetChanged>(); }
		void renamed(const std::string& path) { notify<OtAssetRenamed>(path); }
	} publisher;
};
