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
#include <string>

#include "OtFrameworkApp.h"
#include "OtLogo.h"

#include "OtScene.h"
#include "OtSceneViewer.h"


//
//	OtSceneApp
//

class OtSceneApp : public OtFrameworkApp {
public:
	// constructor
	OtSceneApp(std::string path) : scenePath(path) {}

private:
	// framework callbacks
	void onSetup() override;
	void onRender() override;
	void onTerminate() override;

	// render parts
	void renderSplashScreen();
	void renderViewPort();

	// the scene we are running
	std::string scenePath;
	OtScene scene;
	bool ready = false;

	// the scene viewer
	OtSceneViewer viewer;

	// the logo for the splash screen
	std::shared_ptr<OtLogo> logo;
};
