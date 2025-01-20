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
#include "OtMessageBus.h"
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
//	OtTextEditor::renderMenu
//

void OtTextEditor::renderMenu(bool canRun) {
	// create menubar
	if (ImGui::BeginMenuBar()) {
		renderFileMenu(canRun);

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", " " OT_UI_SHORTCUT "Z", nullptr, editor.CanUndo())) { editor.Undo(); focusOnEditor = true; }
#if __APPLE__
			if (ImGui::MenuItem("Redo", "^" OT_UI_SHORTCUT "Z", nullptr, editor.CanRedo())) { editor.Redo(); focusOnEditor = true; }
#else
			if (ImGui::MenuItem("Redo", " " OT_UI_SHORTCUT "Y", nullptr, editor.CanRedo())) { editor.Redo(); focusOnEditor = true; }
#endif

			ImGui::Separator();
			if (ImGui::MenuItem("Cut", " " OT_UI_SHORTCUT "X", nullptr, editor.AnyCursorHasSelection())) { editor.Cut(); focusOnEditor = true; }
			if (ImGui::MenuItem("Copy", " " OT_UI_SHORTCUT "C", nullptr, editor.AnyCursorHasSelection())) { editor.Copy(); focusOnEditor = true; }
			if (ImGui::MenuItem("Paste", " " OT_UI_SHORTCUT "V", nullptr, ImGui::GetClipboardText() != nullptr)) { editor.Paste(); focusOnEditor = true; }

			ImGui::Separator();
			if (ImGui::MenuItem("Tabs To Spaces")) { editor.TabsToSpaces(); focusOnEditor = true; }
			if (ImGui::MenuItem("Spaces To Tabs")) { editor.SpacesToTabs(); focusOnEditor = true; }
			if (ImGui::MenuItem("Strip Trailing Whitespaces")) { editor.StripTrailingWhitespaces(); focusOnEditor = true; }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Selection")) {
			if (ImGui::MenuItem("Select All", " " OT_UI_SHORTCUT "A", nullptr, !editor.IsEmpty())) { editor.SelectAll(); }
			ImGui::Separator();

			if (ImGui::MenuItem("Indent Line(s)", " " OT_UI_SHORTCUT "]", nullptr, !editor.IsEmpty())) { editor.IndentLines(); focusOnEditor = true; }
			if (ImGui::MenuItem("Deindent Line(s)", " " OT_UI_SHORTCUT "[", nullptr, !editor.IsEmpty())) { editor.DeindentLines(); focusOnEditor = true; }
			if (ImGui::MenuItem("Move Line(s) Up", " " OT_UI_SHORTCUT "\u2191", nullptr, !editor.IsEmpty())) { editor.MoveUpLines(); focusOnEditor = true; }
			if (ImGui::MenuItem("Move Line(s) Down", " " OT_UI_SHORTCUT "\u2193", nullptr, !editor.IsEmpty())) { editor.MoveDownLines(); focusOnEditor = true; }
			if (ImGui::MenuItem("Toggle Comments", " " OT_UI_SHORTCUT "/", nullptr, editor.HasLanguage())) { editor.ToggleComments(); focusOnEditor = true; }
			ImGui::Separator();

			if (ImGui::MenuItem("To Uppercase", nullptr, nullptr, editor.AnyCursorHasSelection())) { editor.SelectionToUpperCase(); focusOnEditor = true; }
			if (ImGui::MenuItem("To Lowercase", nullptr, nullptr, editor.AnyCursorHasSelection())) { editor.SelectionToLowerCase(); focusOnEditor = true; }
			ImGui::Separator();

			if (ImGui::MenuItem("Add Next Occurrence", " " OT_UI_SHORTCUT "D", nullptr, editor.CurrentCursorHasSelection())) { editor.AddNextOccurrence(); focusOnEditor = true; }
			if (ImGui::MenuItem("Select All Occurrences", "^" OT_UI_SHORTCUT "D", nullptr, editor.CurrentCursorHasSelection())) { editor.SelectAllOccurrences(); focusOnEditor = true; }

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Toggle Console")) { OtMessageBus::send("toggleconsole"); }
			ImGui::Separator();

			bool flag;
			flag = editor.IsShowWhitespacesEnabled(); if (ImGui::MenuItem("Show Whitespaces", nullptr, &flag)) { editor.SetShowWhitespacesEnabled(flag); focusOnEditor = true; };
			flag = editor.IsShowLineNumbersEnabled(); if (ImGui::MenuItem("Show Line Numbers", nullptr, &flag)) { editor.SetShowLineNumbersEnabled(flag); focusOnEditor = true; };
			flag = editor.IsShowingMatchingBrackets(); if (ImGui::MenuItem("Show Matching Brackets", nullptr, &flag)) { editor.SetShowMatchingBrackets(flag); focusOnEditor = true; };
			flag = editor.IsCompletingPairedGlyphs(); if (ImGui::MenuItem("Complete Matching Glyphs", nullptr, &flag)) { editor.SetCompletePairedGlyphs(flag); focusOnEditor = true; };

			ImGui::Separator();
			if (ImGui::MenuItem("Clear Errors", nullptr, nullptr, editor.HasErrorMarkers())) { editor.ClearErrorMarkers(); focusOnEditor = true; }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Find")) {
			if (ImGui::MenuItem("Find", " " OT_UI_SHORTCUT "F")) { openFindReplace(); }
			if (ImGui::MenuItem("Find Next", " " OT_UI_SHORTCUT "G"), findText.size()) { findNext(); focusOnEditor = true; }
			if (ImGui::MenuItem("Find All", "^" OT_UI_SHORTCUT "G", findText.size())) { findAll(); focusOnEditor = true; }
			ImGui::Separator();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// handle keyboard shortcuts (if required)
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
		// handle menu shortcuts
		if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_F)) {
			openFindReplace();

		} else if (ImGui::IsKeyChordPressed(ImGuiMod_Shift | ImGuiMod_Ctrl | ImGuiKey_G)) {
			findAll();

		} else if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_G)) {
			findNext();
		}
	}
}


//
//	OtTextEditor::renderEditor
//

void OtTextEditor::renderEditor() {
	// get current position and available space
	auto pos = ImGui::GetCursorPos();
	auto available = ImGui::GetContentRegionAvail();

	// ensure editor has focus (if required)
	if (focusOnEditor) {
		ImGui::SetNextWindowFocus();
		focusOnEditor = false;
	}

	// render the text editor
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[static_cast<size_t>(OtUi::Font::editor)]);
	editor.Render("TextEditor");
	ImGui::PopFont();

	// scroll to line if required
	// (this has to be done here as the editor doesn't handle this well on open)
	if (scrollToLine) {
		editor.SetCursor(scrollToLine - 1, 0);
		scrollToLine = 0;
	}

	// render find/replace window (if required)
	if (findReplaceVisible) {
		// calculate sizes
		auto& style = ImGui::GetStyle();
		auto fieldWidth = 250.0f;

		auto replaceWidth = ImGui::CalcTextSize(" Replace ").x + style.FramePadding.x * 2.0f;
		auto replaceAllWidth = ImGui::CalcTextSize(" Replace All ").x + style.FramePadding.x * 2.0f;
		auto optionWidth = ImGui::CalcTextSize("Aa").x + style.FramePadding.x * 2.0f;

		auto windowHeight =
			style.ChildBorderSize * 2.0f +
			style.WindowPadding.y * 2.0f +
			ImGui::GetFrameHeight() * 2.0f +
			style.ItemSpacing.y;

		auto windowWidth =
			style.ChildBorderSize * 2.0f +
			style.WindowPadding.x * 2.0f +
			fieldWidth + style.ItemSpacing.x +
			replaceWidth + style.ItemSpacing.x +
			replaceAllWidth + style.ItemSpacing.x +
			optionWidth * 3.0f + style.ItemSpacing.x * 2.0f;

		// create window
		ImGui::SetCursorPos(ImVec2(
			pos.x + available.x - windowWidth - style.ScrollbarSize - style.ItemSpacing.x,
			pos.y + style.ItemSpacing.y * 2.0f));

		ImGui::BeginChild("find-replace", ImVec2(windowWidth, windowHeight), ImGuiChildFlags_Borders);

		ImGui::SetNextItemWidth(fieldWidth);

		if (focusOnFind) {
			ImGui::SetKeyboardFocusHere();
			focusOnFind = false;
		}

		if (OtUi::inputString("###find", &findText, ImGuiInputTextFlags_AutoSelectAll)) {
			if (findText.size()) {
				editor.SelectFirstOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);

			} else {
				editor.ClearCursors();
			}
		}

		if (ImGui::IsItemDeactivatedAfterEdit()) {
			focusOnEditor = true;
		}

		if (!findText.size()) {
			ImGui::BeginDisabled();
		}

		ImGui::SameLine();

		if (ImGui::Button("Find", ImVec2(replaceWidth, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (ImGui::Button("Find All", ImVec2(replaceAllWidth, 0.0f))) {
			findAll();
		}

		if (!findText.size()) {
			ImGui::EndDisabled();
		}

		ImGui::SameLine();

		if (OtUi::latchButton("Aa", &caseSensitiveFind, ImVec2(optionWidth, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (OtUi::latchButton("[]", &wholeWordFind, ImVec2(optionWidth, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (ImGui::Button("x", ImVec2(optionWidth, 0.0f))) {
			findReplaceVisible = false;
			focusOnEditor = true;
		}

		ImGui::SetNextItemWidth(fieldWidth);
		OtUi::inputText("###replace", &replaceText);
		ImGui::SameLine();

		if (!findText.size() || !replaceText.size()) {
			ImGui::BeginDisabled();
		}

		if (ImGui::Button("Replace", ImVec2(replaceWidth, 0.0f))) {
			replace();
		}

		ImGui::SameLine();

		if (ImGui::Button("Replace All", ImVec2(replaceAllWidth, 0.0f))) {
			replaceAll();
		}

		if (!findText.size() || !replaceText.size()) {
			ImGui::EndDisabled();
		}

		ImGui::EndChild();
	}
}


//
//	OtTextEditor::findReplace
//

void OtTextEditor::openFindReplace() {
	findReplaceVisible = true;
	focusOnFind = true;
}


//
//	OtTextEditor::find
//

void OtTextEditor::find() {
	if (findText.size()) {
		editor.SelectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtTextEditor::findNext
//

void OtTextEditor::findNext() {
	if (findText.size()) {
		editor.SelectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtTextEditor::findAll
//

void OtTextEditor::findAll() {
	if (findText.size()) {
		editor.SelectAllOccurrencesOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtTextEditor::replace
//

void OtTextEditor::replace() {
	if (findText.size()) {
		if (!editor.AnyCursorHasSelection()) {
			editor.SelectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		}

		editor.ReplaceTextInCurrentCursor(replaceText);
		editor.SelectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtTextEditor::replaceAll
//

void OtTextEditor::replaceAll() {
	if (findText.size()) {
		editor.SelectAllOccurrencesOf(findText, caseSensitiveFind, wholeWordFind);
		editor.ReplaceTextInAllCursors(replaceText);
		focusOnEditor = true;
	}
}
