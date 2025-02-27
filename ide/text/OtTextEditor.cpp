//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <fstream>
#include <map>

#include "imgui.h"

#include "OtCodePoint.h"
#include "OtUi.h"

#include "OtTextEditor.h"


//
//	OtTextEditor::clear
//

void OtTextEditor::clear() {
	editor.SetText("");
}


//
//	OtTextEditor::load
//

void OtTextEditor::load() {
	std::ifstream stream(path.c_str());
	std::string text;

	stream.seekg(0, std::ios::end);
	text.reserve(stream.tellg());
	stream.seekg(0, std::ios::beg);

	text.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
	stream.close();

	editor.SetText(text);
	version = editor.GetUndoIndex();
}


//
//	OtTextEditor::save
//

void OtTextEditor::save() {
	// strip all trailing whitespaces to make text look better
	editor.StripTrailingWhitespaces();

	// save to file and baseline version (we still can undo back to before save but "dirty" tracking works)
	std::ofstream stream(path.c_str());
	stream << editor.GetText();
	stream.close();
	version = editor.GetUndoIndex();
}


//
//	OtTextEditor::renderMenus
//

void OtTextEditor::renderMenus() {
	if (ImGui::BeginMenu("Edit")) {
		if (ImGui::MenuItem("Undo", " " OT_UI_SHORTCUT "Z", nullptr, editor.CanUndo())) { editor.Undo(); }
#if __APPLE__
		if (ImGui::MenuItem("Redo", "^" OT_UI_SHORTCUT "Z", nullptr, editor.CanRedo())) { editor.Redo(); }
#else
		if (ImGui::MenuItem("Redo", " " OT_UI_SHORTCUT "Y", nullptr, editor.CanRedo())) { editor.Redo(); }
#endif

		ImGui::Separator();
		if (ImGui::MenuItem("Cut", " " OT_UI_SHORTCUT "X", nullptr, editor.AnyCursorHasSelection())) { editor.Cut(); }
		if (ImGui::MenuItem("Copy", " " OT_UI_SHORTCUT "C", nullptr, editor.AnyCursorHasSelection())) { editor.Copy(); }
		if (ImGui::MenuItem("Paste", " " OT_UI_SHORTCUT "V", nullptr, ImGui::GetClipboardText() != nullptr)) { editor.Paste(); }

		ImGui::Separator();
		if (ImGui::MenuItem("Tabs To Spaces")) { editor.TabsToSpaces(); }
		if (ImGui::MenuItem("Spaces To Tabs")) { editor.SpacesToTabs(); }
		if (ImGui::MenuItem("Strip Trailing Whitespaces")) { editor.StripTrailingWhitespaces(); }
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Selection")) {
		if (ImGui::MenuItem("Select All", " " OT_UI_SHORTCUT "A", nullptr, !editor.IsEmpty())) { editor.SelectAll(); }
		ImGui::Separator();

		if (ImGui::MenuItem("Indent Line(s)", " " OT_UI_SHORTCUT "]", nullptr, !editor.IsEmpty())) { editor.IndentLines(); }
		if (ImGui::MenuItem("Deindent Line(s)", " " OT_UI_SHORTCUT "[", nullptr, !editor.IsEmpty())) { editor.DeindentLines(); }
		if (ImGui::MenuItem("Move Line(s) Up", " " OT_UI_SHORTCUT "\u2191", nullptr, !editor.IsEmpty())) { editor.MoveUpLines(); }
		if (ImGui::MenuItem("Move Line(s) Down", " " OT_UI_SHORTCUT "\u2193", nullptr, !editor.IsEmpty())) { editor.MoveDownLines(); }
		if (ImGui::MenuItem("Toggle Comments", " " OT_UI_SHORTCUT "/", nullptr, editor.HasLanguage())) { editor.ToggleComments(); }
		ImGui::Separator();

		if (ImGui::MenuItem("To Uppercase", nullptr, nullptr, editor.AnyCursorHasSelection())) { editor.SelectionToUpperCase(); }
		if (ImGui::MenuItem("To Lowercase", nullptr, nullptr, editor.AnyCursorHasSelection())) { editor.SelectionToLowerCase(); }
		ImGui::Separator();

		if (ImGui::MenuItem("Add Next Occurrence", " " OT_UI_SHORTCUT "D", nullptr, editor.CurrentCursorHasSelection())) { editor.AddNextOccurrence(); }
		if (ImGui::MenuItem("Select All Occurrences", "^" OT_UI_SHORTCUT "D", nullptr, editor.CurrentCursorHasSelection())) { editor.SelectAllOccurrences(); }

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View")) {
		bool flag;
		flag = editor.IsShowWhitespacesEnabled(); if (ImGui::MenuItem("Show Whitespaces", nullptr, &flag)) { editor.SetShowWhitespacesEnabled(flag); };
		flag = editor.IsShowLineNumbersEnabled(); if (ImGui::MenuItem("Show Line Numbers", nullptr, &flag)) { editor.SetShowLineNumbersEnabled(flag); };
		flag = editor.IsShowingMatchingBrackets(); if (ImGui::MenuItem("Show Matching Brackets", nullptr, &flag)) { editor.SetShowMatchingBrackets(flag); };
		flag = editor.IsCompletingPairedGlyphs(); if (ImGui::MenuItem("Complete Matching Glyphs", nullptr, &flag)) { editor.SetCompletePairedGlyphs(flag); };

		ImGui::Separator();
		if (ImGui::MenuItem("Clear Errors", nullptr, nullptr, editor.HasMarkers())) { editor.ClearMarkers(); }
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Find")) {
		if (ImGui::MenuItem("Find", " " OT_UI_SHORTCUT "F")) { editor.OpenFindReplaceWindow(); }
		if (ImGui::MenuItem("Find Next", " " OT_UI_SHORTCUT "G", nullptr, editor.HasFindString())) { editor.FindNext(); }
		if (ImGui::MenuItem("Find All", "^" OT_UI_SHORTCUT "G", nullptr, editor.HasFindString())) { editor.FindAll(); }
		ImGui::Separator();
		ImGui::EndMenu();
	}
}


#include <random>

static const char* demo[] = {
	"#!/usr/local/bin/ot\n",
	"\n",
	"//	ObjectTalk Scripting Language\n",
	"//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.\n",
	"//\n",
	"//	This work is licensed under the terms of the MIT license.\n",
	"//	For a copy, see <https://opensource.org/licenses/MIT>\n",
	"\n",
	"var classMembers = members(Array(function() {}).getClass());\n",
	"classMembers.sort();\n",
	"\n",
	"print(\"**Member Functions**\");\n",
	"print();\n",
	"print(\"| Function | Description |\");\n",
	"print(\"| ------ | ----------- |\");\n",
	"\n",
	"for member in classMembers {\n",
	"	if member.startsWith(\"__\") {\n",
	"		member = \"\\_\\_\" + member.right(member.len() - 2);\n",
	"	}\n",
	"\n",
	"	print(\"| \", member, \"() | |\");\n",
	"}\n"
};

static std::string randomDemo() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, sizeof(demo) / sizeof(*demo) - 1);
	std::string result;

	for (auto i = 0; i < 100; i++) {
		result += demo[distrib(gen)];
	}

	return result;
}


//
//	OtTextEditor::handleShortcuts
//

void OtTextEditor::handleShortcuts() {
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_P)) {
		editor.SetText(randomDemo());
		editor.SetCursor(55, 0);
		editor.ScrollToLine(50, TextEditor::Scroll::alignTop);
	}
}


//
//	OtTextEditor::renderEditor
//

void OtTextEditor::renderEditor() {
	// render the text editor
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[static_cast<size_t>(OtUi::Font::editor)]);
	editor.Render("TextEditor");
	ImGui::PopFont();
}
