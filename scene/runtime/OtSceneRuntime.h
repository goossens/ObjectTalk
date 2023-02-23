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
#include <thread>

#include "OtScene2.h"
#include "OtSceneRenderer.h"


//
//	OtSceneRuntime
//

class OtSceneRuntime {
public:
	// setup the scene runtime
	void setup(std::filesystem::path path);

	// load the scene and its assets
	void load();

	// are we loaded yet
	bool isLoaded() { return loaded; }

	// render a frame for the scene (return the texture index)
	int render(int width, int height);

	// terminate the runtime
	void terminate();

private:
	// startup loader thread
	std::thread loader;
	bool loaded = false;

	// the scene we are running
	std::shared_ptr<OtScene2> scene;
	std::filesystem::path scenePath;

	// active camera
	OtEntity activeCamera = OtEntityNull;

	// our scene renderer
	std::shared_ptr<OtSceneRenderer> renderer;
};
