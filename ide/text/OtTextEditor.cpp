//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <map>

#include "imgui.h"

#include "OtText.h"

#include "OtUi.h"

#include "OtTextEditor.h"


//
//	OtTextEditor::OtTextEditor
//

OtTextEditor::OtTextEditor() {
	diff.SetSideBySideMode(true);
}


//
//	OtTextEditor::clear
//

void OtTextEditor::clear() {
	editor.SetText("");
	originalText = "";
}


//
//	OtTextEditor::load
//

void OtTextEditor::load() {
	std::string text;
	OtText::load(path, text);
	editor.SetText(text);
	originalText = text;
	version = editor.GetUndoIndex();
}


//
//	OtTextEditor::save
//

void OtTextEditor::save() {
	// reset error markers
	editor.ClearMarkers();

	// strip all trailing whitespaces to make text look better
	editor.StripTrailingWhitespaces();

	// save to file and baseline version (we still can undo back to before save but "dirty" tracking works)
	OtText::save(path, editor.GetText());
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

		ImGui::Separator();
		if (ImGui::MenuItem("Show Diff", " " OT_UI_SHORTCUT "I")) { showDiff(); }

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
		flag = editor.IsShowPanScrollIndicatorEnabled(); if (ImGui::MenuItem("Show Pan/Scroll Indicator", nullptr, &flag)) { editor.SetShowPanScrollIndicatorEnabled(flag); };
		flag = editor.IsMiddleMousePanMode(); if (ImGui::MenuItem("Middle Mouse Pan Mode", nullptr, &flag)) { if (flag) editor.SetMiddleMousePanMode(); else editor.SetMiddleMouseScrollMode(); };

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


//
//	OtTextEditor::handleShortcuts
//

void OtTextEditor::handleShortcuts() {
	if (ImGui::IsKeyDown(ImGuiMod_Ctrl)) {
		if (ImGui::IsKeyPressed(ImGuiKey_I)) { showDiff(); }
	}
}


//
//	OtTextEditor::renderEditor
//

void OtTextEditor::renderEditor() {
	// render the text editor
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[static_cast<size_t>(OtUi::Font::editor)], 0.0f);
	editor.Render("TextEditor");
	ImGui::PopFont();

	auto viewport = ImGui::GetMainViewport();
	ImVec2 center = viewport->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Changes since Opening File##diff", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		diff.Render("diff", viewport->Size * 0.8f, true);

		ImGui::Separator();
		bool sideBySide = diff.GetSideBySideMode();

		if (ImGui::Checkbox("Show side-by-side", &sideBySide)) {
			diff.SetSideBySideMode(sideBySide);
		}

		ImGui::SameLine();
		static constexpr float buttonWidth = 80.0f;
		OtUi::hSpacer(ImGui::GetContentRegionAvail().x - buttonWidth);

		if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}


//
//	OtTextEditor::showDiff
//

void OtTextEditor::showDiff() {
	diff.SetText(originalText, editor.GetText());
	ImGui::OpenPopup("Changes since Opening File##diff");
}
