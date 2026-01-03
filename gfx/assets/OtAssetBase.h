//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
#include "OtPath.h"
#include "OtPathFollower.h"
#include "OtPubSub.h"
#include "OtText.h"


//
//	Message types
//

struct OtAssetChanged : OtMessage<> {};


//
//	Listerner types
//

using OtAssetListerner = OtPublisher<OtAssetChanged>::listener;


//
//	OtAssetBase
//

class OtAssetBase {
	// give the asset manager full access
	friend class OtAssetManager;

public:
	// destructor
	virtual ~OtAssetBase();

	// get the path
	inline std::string& getPath() { return path; }

	// get state
	inline bool isNull() { return state == State::null; }
	inline bool isLoading() { return state == State::loading; }
	inline bool isMissing() { return state == State::missing; }
	inline bool isInvalid() { return state == State::invalid; }
	inline bool isLoaded() { return state == State::loaded; }
	inline bool isReady() { return state == State::ready; }
	inline bool isVirtual() { return OtText::startsWith(path, "virtual:"); }

	const char* getStateName();
	bool supportsFileType(const std::string& ext);

	inline std::string getErrorMessage() { return errorMessage; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool hasCreator = false;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = "";
	inline virtual const char* getSupportedFileTypes() { return supportedFileTypes; }

	// event handler for file system changes
	// if the callback returns false, the listener is automatically deactivated
	// if the callback returns true, it remains active
	inline OtAssetListerner addListener(std::function<bool()> cb) { return publisher.listen<OtAssetChanged>(cb); }
	inline void cancelListener(OtAssetListerner listener) { publisher.unlisten(listener); }
	inline void notify() { publisher.changed(); }

	// reference counting
	inline void reference() { references++; }
	inline void dereference() { references--; }
	inline size_t getReferences() { return references; }

protected:
	// path to the asset
	std::string path;

	// state of the asset
	enum class State {
		null,
		loading,
		missing,
		invalid,
		loaded,
		ready
	};

	State state = State::null;
	std::string errorMessage;

	// functions to load the asset (to be implemented by derived classes)
	virtual inline State load() { return State::null; }

private:
	// reference count
	size_t references = 0;

	// loader event handling
	uv_async_t* loaderEventHandle = nullptr;

	// follower to detect file system changes
	OtPathFollower follower;

	// our publisher to notify subscribers
	struct Publisher : OtPublisher<OtAssetChanged> {
		void changed() { notify<OtAssetChanged>(); }
	} publisher;
};
