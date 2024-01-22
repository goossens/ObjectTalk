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


//
//	OtAssetBase
//

class OtAssetBase {
public:
	// destructor
	virtual inline ~OtAssetBase() {}

	// functions to load/save the asset (to be implemented by derived classes)
	virtual inline bool load() { return false; }
	virtual inline bool save() { return false; }

	// rename the asset
	void rename(const std::string& newPath);

	// get the path
	inline std::string& getPath() { return path; }

	// get state
	inline bool isNull() { return assetState == nullState; }
	inline bool isScheduled() { return assetState == scheduledState; }
	inline bool isLoading() { return assetState == loadingState; }
	inline bool isMissing() { return assetState == missingState; }
	inline bool isInvalid() { return assetState == invalidState; }
	inline bool isReady() { return assetState == readyState; }

	// can we handle virtual paths
	virtual inline bool handlesVirtual() { return false; }

protected:
	// give the asset manager full access
	friend class OtAssetManager;

	// path to the asset
	std::string path;

	// state of the asset
	enum AssetState {
		nullState,
		scheduledState,
		loadingState,
		missingState,
		invalidState,
		readyState
	} assetState = nullState;
};
