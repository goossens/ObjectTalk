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
#include <future>
#include <memory>

#include "OtScene.h"
#include "OtSceneRenderer.h"


//
//	OtSceneRuntime
//

class OtSceneRuntime {
public:
	// setup the scene runtime
	void setup(std::filesystem::path path);

	// load the scene and its assets
	bool load();

	// are we ready
	bool isReady();

	// render a frame for the scene (return the texture index)
	int render(int width, int height);

	// terminate the runtime
	void terminate();

private:
	// startup loader
	std::future<bool> loader;
	bool ready = false;

	// the scene we are running
	std::unique_ptr<OtScene> scene;
	std::filesystem::path scenePath;

	// system initialization function
	void initializeScriptingSystem();
	void initializeRenderingSystem();

	// active camera
	OtEntity activeCamera = OtEntityNull;
	bool cameraSelected = false;

	// our scene renderer
	std::unique_ptr<OtSceneRenderer> renderer;
};
