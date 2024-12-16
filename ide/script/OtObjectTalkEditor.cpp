//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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

#include "OtMessageBus.h"
#include "OtUi.h"

#include "OtObjectTalkEditor.h"
#include "OtObjectTalkLanguage.h"


//
//	Color palette
//

const static TextEditor::Palette colorPalette = { {
	0xd4d4d4ff,	// Default
	0xc586c0ff,	// Keyword
	0xb5cea8ff,	// Number
	0xce9178ff,	// String
	0xce9178ff, // Char literal
	0xffffffff, // Punctuation
	0x808040ff,	// Preprocessor
	0x9cdcfeff, // Identifier
	0x4fc1ffff, // Known identifier
	0x63c6abff, // Preproc identifier
	0x6a9955ff, // Comment (single line)
	0x6a9955ff, // Comment (multi line)
	0x1e1e1eff, // Background
	0xe0e0e0ff, // Cursor
	0x2060a0ff, // Selection
	0x800020ff, // ErrorMarker
	0x404040ff, // ControlCharacter
	0x0080f040, // Breakpoint
	0x6e7681ff, // Line number
	0xccccccff, // Current line number
} };


//
//	OtObjectTalkEditor::OtObjectTalkEditor
//

OtObjectTalkEditor::OtObjectTalkEditor() {
	editor.SetLanguageDefinition(OtObjectTalkLanguageGetDefinition());
	editor.SetPalette(colorPalette);
	editor.SetLineSpacing(1.25f);
	editor.SetShowWhitespacesEnabled(true);
	editor.SetShortTabsEnabled(true);
	editor.SetShowMatchingBrackets(true);
	editor.SetCompletePairedGlyphs(true);
}


//
//	OtObjectTalkEditor::clear
//

void OtObjectTalkEditor::clear() {
	editor.SetText("");
}


//
//	OtObjectTalkEditor::load
//

void OtObjectTalkEditor::load() {
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
//	OtObjectTalkEditor::save
//

void OtObjectTalkEditor::save() {
	std::ofstream stream(path.c_str());
	stream << editor.GetText();
	stream.close();
	version = editor.GetUndoIndex();
}


//
//	OtObjectTalkEditor::renderMenu
//

void OtObjectTalkEditor::renderMenu(bool canRun) {
	// create menubar
	if (ImGui::BeginMenuBar()) {
		renderFileMenu(canRun);

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", OT_UI_SHORTCUT "Z", nullptr, editor.CanUndo())) { editor.Undo(); }
#if __APPLE__
			if (ImGui::MenuItem("Redo", "^" OT_UI_SHORTCUT "Z", nullptr, editor.CanRedo())) { editor.Redo(); }
#else
			if (ImGui::MenuItem("Redo", OT_UI_SHORTCUT "Y", nullptr, editor.CanRedo())) { editor.Redo(); }
#endif

			ImGui::Separator();
			if (ImGui::MenuItem("Cut", OT_UI_SHORTCUT "X", nullptr, editor.AnyCursorHasSelection())) { editor.Cut(); }
			if (ImGui::MenuItem("Copy", OT_UI_SHORTCUT "C", nullptr, editor.AnyCursorHasSelection())) { editor.Copy(); }
			if (ImGui::MenuItem("Paste", OT_UI_SHORTCUT "V", nullptr, ImGui::GetClipboardText() != nullptr)) { editor.Paste(); }

			ImGui::Separator();
			if (ImGui::MenuItem("Select All", OT_UI_SHORTCUT "A", nullptr, !editor.IsEmpty())) { editor.SelectAll(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Toggle Console")) { OtMessageBus::send("toggleconsole"); }
			ImGui::Separator();

			bool flag;
			flag = editor.IsShowWhitespacesEnabled(); if (ImGui::MenuItem("Show Whitespaces", nullptr, &flag)) { editor.SetShowWhitespacesEnabled(flag); };
			flag = editor.IsShowLineNumbersEnabled(); if (ImGui::MenuItem("Show Line Numbers", nullptr, &flag)) { editor.SetShowLineNumbersEnabled(flag); };
			flag = editor.IsShortTabsEnabled(); if (ImGui::MenuItem("Show Short Tabs", nullptr, &flag)) { editor.SetShortTabsEnabled(flag); };
			flag = editor.IsShowingMatchingBrackets(); if (ImGui::MenuItem("Show Matching Brackets", nullptr, &flag)) { editor.SetShowMatchingBrackets(flag); };
			flag = editor.IsCompletingPairedGlyphs(); if (ImGui::MenuItem("Complete Matchng Glyphs", nullptr, &flag)) { editor.SetCompletePairedGlyphs(flag); };

			ImGui::Separator();
			if (ImGui::MenuItem("Clear Errors")) { clearError(); }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Find")) {
			if (ImGui::MenuItem("Find", OT_UI_SHORTCUT "F")) { openFindReplace(); }
			if (ImGui::MenuItem("Find Next", OT_UI_SHORTCUT "G"), findText.size()) { findNext(); }
			if (ImGui::MenuItem("Find All", "^" OT_UI_SHORTCUT "G", findText.size())) { findAll(); }
			ImGui::Separator();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	// handle keyboard shortcuts (if required)
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) {
		// handle menu shortcuts
		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_F)) {
			openFindReplace();

		} else if (ImGui::Shortcut(ImGuiMod_Shift | ImGuiMod_Ctrl | ImGuiKey_G)) {
			findAll();

		} else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_G)) {
			findNext();
		}
	}
}


//
//	OtObjectTalkEditor::renderEditor
//

void OtObjectTalkEditor::renderEditor() {
	// get current position and available space
	auto pos = ImGui::GetCursorPos();
	auto available = ImGui::GetContentRegionAvail();

	// ensure editor has focus (if required)
	if (focusOnEditor) {
		ImGui::SetNextWindowFocus();
		focusOnEditor = false;
	}

	// render the text editor
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[OtUi::editorFont]);
	editor.Render("TextEditor");
	ImGui::PopFont();

	// scroll to line if required
	// (this has to be done here as the editor doesn't handle this well on open)
	if (scrollToLine) {
		editor.SetCursorPosition(scrollToLine - 1, 0);
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
				editor.SetCursorPosition(0, 0);
				editor.SelectNextOccurrenceOf(findText.c_str(), (int) findText.size(), caseSensitiveFind, wholeWordFind);

			} else {
				editor.ClearSelections();
			}
		}

		if (!findText.size()) {
			ImGui::BeginDisabled();
		}

		ImGui::SameLine();

		if (ImGui::Button("Find", ImVec2(replaceWidth, 0.0f))) {
			findNext();
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
			editor.SetCursorPosition(0, 0);
			findNext();
		}

		ImGui::SameLine();

		if (OtUi::latchButton("[]", &wholeWordFind, ImVec2(optionWidth, 0.0f))) {
			editor.SetCursorPosition(0, 0);
			findNext();
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
//	OtObjectTalkEditor::highlightError
//

void OtObjectTalkEditor::highlightError(size_t line, const std::string& error) {
	std::map<int, std::string> markers;
	markers[(int) line] = error;
	editor.SetErrorMarkers(markers);
	scrollToLine = (int) line;
}


//
//	OtObjectTalkEditor::clearError
//

void OtObjectTalkEditor::clearError() {
	std::map<int, std::string> markers;
	editor.SetErrorMarkers(markers);
}


//
//	OtObjectTalkEditor::findReplace
//

void OtObjectTalkEditor::openFindReplace() {
	findReplaceVisible = true;
	focusOnFind = true;
}


//
//	OtObjectTalkEditor::findNext
//

void OtObjectTalkEditor::findNext() {
	if (findText.size()) {
		editor.SelectNextOccurrenceOf(findText.c_str(), (int) findText.size(), caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtObjectTalkEditor::findAll
//

void OtObjectTalkEditor::findAll() {
	if (findText.size()) {
		editor.SelectAllOccurrencesOf(findText.c_str(), (int) findText.size(), caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtObjectTalkEditor::replace
//

void OtObjectTalkEditor::replace() {
	if (findText.size()) {
		if (!editor.AnyCursorHasSelection()) {
			editor.SelectNextOccurrenceOf(findText.c_str(), (int) findText.size(), caseSensitiveFind, wholeWordFind);
		}

		editor.ReplaceTextInCurrentCursor(replaceText);
		editor.SelectNextOccurrenceOf(findText.c_str(), (int) findText.size(), caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtObjectTalkEditor::replaceAll
//

void OtObjectTalkEditor::replaceAll() {
	if (findText.size()) {
		editor.SelectAllOccurrencesOf(findText.c_str(), (int) findText.size(), caseSensitiveFind, wholeWordFind);
		editor.ReplaceTextInAllCursors(replaceText);
		editor.ClearExtraCursors();
		focusOnEditor = true;
	}
}
