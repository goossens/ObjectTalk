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
#include <memory>

#include "OtSceneAsset.h"
#include "OtSceneRenderer.h"


//
//	OtSceneRuntime
//

class OtSceneRuntime {
public:
	// setup the scene runtime
	void setup(std::filesystem::path path);

	// are we ready
	bool isReady();

	// render a frame for the scene (return the texture index)
	int render(int width, int height);

	// terminate the runtime
	void terminate();

private:
	// the scene we are running
	OtAsset<OtSceneAsset> sceneAsset;

	// system initialization function
	void initializeScriptingSystem();
	void initializeRenderingSystem();

	// active camera
	OtEntity activeCamera = OtEntityNull;

	// our scene renderer
	std::unique_ptr<OtSceneRenderer> renderer;

	// setup state
	bool ready = false;
};
