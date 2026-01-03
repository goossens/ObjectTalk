//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
		OtObject object{this};
		OtVM::callMemberFunction(object, "setup");
	}
}


//
//	OtAppClass::onRender
//

void OtAppClass::onRender() {
	// create window
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin(
		"gui",
		nullptr,
		ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoBringToFrontOnFocus);

	// call subclass member function (if we have one)
	if (hasByName("update")) {
		OtObject object{this};
		OtVM::callMemberFunction(object, "update");
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
		OtObject object{this};
		OtVM::callMemberFunction(object, "terminate");
	}

	// remove all children from the app to avoid memory leaks
	clear();
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
