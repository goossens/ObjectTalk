//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtTextDiffWidget.h"


//
//	OtTextDiffWidgetClass::init
//

void OtTextDiffWidgetClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 3:
			setLanguage(parameters[2]->operator std::string());
			[[fallthrough]];

		case 2:
			setText(parameters[0]->operator std::string(), parameters[1]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
		OtLogError("[TextDiff] constructor expects0, 2 or 3 arguments (not {})", count);
	}
}


//
//	OtTextDiffWidgetClass::setLanguage
//

void OtTextDiffWidgetClass::setLanguage(const std::string& text) {
	if (text == "C") {
		diff.SetLanguage(TextDiff::Language::C());

	} else if (text == "C++") {
		diff.SetLanguage(TextDiff::Language::Cpp());

	} else if (text == "C#") {
		diff.SetLanguage(TextDiff::Language::Cs());

	} else if (text == "AngelScript") {
		diff.SetLanguage(TextDiff::Language::AngelScript());

	} else if (text == "Lua") {
		diff.SetLanguage(TextDiff::Language::Lua());

	} else if (text == "Python") {
		diff.SetLanguage(TextDiff::Language::Python());

	} else if (text == "GLSL") {
		diff.SetLanguage(TextDiff::Language::Glsl());

	} else if (text == "HLSL") {
		diff.SetLanguage(TextDiff::Language::Hlsl());

	} else if (text == "JSON") {
		diff.SetLanguage(TextDiff::Language::Json());

	} else if (text == "Markdown") {
		diff.SetLanguage(TextDiff::Language::Markdown());

	} else if (text == "SQL") {
		diff.SetLanguage(TextDiff::Language::Sql());

	} else {
		OtLogError("Unknown language [{}], should be C, C++, C#, AngelScript, Lua, Python, GLSL, HLSL, JSON, Markdown or SQL", text);
	}
}


//
//	OtTextDiffWidgetClass::render
//

void OtTextDiffWidgetClass::render() {
	ImGui::PushID(this);
	diff.Render("Diff");
	ImGui::PopID();
}


//
//	OtTextDiffWidgetClass::getMeta
//

OtType OtTextDiffWidgetClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTextDiffWidgetClass>("TextDiff", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtTextDiffWidgetClass::init));

		type->set("setTabSize", OtFunction::create(&OtTextDiffWidgetClass::setTabSize));
		type->set("getTabSize", OtFunction::create(&OtTextDiffWidgetClass::getTabSize));
		type->set("setLineSpacing", OtFunction::create(&OtTextDiffWidgetClass::setLineSpacing));
		type->set("getLineSpacing", OtFunction::create(&OtTextDiffWidgetClass::getLineSpacing));
		type->set("setShowWhitespacesEnabled", OtFunction::create(&OtTextDiffWidgetClass::setShowWhitespacesEnabled));
		type->set("isShowWhitespacesEnabled", OtFunction::create(&OtTextDiffWidgetClass::isShowWhitespacesEnabled));
		type->set("setShowScrollbarMiniMapEnabled", OtFunction::create(&OtTextDiffWidgetClass::setShowScrollbarMiniMapEnabled));
		type->set("isShowScrollbarMiniMapEnabled", OtFunction::create(&OtTextDiffWidgetClass::isShowScrollbarMiniMapEnabled));

		type->set("setLanguage", OtFunction::create(&OtTextDiffWidgetClass::setLanguage));
		type->set("getLanguage", OtFunction::create(&OtTextDiffWidgetClass::getLanguage));

		type->set("setText", OtFunction::create(&OtTextDiffWidgetClass::setText));
	}

	return type;
}
