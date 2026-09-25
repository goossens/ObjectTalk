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

#include "imgui.h"

#include "OtScene.h"
#include "OtSceneRenderer.h"


//
//	OtSceneViewer
//

class OtSceneViewer {
public:
	// render scene to screen
	void render(OtScene* scene, const ImVec2& size=ImVec2());

	// release all rendering resources
	inline void clear() { renderer.reset(); }

private:
	// our scene renderer
	std::unique_ptr<OtSceneRenderer> renderer;

	// active camera
	OtEntity activeCamera = OtEntityNull;

	// work variables
	bool initialized = false;
	ImVec2 startPos;
	ImVec2 endPos;
	ImVec2 size;
	std::string error;

	// support functions
	void updateScene(OtScene* scene);
	void renderScene(OtScene* scene);
	void renderMessages(OtScene* scene);
	void renderCenteredText(const std::string& text, float alpha=1.0f);
	void findActiveCamera(OtScene* scene);
};
