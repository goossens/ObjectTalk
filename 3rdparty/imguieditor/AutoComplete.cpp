//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui_internal.h"

#include "TextEditor.h"



//
//	TextEditor::setAutoCompleteConfig
//


void TextEditor::Autocomplete::setConfig(const AutoCompleteConfig* config) {
	if (config) {
		configuration = *config;
		configured = true;

	} else {
		configured = false;
	}

	active = false;
}


//
//	TextEditor::Autocomplete::startTyping
//

bool TextEditor::Autocomplete::startTyping(Cursors& cursors) {
	if (!active && !requestActivation && configured && configuration.triggerOnTyping) {
		start(cursors);
		return true;

	} else {
		return false;
	}
}


//
//	TextEditor::Autocomplete::startShortcut
//

bool TextEditor::Autocomplete::startShortcut(Cursors& cursors) {
	if (!active && !requestActivation && configured && configuration.triggerOnShortcut) {
		start(cursors);
		return true;

	} else {
		return false;
	}
}


//
//	TextEditor::Autocomplete::cancel
//

void TextEditor::Autocomplete::cancel() {
	if (active) {
		requestDeactivation = true;
	}
}


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
//	TextEditor::Autocomplete::render
//

bool TextEditor::Autocomplete::render(Document& document, Cursors& cursors, const Language* language, float textOffset, ImVec2 glyphSize) {
	// see if we need to activate autocomplete mode
	if (requestActivation) {
		// apply popup delay
		if (std::chrono::system_clock::now() > activationTime) {
			// reset activation flag
			requestActivation = false;

			// capture locations
			startLocation = document.findWordStart(currentLocation, true);

			// update the autocomplete state
			updateState(document, language);

			// handle cases where autocomplete request is ignored
			if(state.inComment && !configuration.triggerInComments) {
				return false;
			}

			if(state.inString && !configuration.triggerInStrings) {
				return false;
			}

			// get initial list of suggestions from the app
			refreshSuggestions();

			// show autocomplete popup window
			ImGui::OpenPopup("AutoCompleteContextMenu");
			active = true;
		}
	}

	// only continue if autocomplete is active
	if (!active) {
		return false;
	}

	// see if cursor moved since last time
	auto newLocation = cursors.getMain().getSelectionEnd();

	if (newLocation != currentLocation) {
		// see if we need to deactivate autocomplete because cursor is on new line
		if (newLocation.line != currentLocation.line) {
			requestDeactivation = true;

		} else {
			// see if cursor moved away from current word
			auto newStart = document.findWordStart(newLocation, true);

			if (newStart == startLocation) {
				currentLocation = newLocation;

				// we deactivate autocomplete if the current location is the start
				if (currentLocation == startLocation) {
					requestDeactivation = true;

				} else {
					updateState(document, language);
					refreshSuggestions();
				}

			} else {
				requestDeactivation = true;
			}
		}
	}

	// close autocomplete when user hits escape key
	if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
		requestDeactivation = true;
	}

	// open popup window
	bool result = false;
	auto cursorScreenPos = ImGui::GetCursorScreenPos();

	ImGui::SetNextWindowPos(ImVec2(
		cursorScreenPos.x + textOffset + currentLocation.column * glyphSize.x,
		cursorScreenPos.y + (currentLocation.line + 1) * glyphSize.y));

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	if (ImGui::BeginPopup("AutoCompleteContextMenu", flags)) {
		if (ImGui::IsWindowAppearing()) {
	    	ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
		}

		// deactivate popup (if requested)
		if (requestDeactivation) {
			ImGui::CloseCurrentPopup();
			requestDeactivation = false;
			active = false;

		} else {
			// do we have any suggestions
			if (state.suggestions.size()) {
				auto items = std::min(state.suggestions.size(), static_cast<size_t>(10));

				// apply arrow keys to selected suggestion
				if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) && currentSelection > 0) {
					currentSelection--;

				} else if (ImGui::IsKeyPressed(ImGuiKey_DownArrow) && currentSelection < items - 1) {
					currentSelection++;

				// use selected suggestion if user hit tab of return
				} else if (ImGui::IsKeyPressed(ImGuiKey_Tab) || ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)) {
					requestDeactivation = true;
					result = true;

				} else if (configuration.autoInsertSingleSuggestions && state.suggestions.size() == 1) {
					requestDeactivation = true;
					result = true;
				}

				// render top suggestions
				for (size_t i = 0; i < items; i++) {
					// ensure unique ID
					ImGui::PushID(static_cast<int>(i));

					if (renderSuggestion(state.suggestions[i].c_str(), state.searchTerm, i == currentSelection)) {
						// user clicked on a suggestion, use it
						currentSelection = i;
						requestDeactivation = true;
						result = true;
					}

					ImGui::PopID();
				}

			} else {
				ImGui::TextUnformatted("No suggestions");
			}
		}

		ImGui::EndPopup();

	} else {
		requestDeactivation = false;
		active = false;
	}

	return result;
}


//
//	TextEditor::Autocomplete::setSuggestions
//

void TextEditor::Autocomplete::setSuggestions(const std::vector<std::string>& suggestions) {
	state.suggestions = suggestions;
	currentSelection = 0;
}


//
//	TextEditor::Autocomplete::start
//

void TextEditor::Autocomplete::start(Cursors& cursors) {
	// request start of autocomplete mode (can't be done here as the Dear ImGui context might not be right)
	requestActivation = true;
	currentLocation = cursors.getMain().getSelectionEnd();
	activationTime = std::chrono::system_clock::now() + configuration.triggerDelay;
}


//
//	TextEditor::Autocomplete::updateState
//

void TextEditor::Autocomplete::updateState(Document& document, const Language* language) {
	state.searchTerm = document.getSectionText(startLocation, currentLocation);

	if (currentLocation.column == 0) {
		state.inIdentifier = false;
		state.inNumber = false;

		auto lineState = document[currentLocation.line].state;
		state.inComment = lineState == State::inComment;

		state.inString =
			lineState == State::inDoubleQuotedString ||
			lineState == State::inSingleQuotedString||
			lineState == State::inOtherString ||
			lineState == State::inOtherStringAlt;

	} else {
		auto color = document.getColor(Coordinate(currentLocation.line, currentLocation.column - 1));
		state.inIdentifier = color == Color::identifier || color == Color::knownIdentifier;
		state.inNumber = color == Color::number;
		state.inComment = color == Color::comment;
		state.inString = color == Color::string;
	}

	state.line = currentLocation.line;
	state.searchTermStartColumn = startLocation.column;
	state.searchTermStartIndex = document.getIndex(startLocation);
	state.searchTermEndColumn = currentLocation.column;
	state.searchTermEndIndex= document.getIndex(currentLocation);

	state.language = language;
	state.userData = configuration.userData;
}


//
//	TextEditor::Autocomplete::refreshSuggestions
//

void TextEditor::Autocomplete::refreshSuggestions() {
	// populate suggestion list through callback (or clear it if there is none)
	if (configuration.callback) {
		configuration.callback(state);

	} else {
		state.suggestions.clear();
	}

	currentSelection = 0;
}
