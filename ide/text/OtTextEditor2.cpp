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

#include "OtTextEditor2.h"


//
//	Color palette
//

const static OtTextWidget::Palette colorPalette = { {
	IM_COL32(224, 224, 224, 255),	// standard
	IM_COL32(197, 134, 192, 255),	// keyword
	IM_COL32(181, 206, 168, 255),	// number
	IM_COL32(206, 145, 120, 255),	// string
	IM_COL32(206, 145, 120, 255),	// character
	IM_COL32(255, 255, 255, 255),	// punctuation
	IM_COL32(128, 128,  64, 255),	// preprocessor
	IM_COL32(156, 220, 254, 255),	// identifier
	IM_COL32( 79, 193, 255, 255),	// known identifier
	IM_COL32(106, 153,  85, 255),	// comment
	IM_COL32( 30,  30,  30, 255),	// background
	IM_COL32(224, 224, 224, 255),	// cursor
	IM_COL32( 32,  96, 160, 255),	// selection
	IM_COL32(128,   0,  32, 255),	// errorMarker
	IM_COL32( 90,  90,  90, 255),	// whitespace
	IM_COL32(128, 128, 144, 255),	// line number
	IM_COL32(224, 224, 240, 255),	// current line number
} };


//
//	OtTextEditor2::OtTextEditor2
//

OtTextEditor2::OtTextEditor2() {
	editor.SetShowWhitespacesEnabled(true);
	editor.SetPalette(colorPalette);
}


//
//	OtTextEditor2::clear
//

void OtTextEditor2::clear() {
	editor.SetText("");
}


//
//	OtTextEditor2::load
//

void OtTextEditor2::load() {
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
//	OtTextEditor2::save
//

void OtTextEditor2::save() {
	// strip all trailing whitespaces to make text look better
	editor.StripTrailingWhitespaces();

	// save to file and baseline version (we still can undo back to before save but "dirty" tracking works)
	std::ofstream stream(path.c_str());
	stream << editor.GetText();
	stream.close();
	version = editor.GetUndoIndex();
}


//
//	OtTextEditor2::renderMenu
//

void OtTextEditor2::renderMenu(bool canRun) {
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

			ImGui::Separator();
			if (ImGui::MenuItem("Tabs To Spaces")) { editor.TabsToSpaces(); focusOnEditor = true; }
			if (ImGui::MenuItem("Spaces To Tabs")) { editor.SpacesToTabs(); focusOnEditor = true; }
			if (ImGui::MenuItem("Strip Trailing Whitespaces")) { editor.StripTrailingWhitespaces(); focusOnEditor = true; }
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View")) {
			if (ImGui::MenuItem("Toggle Console")) { OtMessageBus::send("toggleconsole"); }
			ImGui::Separator();

			bool flag;
			flag = editor.IsShowWhitespacesEnabled(); if (ImGui::MenuItem("Show Whitespaces", nullptr, &flag)) { editor.SetShowWhitespacesEnabled(flag); };
			flag = editor.IsShowLineNumbersEnabled(); if (ImGui::MenuItem("Show Line Numbers", nullptr, &flag)) { editor.SetShowLineNumbersEnabled(flag); };
			flag = editor.IsShowingMatchingBrackets(); if (ImGui::MenuItem("Show Matching Brackets", nullptr, &flag)) { editor.SetShowMatchingBrackets(flag); };
			flag = editor.IsCompletingPairedGlyphs(); if (ImGui::MenuItem("Complete Matchng Glyphs", nullptr, &flag)) { editor.SetCompletePairedGlyphs(flag); };

			ImGui::Separator();
			if (ImGui::MenuItem("Clear Errors", nullptr, nullptr, editor.HasErrorMarkers())) { editor.ClearErrorMarkers(); }
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
//	OtTextEditor2::renderEditor
//

void OtTextEditor2::renderEditor() {
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
//	OtTextEditor2::findReplace
//

void OtTextEditor2::openFindReplace() {
	findReplaceVisible = true;
	focusOnFind = true;
}


//
//	OtTextEditor2::find
//

void OtTextEditor2::find() {
	if (findText.size()) {
		editor.SelectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtTextEditor2::findNext
//

void OtTextEditor2::findNext() {
	if (findText.size()) {
		editor.SelectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtTextEditor2::findAll
//

void OtTextEditor2::findAll() {
	if (findText.size()) {
		editor.SelectAllOccurrencesOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	OtTextEditor2::replace
//

void OtTextEditor2::replace() {
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
//	OtTextEditor2::replaceAll
//

void OtTextEditor2::replaceAll() {
	if (findText.size()) {
		editor.SelectAllOccurrencesOf(findText, caseSensitiveFind, wholeWordFind);
		editor.ReplaceTextInAllCursors(replaceText);
		focusOnEditor = true;
	}
}
