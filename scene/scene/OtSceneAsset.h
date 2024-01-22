//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	// constructpr
	OtSceneAsset();

	// load the asset
	bool load() override;

	// access the scene
	inline OtScene* getScene() { return scene.get(); }

private:
	// the actual scene
	std::unique_ptr<OtScene> scene;
};
