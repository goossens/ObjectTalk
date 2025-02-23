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

#include "OtTextEditorWidget.h"


//
//	OtTextEditorWidgetClass::init
//

void OtTextEditorWidgetClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setLanguage(parameters[1]->operator std::string());
			[[fallthrough]];

		case 1:
			setText(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogFatal("[TextEditor] constructor expects up to 2 arguments (not {})", count);
	}
}


//
//	OtTextEditorWidgetClass::setLanguage
//

void OtTextEditorWidgetClass::setLanguage(const std::string& text) {
	if (text == "C") {
		editor.SetLanguage(TextEditor::Language::C());

	} else if (text == "C++") {
		editor.SetLanguage(TextEditor::Language::Cpp());

	} else if (text == "C#") {
		editor.SetLanguage(TextEditor::Language::Cs());

	} else if (text == "AngelScript") {
		editor.SetLanguage(TextEditor::Language::AngelScript());

	} else if (text == "Lua") {
		editor.SetLanguage(TextEditor::Language::Lua());

	} else if (text == "Python") {
		editor.SetLanguage(TextEditor::Language::Python());

	} else if (text == "GLSL") {
		editor.SetLanguage(TextEditor::Language::Glsl());

	} else if (text == "HLSL") {
		editor.SetLanguage(TextEditor::Language::Hlsl());

	} else if (text == "JSON") {
		editor.SetLanguage(TextEditor::Language::Json());

	} else if (text == "Markdown") {
		editor.SetLanguage(TextEditor::Language::Markdown());

	} else {
		OtLogFatal("Unknow language[{}],Shoudl be C, C++, C#, AngelScript", "Lua", "Python", "GLSL", "HLSL", "JSON", "Markdown", text);
	}
}


//
//	OtTextEditorWidgetClass::render
//

void OtTextEditorWidgetClass::render() {
	ImGui::PushID(this);
	editor.Render("Editor");
	ImGui::PopID();
}


//
//	OtTextEditorWidgetClass::getMeta
//

OtType OtTextEditorWidgetClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtTextEditorWidgetClass>("TextEditor", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtTextEditorWidgetClass::init));

		type->set("setTabSize", OtFunction::create(&OtTextEditorWidgetClass::setTabSize));
		type->set("getTabSize", OtFunction::create(&OtTextEditorWidgetClass::getTabSize));
		type->set("setLineSpacing", OtFunction::create(&OtTextEditorWidgetClass::setLineSpacing));
		type->set("getLineSpacing", OtFunction::create(&OtTextEditorWidgetClass::getLineSpacing));
		type->set("setReadOnlyEnabled", OtFunction::create(&OtTextEditorWidgetClass::setReadOnlyEnabled));
		type->set("isReadOnlyEnabled", OtFunction::create(&OtTextEditorWidgetClass::isReadOnlyEnabled));
		type->set("setAutoIndentEnabled", OtFunction::create(&OtTextEditorWidgetClass::setAutoIndentEnabled));
		type->set("isAutoIndentEnabled", OtFunction::create(&OtTextEditorWidgetClass::isAutoIndentEnabled));
		type->set("setShowWhitespacesEnabled", OtFunction::create(&OtTextEditorWidgetClass::setShowWhitespacesEnabled));
		type->set("isShowWhitespacesEnabled", OtFunction::create(&OtTextEditorWidgetClass::isShowWhitespacesEnabled));
		type->set("setShowLineNumbersEnabled", OtFunction::create(&OtTextEditorWidgetClass::setShowLineNumbersEnabled));
		type->set("isShowLineNumbersEnabled", OtFunction::create(&OtTextEditorWidgetClass::isShowLineNumbersEnabled));
		type->set("setShowMatchingBrackets", OtFunction::create(&OtTextEditorWidgetClass::setShowMatchingBrackets));
		type->set("isShowingMatchingBrackets", OtFunction::create(&OtTextEditorWidgetClass::isShowingMatchingBrackets));
		type->set("setCompletePairedGlyphs", OtFunction::create(&OtTextEditorWidgetClass::setCompletePairedGlyphs));
		type->set("isCompletingPairedGlyphs", OtFunction::create(&OtTextEditorWidgetClass::isCompletingPairedGlyphs));
		type->set("setOverwriteEnabled", OtFunction::create(&OtTextEditorWidgetClass::setOverwriteEnabled));
		type->set("isOverwriteEnabled", OtFunction::create(&OtTextEditorWidgetClass::isOverwriteEnabled));

		type->set("setLanguage", OtFunction::create(&OtTextEditorWidgetClass::setLanguage));
		type->set("getLanguage", OtFunction::create(&OtTextEditorWidgetClass::getLanguage));

		type->set("setText", OtFunction::create(&OtTextEditorWidgetClass::setText));
		type->set("getText", OtFunction::create(&OtTextEditorWidgetClass::getText));
		type->set("isEmpty", OtFunction::create(&OtTextEditorWidgetClass::isEmpty));
		type->set("lineCount", OtFunction::create(&OtTextEditorWidgetClass::lineCount));

		type->set("cut", OtFunction::create(&OtTextEditorWidgetClass::cut));
		type->set("copy", OtFunction::create(&OtTextEditorWidgetClass::copy));
		type->set("paste", OtFunction::create(&OtTextEditorWidgetClass::paste));
		type->set("undo", OtFunction::create(&OtTextEditorWidgetClass::undo));
		type->set("redo", OtFunction::create(&OtTextEditorWidgetClass::redo));
		type->set("canUndo", OtFunction::create(&OtTextEditorWidgetClass::canUndo));
		type->set("canRedo", OtFunction::create(&OtTextEditorWidgetClass::canRedo));
		type->set("getUndoIndex", OtFunction::create(&OtTextEditorWidgetClass::getUndoIndex));
	}

	return type;
}
