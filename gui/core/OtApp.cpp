//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtFunction.h"
#include "OtLog.h"
#include "OtVM.h"

#include "OtApp.h"
#include "OtFramework.h"
#include "OtGui.h"


//
//	OtAppClass::onSetup
//

void OtAppClass::onSetup() {
	// call subclass member function (if we have one)
	if (hasByName("setup")) {
		OtVM::instance()->callMemberFunction(OtApp(this), "setup");
	}
}


//
//	OtAppClass::onRender
//

void OtAppClass::onRender() {
	// create window
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin(
		"gui",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus);

	// render all children
	for (auto& child : children) {
		if (child->isEnabled()) {
			OtWidget(child)->render();
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();
}


//
//	OtAppClass::onTerminate
//

void OtAppClass::onTerminate() {
	// call subclass member function (if we have one)
	if (hasByName("terminate")) {
		OtVM::instance()->callMemberFunction(OtApp(this), "terminate");
	}

	// remove all children from the app to avoid memory leaks
	clear();

	// also remove app instance variables that the user might have added
	unsetAll();
}


//
//	OtAppClass::run
//

void OtAppClass::run() {
	OtFramework framework;
	framework.run(this);
}


//
//	OtAppClass::getMeta
//

OtType OtAppClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtAppClass>("App", OtWidgetClass::getMeta());
		type->set("run", OtFunction::create(&OtAppClass::run));
	}

	return type;
}
