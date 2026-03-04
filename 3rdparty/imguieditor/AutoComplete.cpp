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
//	renderSuggestion
//

static bool renderSuggestion(const std::string_view& suggestion, const std::string_view& searchTerm, bool selected) {
	// custom widget to render an autocomplete suggestion in the style of Visual Studio Code
	auto glyphPos = ImGui::GetCursorScreenPos();
	auto size = ImVec2(250.0f, ImGui::GetFrameHeightWithSpacing());
	auto clicked = ImGui::InvisibleButton("suggestion", size);

	auto drawList = ImGui::GetWindowDrawList();
	auto font = ImGui::GetFont();
	auto fontSize = ImGui::GetFontSize();
	auto glyphWidth = ImGui::CalcTextSize("#").x;

	// highlight selected item
	if (selected) {
		drawList->AddRectFilled(glyphPos, glyphPos + size, ImGui::GetColorU32(ImGuiCol_Header));
	}

	// process all UTF-8 glyphs in suggestion
	glyphPos += ImGui::GetStyle().FramePadding;
	auto suggestionEnd = suggestion.end();
	auto searchTermEnd = searchTerm.end();
	auto i = TextEditor::CodePoint::skipBOM(suggestion.begin(), suggestionEnd);
	auto j = TextEditor::CodePoint::skipBOM(searchTerm.begin(), searchTermEnd);

	while (i < suggestionEnd) {
		// get next glyph from suggestion
		ImWchar codepoint;
		i = TextEditor::CodePoint::read(i, suggestionEnd, &codepoint);

		// highlight glyph in suggestion that match search term
		auto color = ImGui::GetColorU32(ImGuiCol_Text);

		if (j < searchTermEnd) {
			ImWchar searchCodePoint;
			auto next = TextEditor::CodePoint::read(j, searchTermEnd, &searchCodePoint);

			if (searchCodePoint == codepoint) {
				color = ImGui::GetColorU32(ImGuiCol_TextLink);
				j = next;
			}
		}

		// render the glyph
		font->RenderChar(drawList, fontSize, glyphPos, color, codepoint);
		glyphPos.x += glyphWidth;
	}

	return clicked;
}


//
//	TextEditor::renderAutoComplete
//

void TextEditor::renderAutoComplete() {
	// see if we need to activate autocomplete mode
	if (activateAutoComplete) {
		// reset activation flag
		activateAutoComplete = false;

		// capture locations
		autoCompleteLocation = cursors.getMain().getSelectionEnd();
		autoCompleteStart = document.findWordStart(autoCompleteLocation, true);

		// update the autocomplete state
		updateAutoCompleteState();

		// handle cases where autocomplete request is ignored
		if(autoCompleteState.inComment && !autoCompleteConfig.triggerInComments) {
			return;
		}

		if(autoCompleteState.inString && !autoCompleteConfig.triggerInStrings) {
			return;
		}

		// get initial list of suggestions from the app
		refreshAutoCompleteSuggestions();

		// show autocomplete popup window
		ImGui::OpenPopup("AutoCompleteContextMenu");
		autoCompleteActive = true;
	}

	// only continue if autocomplete is active
	if (!autoCompleteActive) {
		return;
	}

	// see if cursor moved since last time
	auto newLocation = cursors.getMain().getSelectionEnd();

	if (newLocation != autoCompleteLocation) {
		// see if we need to deactivate auto complete because cursor is on new line
		if (newLocation.line != autoCompleteLocation.line) {
			deactivateAutoComplete = true;

		} else {
			// see if cursor moved away from current word
			auto newStart = document.findWordStart(newLocation, true);

			if (newStart == autoCompleteStart) {
				autoCompleteLocation = newLocation;

				// we deactivate autocomplete is the current location is the start
				if (autoCompleteLocation == autoCompleteStart) {
					deactivateAutoComplete = true;

				} else {
					updateAutoCompleteState();
					refreshAutoCompleteSuggestions();
				}

			} else {
				deactivateAutoComplete = true;
			}
		}
	}

	// close autocomplete when user hits escape key
	if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
		deactivateAutoComplete = true;
	}

	// open popup window
	auto cursorScreenPos = ImGui::GetCursorScreenPos();

	ImGui::SetNextWindowPos(ImVec2(
		cursorScreenPos.x + textOffset + autoCompleteLocation.column * glyphSize.x,
		cursorScreenPos.y + (autoCompleteLocation.line + 1) * glyphSize.y));

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	if (ImGui::BeginPopup("AutoCompleteContextMenu", flags)) {
		// deactivate popup (if requested)
		if (deactivateAutoComplete) {
			ImGui::CloseCurrentPopup();
			deactivateAutoComplete = false;
			autoCompleteActive = false;

		} else {
			// do we have any suggestions
			if (autoCompleteState.suggestions.size()) {
				auto items = std::min(autoCompleteState.suggestions.size(), static_cast<size_t>(10));

				// apply arrow keys to selected suggestion
				if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && autoCompleteSelection > 0) {
					autoCompleteSelection--;

				} else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && autoCompleteSelection < items - 1) {
					autoCompleteSelection++;

				// use selected suggestion if user hit tab of return
				} else if (ImGui::IsKeyPressed(ImGuiKey_Tab) || ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) {
					applyAutoComplete();
				}

				// render top suggestions
				for (size_t i = 0; i < items; i++) {
					// ensure unique ID
					ImGui::PushID(static_cast<int>(i));

					if (renderSuggestion(autoCompleteState.suggestions[i].c_str(), autoCompleteState.searchTerm, i == autoCompleteSelection)) {
						// user clicked on a suggestion, use it
						autoCompleteSelection = i;
						applyAutoComplete();
					}

					ImGui::PopID();
				}

			} else {
				ImGui::TextUnformatted("No suggestions");
			}
		}

		ImGui::EndPopup();

	} else {
		deactivateAutoComplete = false;
		autoCompleteActive = false;
	}
}


//
//	TextEditor::setAutoCompleteConfig
//

void TextEditor::setAutoCompleteConfig(const AutoCompleteConfig* config) {
	if (config) {
		autoCompleteConfig = *config;
		autoCompleteConfigured = true;

	} else {
		autoCompleteConfigured = false;
	}

	autoCompleteActive = false;
}


//
//	TextEditor::startAutoCompleteOnTyping
//

void TextEditor::startAutoCompleteOnTyping() {
	if (!autoCompleteActive && autoCompleteConfigured && autoCompleteConfig.triggersOnTyping) {
		// request start of autocomplete mode (can't be done here as the Dear ImGui context might not be right)
		activateAutoComplete = true;

		// additional cursors create a mess so clear them
		cursors.clearAdditional();
	}
}


//
//	TextEditor::startAutoCompleteOnShortcut
//

void TextEditor::startAutoCompleteOnShortcut() {
	if (!autoCompleteActive && autoCompleteConfigured && autoCompleteConfig.triggersOnShortcut) {
		// request start of autocomplete mode (can't be done here as the Dear ImGui context might not be right)
		activateAutoComplete = true;

		// additional cursors create a mess so clear them
		cursors.clearAdditional();

		// ensure cursor is visible so suggestions can be seen
		makeCursorVisible();
	}
}


//
//	TextEditor::cancelAutoComplete
//

void TextEditor::cancelAutoComplete() {
	if (autoCompleteActive) {
		deactivateAutoComplete = true;
	}
}


//
//	TextEditor::applyAutoComplete
//

void TextEditor::applyAutoComplete() {
	// remove word and replace it with chosen suggestion
	auto transaction = startTransaction();
	deleteText(transaction, autoCompleteStart, document.findWordEnd(autoCompleteStart, true));
	auto newEnd = insertText(transaction, autoCompleteStart, autoCompleteState.suggestions[autoCompleteSelection]);
	cursors.getMain().update(newEnd, newEnd);
	endTransaction(transaction);
}


//
//	TextEditor::updateAutoCompleteState
//

void TextEditor::updateAutoCompleteState() {
	autoCompleteState.searchTerm = document.getSectionText(autoCompleteStart, autoCompleteLocation);

	if (autoCompleteLocation.column == 0) {
		auto state = document[autoCompleteLocation.line].state;
		autoCompleteState.inComment = state == State::inComment;

		autoCompleteState.inString =
			state == State::inDoubleQuotedString ||
			state == State::inSingleQuotedString||
			state == State::inOtherString ||
			state == State::inOtherStringAlt;

	} else {
		auto color = document[autoCompleteLocation.line][autoCompleteLocation.column - 1].color;
		autoCompleteState.inComment = color == Color::comment;
		autoCompleteState.inString = color == Color::string;
	}

	autoCompleteState.language = language;
	autoCompleteState.userData = autoCompleteConfig.userData;
}


//
//	TextEditor::refreshAutoCompleteSuggestions
//

void TextEditor::refreshAutoCompleteSuggestions() {
	if (autoCompleteConfig.callback) {
		autoCompleteConfig.callback(autoCompleteState);

	} else {
		autoCompleteState.suggestions.clear();
	}

	autoCompleteSelection = 0;
}
