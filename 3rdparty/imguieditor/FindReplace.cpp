//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "TextEditor.h"


//
//	latchButton
//

static bool latchButton(const char* label, bool* value, const ImVec2& size) {
	auto changed = false;
	ImVec4* colors = ImGui::GetStyle().Colors;

	if (*value) {
		ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_ButtonActive]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_TableBorderLight]);

	} else {
		ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_TableBorderLight]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[ImGuiCol_TableBorderLight]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[ImGuiCol_ButtonActive]);
	}

	ImGui::Button(label, size);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
		*value = !*value;
		changed = true;
	}

	ImGui::PopStyleColor(3);
	return changed;
}


//
//	inputString
//

static bool inputString(const char* label, std::string* value, ImGuiInputTextFlags flags=ImGuiInputTextFlags_None) {
	flags |=
		ImGuiInputTextFlags_NoUndoRedo |
		ImGuiInputTextFlags_CallbackResize;

	return ImGui::InputText(label, (char*) value->c_str(), value->capacity() + 1, flags, [](ImGuiInputTextCallbackData* data) {
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
			std::string* value = (std::string*) data->UserData;
			value->resize(data->BufTextLen);
			data->Buf = (char*) value->c_str();
		}

		return 0;
	}, value);
}


//
//	TextEditor::renderFindReplace
//

void TextEditor::renderFindReplace(ImVec2 pos, float width) {
	// render find/replace window (if required)
	if (findReplaceVisible) {
		// save current screen position
		auto currentScreenPosition = ImGui::GetCursorScreenPos();

		// calculate sizes
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 4.0f));
		auto& style = ImGui::GetStyle();
		auto fieldWidth = 250.0f;

		auto button1Width = ImGui::CalcTextSize(findButtonLabel.c_str()).x + style.ItemSpacing.x * 2.0f;
		auto button2Width = ImGui::CalcTextSize(findAllButtonLabel.c_str()).x + style.ItemSpacing.x * 2.0f;
		auto optionWidth = ImGui::CalcTextSize("Aa").x + style.ItemSpacing.x * 2.0f;

		if (!readOnly) {
			button1Width = std::max(button1Width, ImGui::CalcTextSize(replaceButtonLabel.c_str()).x + style.ItemSpacing.x * 2.0f);
			button2Width = std::max(button2Width, ImGui::CalcTextSize(replaceAllButtonLabel.c_str()).x + style.ItemSpacing.x * 2.0f);
		}

		auto windowHeight =
			style.ChildBorderSize * 2.0f +
			style.WindowPadding.y * 2.0f +
			ImGui::GetFrameHeight() +
			(readOnly ? 0.0f : (style.ItemSpacing.y + ImGui::GetFrameHeight()));

		auto windowWidth =
			style.ChildBorderSize * 2.0f +
			style.WindowPadding.x * 2.0f +
			fieldWidth + style.ItemSpacing.x +
			button1Width + style.ItemSpacing.x +
			button2Width + style.ItemSpacing.x +
			optionWidth * 3.0f + style.ItemSpacing.x * 2.0f;

		// create window
		ImGui::SetNextWindowPos(ImVec2(
			pos.x + width - windowWidth - style.ItemSpacing.x,
			pos.y + style.ItemSpacing.y * 2.0f));

		ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));
		ImGui::SetNextWindowBgAlpha(0.75f);

		ImGui::BeginChild("find-replace", ImVec2(windowWidth, windowHeight), ImGuiChildFlags_Borders);
		ImGui::SetNextItemWidth(fieldWidth);

		if (focusOnFind) {
			ImGui::SetKeyboardFocusHere();
			focusOnFind = false;
		}

		if (inputString("###find", &findText, ImGuiInputTextFlags_AutoSelectAll)) {
			if (findText.size()) {
				selectFirstOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);

			} else {
				cursors.clearAll();
			}
		}

		if (ImGui::IsItemDeactivated() && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))){
			focusOnEditor = true;
		}

		bool disableFindButtons = !findText.size();

		if (disableFindButtons) {
			ImGui::BeginDisabled();
		}

		ImGui::SameLine();

		if (ImGui::Button(findButtonLabel.c_str(), ImVec2(button1Width, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (ImGui::Button(findAllButtonLabel.c_str(), ImVec2(button2Width, 0.0f))) {
			findAll();
		}

		if (disableFindButtons) {
			ImGui::EndDisabled();
		}

		ImGui::SameLine();

		if (latchButton("Aa", &caseSensitiveFind, ImVec2(optionWidth, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (latchButton("[]", &wholeWordFind, ImVec2(optionWidth, 0.0f))) {
			find();
		}

		ImGui::SameLine();

		if (ImGui::Button("x", ImVec2(optionWidth, 0.0f))) {
			closeFindReplace();
		}

		if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			closeFindReplace();
		}

		if (!readOnly) {
			ImGui::SetNextItemWidth(fieldWidth);
			inputString("###replace", &replaceText);
			ImGui::SameLine();

			bool disableReplaceButtons = !findText.size() || !replaceText.size();

			if (disableReplaceButtons) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button(replaceButtonLabel.c_str(), ImVec2(button1Width, 0.0f))) {
				replace();
			}

			ImGui::SameLine();

			if (ImGui::Button(replaceAllButtonLabel.c_str(), ImVec2(button2Width, 0.0f))) {
				replaceAll();
			}

			if (disableReplaceButtons) {
				ImGui::EndDisabled();
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::SetCursorScreenPos(currentScreenPosition);
	}
}


//
//	TextEditor::selectFirstOccurrenceOf
//

void TextEditor::selectFirstOccurrenceOf(const std::string_view& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(Coordinate(0, 0), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		makeCursorVisible();

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::selectNextOccurrenceOf
//

void TextEditor::selectNextOccurrenceOf(const std::string_view& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(cursors.getCurrent().getSelectionEnd(), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		makeCursorVisible();

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::selectAllOccurrencesOf
//

void TextEditor::selectAllOccurrencesOf(const std::string_view& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(Coordinate(0, 0), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		bool done = false;

		while (!done) {
			Coordinate nextStart, nextEnd;
			document.findText(cursors.getCurrent().getSelectionEnd(), text, caseSensitive, wholeWord, nextStart, nextEnd);

			if (nextStart == start && nextEnd == end) {
				done = true;

			} else {
				cursors.addCursor(nextStart, nextEnd);
			}
		}

		makeCursorVisible();

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::addNextOccurrence
//

void TextEditor::addNextOccurrence() {

	auto cursor = cursors.getCurrent();
	auto text = document.getSectionText(cursor.getSelectionStart(), cursor.getSelectionEnd());
	Coordinate start, end;

	if (document.findText(cursor.getSelectionEnd(), text, true, false, start, end)) {
		cursors.addCursor(start, end);
	}
}


//
//	TextEditor::addNextOccurrences
//

void TextEditor::selectAllOccurrences() {
	auto cursor = cursors.getCurrent();
	auto text = document.getSectionText(cursor.getSelectionStart(), cursor.getSelectionEnd());
	selectAllOccurrencesOf(text, true, false);
}


//
//	TextEditor::replaceTextInCurrentCursor
//

void TextEditor::replaceTextInCurrentCursor(const std::string_view& text) {
	auto transaction = startTransaction();

	// first delete old text
	auto cursor = cursors.getCurrentAsIterator();
	auto start = cursor->getSelectionStart();
	auto end = cursor->getSelectionEnd();
	deleteText(transaction, start, end);
	cursors.adjustForDelete(cursor, start, end);

	// now insert new text
	Coordinate newEnd = insertText(transaction, start, text);
	cursor->update(newEnd, false);
	cursors.adjustForInsert(cursor, start, newEnd);

	endTransaction(transaction);
}


//
//	TextEditor::replaceTextInAllCursors
//

void TextEditor::replaceTextInAllCursors(const std::string_view& text) {
	auto transaction = startTransaction();
	insertTextIntoAllCursors(transaction, text);
	endTransaction(transaction);
}


//
//	TextEditor::replaceSectionText
//

void TextEditor::replaceSectionText(const Coordinate& start, const Coordinate& end, const std::string_view& text) {
	auto transaction = startTransaction();
	deleteText(transaction, start, end);
	auto newEnd = insertText(transaction, start, text);
	cursors.clearAdditional();
	cursors.getMain().update(newEnd, newEnd);
	endTransaction(transaction);
}


//
//	TextEditor::openFindReplace
//

void TextEditor::openFindReplace() {
	// get main cursor location
	auto cursor = cursors.getMain();

	// see if we have a current selection that's on one line
	if (cursor.hasSelection()) {
		if (cursor.getSelectionStart().line == cursor.getSelectionEnd().line) {
			// use it as the default search
			findText = document.getSectionText(cursor.getSelectionStart(), cursor.getSelectionEnd());
		}

	} else {
		// if cursor is inside "real" word, use that as the default
		auto start = document.findWordStart(cursor.getSelectionStart(), true);
		auto end = document.findWordEnd(cursor.getSelectionStart(), true);

		if (start != end) {
			findText = document.getSectionText(start, end);
		}
	}

	findReplaceVisible = true;
	focusOnFind = true;
}


//
//	TextEditor::closeFindReplace
//

void TextEditor::closeFindReplace() {
	findReplaceVisible = false;
	focusOnFind = false;
	focusOnEditor = true;
}


//
//	TextEditor::find
//

void TextEditor::find() {
	if (findText.size()) {
		selectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	TextEditor::findNext
//

void TextEditor::findNext() {
	if (findText.size()) {
		selectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	TextEditor::findAll
//

void TextEditor::findAll() {
	if (findText.size()) {
		selectAllOccurrencesOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	TextEditor::replace
//

void TextEditor::replace() {
	if (findText.size()) {
		if (!cursors.anyHasSelection()) {
			selectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		}

		replaceTextInCurrentCursor(replaceText);
		selectNextOccurrenceOf(findText, caseSensitiveFind, wholeWordFind);
		focusOnEditor = true;
	}
}


//
//	TextEditor::replaceAll
//

void TextEditor::replaceAll() {
	if (findText.size()) {
		selectAllOccurrencesOf(findText, caseSensitiveFind, wholeWordFind);
		replaceTextInAllCursors(replaceText);
		focusOnEditor = true;
	}
}
