//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtSceneRuntime.h"


//
//	OtSceneApp
//

class OtSceneApp : public OtSceneRuntime, public OtFrameworkApp {
public:
	// constructor
	inline OtSceneApp(std::string path) : scenePath(path) {}

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

	// the logo for the splash screen
	std::shared_ptr<OtLogo> logo;
};
