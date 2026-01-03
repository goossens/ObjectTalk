//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "OtAsset.h"
#include "OtAssetBase.h"
#include "OtScene.h"


//
//	OtSceneAsset
//

class OtSceneAsset : public OtAssetBase {
public:
	// constructor
	OtSceneAsset();

	// access the scene
	inline OtScene* getScene() { return scene.get(); }

	// asset properties
	static constexpr bool hasEditor = true;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".ots";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override;

private:
	// the actual scene
	std::unique_ptr<OtScene> scene;
};
