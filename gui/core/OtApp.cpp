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
#include "OtModule.h"
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
		OtVM::callMemberFunction(OtApp(this), "setup");
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

	// call subclass member function (if we have one)
	if (hasByName("update")) {
		OtVM::callMemberFunction(OtApp(this), "update");
	}

	// render all children
	renderChildren();

	ImGui::End();
	ImGui::PopStyleVar();
}


//
//	OtAppClass::onTerminate
//

void OtAppClass::onTerminate() {
	// call subclass member function (if we have one)
	if (hasByName("terminate")) {
		OtVM::callMemberFunction(OtApp(this), "terminate");
	}

	// remove all children from the app to avoid memory leaks
	clear();

	// clear all entities that might cache objects
	// this is to ensure any UI resources are released before the those libraries are shutdown
	OtVM::clear();
	OtModuleClass::clear();
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
