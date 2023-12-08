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


//
//	OtAssetBase
//

class OtAssetBase {
public:
	// destructor
	virtual ~OtAssetBase() {}

	// function to load the asset (to be implemented by derived classes)
	virtual bool load(const std::filesystem::path& path) { return false; }

	// get state
	inline bool isNull() { return assetState == nullState; }
	inline bool isScheduled() { return assetState == scheduledState; }
	inline bool isLoading() { return assetState == loadingState; }
	inline bool isMissing() { return assetState == missingState; }
	inline bool isInvalid() { return assetState == invalidState; }
	inline bool isReady() { return assetState == readyState; }

private:
	// give the asset manager full access
	friend class OtAssetManager;

	// path to the asset
	std::filesystem::path assetPath;

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
