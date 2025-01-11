//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cctype>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "TextEditor.h"


//
//	CodePointGet
//

static std::string::const_iterator CodePointGet(std::string::const_iterator i, ImWchar* codepoint) {
	// parse a UTF-8 sequence into a unicode codepoint
	if ((*i & 0x80) == 0) {
		*codepoint = *i;
		i++;

	} else if ((*i & 0xE0) == 0xC0) {
		*codepoint = ((*i & 0x1f) << 6) | (*(i + 1) & 0x3f);
		i += 2;

	} else if ((*i & 0xF0) == 0xE0) {
		*codepoint = ((*i & 0x0f) << 12) | ((*(i + 1) & 0x3f) << 6) | (*(i + 2) & 0x3f);
		i += 3;

	} else if ((*i & 0xF8) == 0xF0) {
#if defined(IMGUI_USE_WCHAR32)
		*codepoint = ((*i & 0x07) << 18) | ((*(i + 1) & 0x3f) << 12) | ((*(i + 2) & 0x3f) << 6) | (*(i + 3) & 0x3f);
#else
		*codepoint = IM_UNICODE_CODEPOINT_INVALID;
#endif
		i += 4;

	} else {
		*codepoint = IM_UNICODE_CODEPOINT_INVALID;
		i++;
	}

	return i;
}


//
//	CodePointPut
//

static std::string::iterator CodePointPut(std::string::iterator i, ImWchar codepoint) {
	// generate UTF-8 sequence from a unicode codepoint
	if (codepoint < 0x80) {
		*i++ = codepoint;

	} else if (codepoint < 0x800) {
		*i++ = 0xc0 | ((codepoint >> 6) & 0x1f);
		*i++ = 0x80 | (codepoint & 0x3f);

#if defined(IMGUI_USE_WCHAR32)
	} else if (codepoint < 0x10000) {
		*i++ = 0xe0 | ((codepoint >> 12) & 0x0f);
		*i++ = 0x80 | ((codepoint >> 6) & 0x3f);
		*i++ = 0x80 | (codepoint & 0x3f);

	} else if (codepoint >= 0x110000) {
		codepoint = IM_UNICODE_CODEPOINT_INVALID;
		*i++ = 0xe0 | ((codepoint >> 12) & 0x0f);
		*i++ = 0x80 | ((codepoint >> 6) & 0x3f);
		*i++ = 0x80 | (codepoint & 0x3f);

	} else {
		*i++ = 0xf0 | ((codepoint >> 18) & 0x07);
		*i++ = 0x80 | ((codepoint >> 12) & 0x3f);
		*i++ = 0x80 | ((codepoint >> 6) & 0x3f);
		*i++ = 0x80 | (codepoint & 0x3f);

#else
	} else {
		*i++ = 0xe0 | ((codepoint >> 12) & 0x0f);
		*i++ = 0x80 | ((codepoint >> 6) & 0x3f);
		*i++ = 0x80 | (codepoint & 0x3f);
	}
#endif

	return i;
}


//
//	CodePointIsSpace
//

static inline bool CodePointIsSpace(ImWchar codepoint) {
	// see if codepoint represents a whitespace
	return codepoint < 256 ? std::isspace(static_cast<unsigned char>(codepoint)) : false;
}


//
//	CodePointToLower
//

static inline ImWchar CodePointToLower(ImWchar codepoint) {
	// convert a codepoint to lowercase
	return codepoint < 256 ? std::tolower(static_cast<unsigned char>(codepoint)) : codepoint;
}


//
//	TextEditor::setText
//

void TextEditor::setText(const std::string &text) {
	// load text into document and reset transactions and cursors
	document.setText(text);
	transactions.clear();
	cursors.clearAll();
}


//
//	TextEditor::render
//

void TextEditor::render(const char* title, const ImVec2& size, bool border) {
	// update color palette (if required)
	if (paletteAlpha != ImGui::GetStyle().Alpha) {
		updatePalette();
	}

	// get font information and determine start of text
	font = ImGui::GetFont();
	fontSize = ImGui::GetFontSize();
	glyphSize = ImVec2(font->CalcTextSizeA(fontSize, FLT_MAX, -1.0f, "#").x, ImGui::GetTextLineHeightWithSpacing() * lineSpacing);
	textStart = leftMargin * glyphSize.x;

	// adjust text start in case line numbers are shown
	if (showLineNumbers) {
		int digits = static_cast<int>(std::log10(document.lines() + 1) + 1.0f);
		textStart += glyphSize.x * (digits + lineNumberMargin);
	}

	// set style
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(palette.get(Color::background)));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	// start a new child window
	// this must be done before we we handle keyboard and mouse interactions to ensure correct ImGui context
	int longestLine = document.maxColumn();
	ImGui::SetNextWindowContentSize({textStart + longestLine * glyphSize.x + cursorWidth, document.lines() * glyphSize.y});
	ImGui::BeginChild(title, size, border, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNavInputs);

	// handle keyboard and mouse inputs
	handleKeyboardInputs();
	handleMouseInteractions();

	// ensure cursors are up to date (sorted and merged if required)
	if (cursors.anyHasUpdate()) {
		cursors.update();
	}

	// recolorize document if showMatchingBrackets option has changed
	if (lastShowMatchingBrackets != showMatchingBrackets) {
		document.colorize();
	}

	// rebuild bracket list (if document has changes or showMatchingBrackets option has changed)
	if (showMatchingBrackets && (!lastShowMatchingBrackets || document.isUpdated())) {
		brackets.update(document);
	}

	lastShowMatchingBrackets = showMatchingBrackets;

	// ensure cursor is visible (if requested)
	if (ensureCursorIsVisible) {
		auto cursor = cursors.getCurrent().getInteractiveEnd();

		if (cursor.line <= firstVisibleLine) {
			float targetScroll = std::max(0.0f, (cursor.line - 0.5f) * glyphSize.y);

			if (targetScroll < ImGui::GetScrollY()) {
				ImGui::SetScrollY(targetScroll);
			}

		} else if (cursor.line >= lastVisibleLine) {
			float targetScroll = std::max(0.0f, (cursor.line + 1.5f) * glyphSize.y - visibleHeight);

			if (targetScroll > ImGui::GetScrollY()) {
				ImGui::SetScrollY(targetScroll);
			}
		}

		if (cursor.column <= firstVisibleColumn) {
			float targetScroll = std::max(0.0f, (cursor.column - 0.5f) * glyphSize.x);

			if (targetScroll < ImGui::GetScrollX()) {
				ImGui::SetScrollX(targetScroll);
			}

		} else if (cursor.column >= lastVisibleColumn) {
			float targetScroll = std::max(0.0f, (cursor.column + 0.5f) * glyphSize.x - visibleWidth);

			if (targetScroll > ImGui::GetScrollX()) {
				ImGui::SetScrollX(targetScroll);
			}
		}

		ensureCursorIsVisible = false;
	}

	// determine view parameters
	float scrollbarSize = ImGui::GetStyle().ScrollbarSize;
	visibleHeight = ImGui::GetWindowHeight() - ((longestLine >= visibleColumns) ? scrollbarSize : 0.0f);
	visibleLines = std::max(static_cast<int>(std::ceil(visibleHeight / glyphSize.y)), 0);
	firstVisibleLine = std::max(static_cast<int>(std::floor(ImGui::GetScrollY() / glyphSize.y)), 0);
	lastVisibleLine = static_cast<int>(std::ceil((ImGui::GetScrollY() + visibleHeight) / glyphSize.y));

	auto tabSize = document.getTabSize();
	visibleWidth = ImGui::GetWindowWidth() - textStart - ((document.lines() >= visibleLines) ? scrollbarSize : 0.0f);
	visibleColumns = std::max(static_cast<int>(std::ceil(visibleWidth / glyphSize.x)), 0);
	firstVisibleColumn = (std::max(static_cast<int>(std::floor(ImGui::GetScrollX() / glyphSize.x)), 0) / tabSize) * tabSize;
	lastVisibleColumn = ((static_cast<int>(std::ceil((ImGui::GetScrollX() + visibleWidth) / glyphSize.x)) + 1) / tabSize) * tabSize;

	// render editor parts
	renderSelections();
	renderMatchingBrackets();
	renderText();
	renderCursors();
	renderLineNumbers();

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}


//
//	TextEditor::renderSelections
//

void TextEditor::renderSelections() {
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

	// draw background for selections
	for (auto& cursor : cursors) {
		if (cursor.hasSelection()) {
			auto start = cursor.getSelectionStart();
			auto end = cursor.getSelectionEnd();

			if (end.line >= firstVisibleLine && start.line <= lastVisibleLine) {
				auto first = std::max(start.line, firstVisibleLine);
				auto last = std::min(end.line, lastVisibleLine);

				for (auto line = first; line <= last; line++) {
					auto x = cursorScreenPos.x + textStart;
					auto left = x + (line == first ? start.column : 0) * glyphSize.x;
					auto right = x + (line == last ? end.column : document.maxColumn(line)) * glyphSize.x;
					auto y = cursorScreenPos.y + line * glyphSize.y;
					drawList->AddRectFilled(ImVec2(left, y), ImVec2(right, y + glyphSize.y), palette.get(Color::selection));
				}
			}
		}
	}
}


//
//	TextEditor::renderMatchingBrackets
//

void TextEditor::renderMatchingBrackets() {
	if (showMatchingBrackets) {
		if (brackets.size()) {
			auto drawList = ImGui::GetWindowDrawList();
			ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

			// render bracket pair lines
			for (auto& bracket : brackets) {
				if ((bracket.end.line - bracket.start.line) > 1 &&
					bracket.start.line <= lastVisibleLine &&
					bracket.end.line > firstVisibleLine) {

					auto lineX = cursorScreenPos.x + textStart + std::min(bracket.start.column, bracket.end.column) * glyphSize.x;
					auto startY = cursorScreenPos.y + (bracket.start.line + 1) * glyphSize.y;
					auto endY = cursorScreenPos.y + bracket.end.line * glyphSize.y;
					drawList->AddLine(ImVec2(lineX, startY), ImVec2(lineX, endY), palette.get(Color::whitespace), 1.0f);
				}
			}

			// render active bracket pair
			auto active = brackets.getActive(cursors.getMain().getInteractiveEnd());

			if (active != brackets.end() &&
				active->start.line <= lastVisibleLine &&
				active->end.line > firstVisibleLine) {

				auto x1 = cursorScreenPos.x + textStart + active->start.column * glyphSize.x;
				auto y1 = cursorScreenPos.y + active->start.line * glyphSize.y;
				drawList->AddRectFilled(ImVec2(x1, y1), ImVec2(x1 + glyphSize.x, y1 + glyphSize.y), palette.get(Color::whitespace));

				auto x2 = cursorScreenPos.x + textStart + active->end.column * glyphSize.x;
				auto y2 = cursorScreenPos.y + active->end.line * glyphSize.y;
				drawList->AddRectFilled(ImVec2(x2, y2), ImVec2(x2 + glyphSize.x, y2 + glyphSize.y), palette.get(Color::whitespace));

				if (active->end.line - active->start.line > 1) {
					auto lineX = std::min(x1, x2);
					drawList->AddLine(ImVec2(lineX, y1 + glyphSize.y), ImVec2(lineX, y2), palette.get(Color::cursor), 1.0f);
				}
			}
		}
	}
}


//
//	TextEditor::renderText
//

void TextEditor::renderText() {
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	ImVec2 lineScreenPos = cursorScreenPos + ImVec2(textStart, firstVisibleLine * glyphSize.y);
	auto tabSize = document.getTabSize();

	for (int i = firstVisibleLine; i <= lastVisibleLine && i < document.lines(); i++) {
		auto& line = document[i];

		// draw colored glyphs for current line
		auto column = firstVisibleColumn;
		auto index = document.getIndex(line, column);
		auto glyphs = line.glyphs();

		while (index < glyphs && column <= lastVisibleColumn) {
			auto& glyph = line[index];
			auto character = glyph.character;
			ImVec2 glyphPos{lineScreenPos.x + column * glyphSize.x, lineScreenPos.y};

			if (character == '\t') {
				if (showWhitespaces) {
					const auto x1 = glyphPos.x + glyphSize.x * 0.3f;
					const auto y = glyphPos.y + fontSize * 0.5f;
					const auto x2 = glyphPos.x + glyphSize.x;

					ImVec2 p1, p2, p3, p4;
					p1 = ImVec2(x1, y);
					p2 = ImVec2(x2, y);
					p3 = ImVec2(x2 - fontSize * 0.16f, y - fontSize * 0.16f);
					p4 = ImVec2(x2 - fontSize * 0.16f, y + fontSize * 0.16f);

					drawList->AddLine(p1, p2, palette.get(Color::whitespace));
					drawList->AddLine(p2, p3, palette.get(Color::whitespace));
					drawList->AddLine(p2, p4, palette.get(Color::whitespace));
				}

			} else if (character == ' ') {
				if (showWhitespaces) {
					const auto x = glyphPos.x + glyphSize.x * 0.5f;
					const auto y = glyphPos.y + fontSize * 0.5f;
					drawList->AddCircleFilled(ImVec2(x, y), 1.5f, palette.get(Color::whitespace), 4);
				}

			} else {
				font->RenderChar(drawList, fontSize, glyphPos, palette.get(glyph.color), character);
			}

			index++;
			column += (character == '\t') ? tabSize - (column % tabSize) : 1;
		}

		lineScreenPos.y += glyphSize.y;
	}
}


//
//	TextEditor::renderCursors
//

void TextEditor::renderCursors() {
	// update cursor animation timer
	cursorAnimationTimer = std::fmod(cursorAnimationTimer + ImGui::GetIO().DeltaTime, 1.0f);

	if (ImGui::IsWindowFocused() && (!ImGui::GetIO().ConfigInputTextCursorBlink || cursorAnimationTimer < 0.5f)) {
		auto drawList = ImGui::GetWindowDrawList();
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();

		for (auto& cursor : cursors) {
			auto pos = cursor.getInteractiveEnd();

			if (pos.line >= firstVisibleLine && pos.line <= lastVisibleLine) {
				auto x = cursorScreenPos.x + textStart + pos.column * glyphSize.x;
				auto y = cursorScreenPos.y + pos.line * glyphSize.y;
				drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + cursorWidth, y + glyphSize.y), palette.get(Color::cursor));
			}
		}
	}
}


//
//	TextEditor::renderLineNumbers
//

void TextEditor::renderLineNumbers() {
	if (showLineNumbers) {
		auto drawList = ImGui::GetWindowDrawList();
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
		auto lineNumberEnd = ImGui::GetWindowPos().x + textStart;

		// erase background in case we are scrolling horizontally
		if (ImGui::GetScrollX() > 0.0f) {
			drawList->AddRectFilled(
				ImGui::GetWindowPos(),
				ImGui::GetWindowPos() + ImVec2(textStart,
				ImGui::GetWindowSize().y),
				palette.get(Color::background));
		}

		// get the line number of the last cursor
		auto curserLine = cursors.getCurrent().getInteractiveEnd().line;

		for (int i = firstVisibleLine; i <= lastVisibleLine && i < document.lines(); i++) {
			int digits = static_cast<int>(std::log10(i + 1) + 1.0f);
			auto lineNumberWidth = (digits + lineNumberMargin) * glyphSize.x;
			Color foreground =  i == curserLine ? Color::currentLineNumber : Color::lineNumber;

			drawList->AddText(
				ImVec2(lineNumberEnd - lineNumberWidth,
				cursorScreenPos.y + i * glyphSize.y),
				palette.get(foreground),
				std::to_string(i + 1).c_str());
		}
	}
}


//
//	TextEditor::handleKeyboardInputs
//

void TextEditor::handleKeyboardInputs() {
	if (ImGui::IsWindowFocused()) {
		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

		// get state of modifier keys
		auto shift = ImGui::IsKeyDown(ImGuiMod_Shift);
		auto ctrl = ImGui::IsKeyDown(ImGuiMod_Ctrl);
		auto alt = ImGui::IsKeyDown(ImGuiMod_Alt);

		auto isNoModifiers = !ctrl && !shift && !alt;
		auto isShortcut = ctrl && !shift && !alt;
		auto isShiftShortcut = ctrl && shift && !alt;
		auto isOptionalShiftShortcut = ctrl && !alt;
		auto isAltOnly = !ctrl && !shift && alt;
		auto isShiftOnly = !ctrl && shift && !alt;
		auto isOptionalShift = !ctrl && !alt;
		auto isOptionalAlt = !ctrl && !shift;
		auto isOptionalAltShift = !ctrl;

		// cursor movements and selections
		if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUp(1, shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDown(1, shift); }
		else if (isOptionalAltShift && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { moveLeft(shift, alt); }
		else if (isOptionalAltShift && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { moveRight(shift, alt); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_PageUp)) { moveUp(visibleLines - 2, shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_PageDown)) { moveDown(visibleLines - 2, shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveToTop(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Home)) { moveToTop(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveToBottom(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_End)) { moveToBottom(shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_Home)) { moveToStartOfLine(shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_End)) { moveToEndOfLine(shift); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_A)) { selectAll(); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_D) && cursors.currentCursorHasSelection()) { addNextOccurrence(); }

		// clipboard operations
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_X)) { cut(); }
		else if (isShiftOnly && ImGui::IsKeyPressed(ImGuiKey_Delete)) { cut(); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_Insert)) { copy(); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_C)) { copy() ;}

		else if (!readOnly && isShiftOnly && ImGui::IsKeyPressed(ImGuiKey_Insert)) { paste(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_V)) { paste(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Z)) { undo(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Y)) { redo(); }
		else if (!readOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Z)) { redo(); }

		// remove text
		else if (!readOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Delete)) { handleDelete(alt); }
		else if (!readOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Backspace)) { handleBackspace(alt); }
		else if (!readOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_K)) { removeSelectedLines(); }

		// text manipulation
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) { deindentLines(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_RightBracket)) { indentLines(); }
		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUpLines(); }
		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDownLines(); }
		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Slash) && document.hasLanguage()) { toggleComments(); }

		// change insert mode
		else if (isNoModifiers && ImGui::IsKeyPressed(ImGuiKey_Insert)) { overwrite = !overwrite; }

		// handle new line
		else if (!readOnly && isNoModifiers && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { handleCharacter('\n'); }
		else if (!readOnly && isShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineBelow(); }
		else if (!readOnly && isShiftShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineAbove(); }

		// handle tabs
		else if (!readOnly && isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_Tab)) {
			if (cursors.anyHasSelection()) {
				if (shift) {
					deindentLines();

				} else {
					indentLines();
				}

			} else {
				handleCharacter('\t');
			}
		}

		// handle regular text
		if (!readOnly && !io.InputQueueCharacters.empty()) {
			for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
				auto character = io.InputQueueCharacters[i];

				if (character == '\n' || character >= 32)
					handleCharacter(character);
			}

			io.InputQueueCharacters.resize(0);
		}
	}
}


//
//	TextEditor::handleMouseInteractions
//

void TextEditor::handleMouseInteractions() {
	// pan with dragging middle mouse button
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
		ImGui::SetScrollX(ImGui::GetScrollX() - mouseDelta.x);
		ImGui::SetScrollY(ImGui::GetScrollY() - mouseDelta.y);
		ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);

	// ignore other interactions when the editor is not hovered
	} else if (ImGui::IsWindowHovered()) {
		auto io = ImGui::GetIO();
		ImVec2 mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
		bool overLineNumbers = showLineNumbers && (mousePos.x - ImGui::GetScrollX() < textStart);

		auto mouseCoord = document.normalizeCoordinate(Coordinate(
			std::max(0, static_cast<int>(floor(mousePos.y / glyphSize.y))),
			std::max(0, static_cast<int>(floor((mousePos.x - textStart) / glyphSize.x)))
		));

		// show text cursor if required
		if (ImGui::IsWindowFocused() && !overLineNumbers) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			// update selection with dragging left mouse button
			io.WantCaptureMouse = true;

			if (overLineNumbers) {
				auto& cursor = cursors.getCurrent();
				auto start = Coordinate(mouseCoord.line, 0);
				auto end = document.getDown(start);
				cursor.update(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

			} else {
				cursors.updateCurrentCursor(mouseCoord);
			}

			ensureCursorIsVisible = true;

		} else {
			// handle left mouse button actions
			auto click = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			auto doubleClick = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
			auto now = static_cast<float>(ImGui::GetTime());
			auto tripleClick = click && !doubleClick && (lastClickTime != -1.0f && (now - lastClickTime) < io.MouseDoubleClickTime);

			if (click || doubleClick || tripleClick) {
				lastClickTime = tripleClick ? -1.0f : now;
			}

			if (tripleClick) {
				// left mouse button triple click
				if (!overLineNumbers) {
					auto start = document.getStartOfLine(mouseCoord);
					auto end = document.getDown(start);
					cursors.updateCurrentCursor(start, end);
				}

			} else if (doubleClick) {
				// left mouse button double click
				if (!overLineNumbers) {
					auto start = document.findWordStart(mouseCoord);
					auto end = document.findWordEnd(mouseCoord);
					cursors.updateCurrentCursor(start, end);
				}

			} else if (click) {
				// left mouse button single click
				auto extendCursor = ImGui::IsKeyDown(ImGuiMod_Shift);

#if __APPLE__
				auto addCursor = ImGui::IsKeyDown(ImGuiMod_Alt);
#else
				auto addCursor = ImGui::IsKeyDown(ImGuiMod_Ctrl);
#endif

				// handle mouse clicks over line numbers
				if (overLineNumbers) {
					auto start = Coordinate(mouseCoord.line, 0);
					auto end = document.getDown(start);

					if (extendCursor) {
						auto& cursor = cursors.getCurrent();
						cursor.update(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

					} else if (addCursor) {
						cursors.addCursor(start, end);

					} else {
						cursors.setCursor(start, end);
					}

				// handle mouse clicks over text
				} else {
					if (extendCursor) {
						cursors.updateCurrentCursor(mouseCoord);

					} else if (addCursor) {
						cursors.addCursor(mouseCoord);

					} else {
						cursors.setCursor(mouseCoord);
					}
				}
			}
		}
	}
}


//
//	TextEditor::selectAll
//

void TextEditor::selectAll() {
	moveToTop(false);
	moveToBottom(true);
}


//
//	TextEditor::selectLine
//

void TextEditor::selectLine(int line) {
	Coordinate start{line, 0};
	moveTo(start, false);
	moveTo(document.getDown(start), true);
}


//
//	TextEditor::selectLines
//

void TextEditor::selectLines(int startLine, int endLine) {
	Coordinate start{startLine, 0};
	moveTo(start, false);
	moveTo(document.getDown(start, endLine - startLine + 1), true);
}


//
//	TextEditor::cut
//

void TextEditor::cut() {
	// copy selections to clipboard and remove them
	copy();
	auto transaction = startTransaction();
	deleteTextFromAllCursors(transaction);
	cursors.getCurrent().resetToStart();
	endTransaction(transaction);
}


//
//	TextEditor::copy
//

void TextEditor::copy() const {
	// copy all selections and put them on the clipboard
	// empty cursors copy the entire line
	std::string text;

	if (cursors.anyHasSelection()) {
		for (auto& cursor : cursors) {
			if (text.size()) {
				text += "\n";
			}

			if (cursor.hasSelection()) {
				text += document.getSectionText(cursor.getSelectionStart(), cursor.getSelectionEnd());

			} else {
				text += document.getLineText(cursor.getSelectionStart().line);
			}
		}

	} else {
		for (auto& cursor : cursors) {
			text += document.getLineText(cursor.getSelectionStart().line) + "\n";
		}
	}

	ImGui::SetClipboardText(text.c_str());
}


//
//	TextEditor::paste
//

void TextEditor::paste() {
	// ignore non-text clipboard content
	if (ImGui::GetClipboardText() != nullptr) {
		auto transaction = startTransaction();
		std::string clipboardText = ImGui::GetClipboardText();
		insertTextIntoAllCursors(transaction, ImGui::GetClipboardText());
		endTransaction(transaction);
	}
}


//
//	TextEditor::undo
//

void TextEditor::undo() {
	if (transactions.canUndo()) {
		transactions.undo(document, cursors);
	}
}


//
//	TextEditor::redo
//

void TextEditor::redo() {
	if (transactions.canRedo()) {
		transactions.redo(document, cursors);
	}
}


//
//	TextEditor::selectFirstOccurrenceOf
//

void TextEditor::selectFirstOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(Coordinate(0, 0), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		ensureCursorIsVisible = true;

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::selectNextOccurrenceOf
//

void TextEditor::selectNextOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord) {
	Coordinate start, end;

	if (document.findText(cursors.getCurrent().getSelectionEnd(), text, caseSensitive, wholeWord, start, end)) {
		cursors.setCursor(start, end);
		ensureCursorIsVisible = true;

	} else {
		cursors.clearAdditional(true);
	}
}


//
//	TextEditor::selectAllOccurrencesOf
//

void TextEditor::selectAllOccurrencesOf(const std::string& text, bool caseSensitive, bool wholeWord) {
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

		ensureCursorIsVisible = true;

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

void TextEditor::replaceTextInCurrentCursor(const std::string& text) {
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

void TextEditor::replaceTextInAllCursors(const std::string& text) {
	auto transaction = startTransaction();
	insertTextIntoAllCursors(transaction, text);
	endTransaction(transaction);
}


//
//	TextEditor::moveUp
//

void TextEditor::moveUp(int lines, bool select) {
	for (auto& cursor : cursors) {
		cursor.update(document.getUp(cursor.getInteractiveEnd(), lines), select);
	}

	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveDown
//

void TextEditor::moveDown(int lines, bool select) {
	for (auto& cursor : cursors) {
		cursor.update(document.getDown(cursor.getInteractiveEnd(), lines), select);
	}

	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveLeft
//

void TextEditor::moveLeft(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		cursor.update(document.getLeft(cursor.getInteractiveEnd(), wordMode), select);
	}

	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveRight
//

void TextEditor::moveRight(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		cursor.update(document.getRight(cursor.getInteractiveEnd(), wordMode), select);
	}

	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToTop
//

void TextEditor::moveToTop(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getTop(), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToBottom
//

void TextEditor::moveToBottom(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getBottom(), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToStartOfLine
//

void TextEditor::moveToStartOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getStartOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveToEndOfLine
//

void TextEditor::moveToEndOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getEndOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::moveTo
//

void TextEditor::moveTo(Coordinate coordinate, bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(coordinate, select);
	ensureCursorIsVisible = true;
}


//
//	TextEditor::handleCharacter
//

void TextEditor::handleCharacter(ImWchar character) {
	auto transaction = startTransaction();

	auto opener = static_cast<char>(character);
	auto isPaired = !overwrite && completePairedGlyphs && (opener == '{' || opener == '[' || opener == '(' || opener == '"' || opener == '\'');
	auto closer = opener == '{' ? '}' : (opener == '[' ? ']' : (opener == '(' ? ')' : opener));

	// ignore input if it was the closing character for a pair that was automatically inserted
	if (completePairCloser) {
		if (completePairCloser == character && completePairLocation == cursors.getCurrent().getSelectionEnd()) {
			completePairCloser = 0;
			return;
		}

		completePairCloser = 0;
	}

	if (cursors.anyHasSelection() && isPaired) {
		// encapsulate the current selections with the requested pairs
		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			if (cursor->hasSelection()) {
				auto start = cursor->getSelectionStart();
				auto end = cursor->getSelectionEnd();

				// insert the opening glyph
				auto end1 = insertText(transaction, end, std::string(1, closer));
				cursors.adjustForInsert(cursor, start, end1);

				// insert the closing glyph
				auto end2 = insertText(transaction, start, std::string(1, opener));
				cursors.adjustForInsert(cursor, start, end2);

				// update old selection
				cursor->update(Coordinate(start.line, start.column + 1), Coordinate(end.line, end.column + 1));
			}
		}

	} else if (isPaired) {
		// insert the requested pair
		std::string pair(2, 0);
		pair[0] = opener;
		pair[1] = closer;

		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			auto start = cursor->getSelectionStart();
			auto end = insertText(transaction, start, pair);
			cursors.adjustForInsert(cursor, start, end);
			cursor->update(Coordinate(start.line, start.column + 1), false);
		}

		// remember the closer
		completePairCloser = closer;
		completePairLocation = cursors.getCurrent().getSelectionEnd();

	} else if (!overwrite && autoIndent && character == '\n') {
		// handle auto indent case
		autoIndentAllCursors(transaction);

	} else {
		// handle overwrite by deleting next glyph before insert
		if (overwrite) {
			for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
				if (!cursor->hasSelection()) {
					auto start = cursor->getSelectionStart();

					if (start != document.getEndOfLine(start)) {
						auto end = document.getRight(start);
						deleteText(transaction, start, end);
						cursors.adjustForDelete(cursor, start, end);
					}
				}
			}
		}

		// just insert a regular character
		std::string utf8(4, 0);
		std::string text(utf8.begin(), CodePointPut(utf8.begin(), character));
		insertTextIntoAllCursors(transaction, text);
	}

	endTransaction(transaction);
}


//
//	TextEditor::handleBackspace
//

void TextEditor::handleBackspace(bool wordMode) {
	auto transaction = startTransaction();

	// remove selections or characters to the left of the cursor
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->hasSelection() ? cursor->getSelectionStart() : document.getLeft(cursor->getSelectionStart(), wordMode);
		auto end = cursor->getSelectionEnd();
		deleteText(transaction, start, end);
		cursor->update(start, false);
		cursors.adjustForDelete(cursor, start, end);
	}

	endTransaction(transaction);
}

//
//	TextEditor::handleDelete
//

void TextEditor::handleDelete(bool wordMode) {
	auto transaction = startTransaction();

	// remove selections or characters to the right of the cursor
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();
		auto end = cursor->hasSelection() ? cursor->getSelectionEnd() : document.getRight(cursor->getSelectionEnd(), wordMode);
		deleteText(transaction, start, end);
		cursor->update(start, false);
		cursors.adjustForDelete(cursor, start, end);
	}

	endTransaction(transaction);
}


//
//	TextEditor::removeSelectedLines
//

void TextEditor::removeSelectedLines() {
	auto transaction = startTransaction();

	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = document.getStartOfLine(cursor->getSelectionStart());
		auto end = cursor->getSelectionEnd();
		end = (end.column == 0) ? end : document.getNextLine(end);
		deleteText(transaction, start, end);
		cursor->update(start, false);
		cursors.adjustForDelete(cursor, start, end);
	}

	endTransaction(transaction);
}


//
//	TextEditor::insertLineAbove
//

void TextEditor::insertLineAbove() {
	auto transaction = startTransaction();

	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = document.getStartOfLine(cursor->getSelectionStart());
		auto end = insertText(transaction, start, "\n");
		cursor->update(start, false);
		cursors.adjustForInsert(cursor, start, end);
	}

	endTransaction(transaction);
}


//
//	TextEditor::insertLineBelow
//

void TextEditor::insertLineBelow() {
	auto transaction = startTransaction();

	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionEnd();
		start = (start.column == 0) ? start : document.getNextLine(start);
		auto end = insertText(transaction, start, "\n");
		cursor->update(start, false);
		cursors.adjustForInsert(cursor, start, end);
	}

	endTransaction(transaction);
}


//
//	TextEditor::indentLines
//

void TextEditor::indentLines() {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto cursorStart = cursor->getSelectionStart();
		auto cursorEnd = cursor->getInteractiveEnd();

		// process all lines in this cursor
		for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
			if (Coordinate(line, 0) != cursorEnd && document[line].glyphs()) {
				auto insertStart = Coordinate(line, 0);
				auto insertEnd = insertText(transaction, insertStart, "\t");
				cursors.adjustForInsert(cursor, insertStart, insertEnd);
			}
		}

		auto tabSize = document.getTabSize();
		cursorStart.column += cursorStart.column ? tabSize : 0;
		cursorEnd.column += cursorEnd.column ? tabSize : 0;
		cursor->update(cursorStart, cursorEnd);
	}

	endTransaction(transaction);
}


//
//	TextEditor::deindentLines
//

void TextEditor::deindentLines() {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto cursorStart = cursor->getSelectionStart();
		auto cursorEnd = cursor->getInteractiveEnd();
		auto tabSize = document.getTabSize();

		for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
			// determine how many whitespace are available at the start with a max of 4 columns
			int column = 0;
			int index = 0;

			while (column < 4 && index < document[line].glyphs() && std::isblank(document[line][index].character)) {
				column += document[line][index].character == '\t' ? tabSize - (column % tabSize) : 1;
				index++;
			}

			// delete that whitespace if required
			Coordinate deleteStart{line, 0};
			Coordinate deleteEnd{line, document.getColumn(line, index)};

			if (deleteEnd != deleteStart) {
				deleteText(transaction, deleteStart, deleteEnd);
				cursors.adjustForDelete(cursor, deleteStart, deleteEnd);
			}
		}
	}

	endTransaction(transaction);
}


//
//	Widget::moveUpLines
//

void TextEditor::moveUpLines() {
	// don't move up if first line is in one of the cursors
	if (cursors[0].getSelectionStart().line != 0) {
		auto transaction = startTransaction();

		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			auto start = cursor->getSelectionStart();
			auto end = cursor->getSelectionEnd();

			// delete existing lines
			auto deleteStart = document.getStartOfLine(start);
			auto deleteEnd = (end.column == 0) ? end : document.getNextLine(end);;
			auto text = document.getSectionText(deleteStart, deleteEnd);
			deleteText(transaction, deleteStart, deleteEnd);
			cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

			// insert text one line up
			auto insertStart = document.getUp(deleteStart);
			auto insertEnd = insertText(transaction, insertStart, text);
			cursors.adjustForInsert(cursor, insertStart, insertEnd);

			// update cursor
			cursor->update(start - Coordinate(1, 0), end - Coordinate(1, 0));
		}

		endTransaction(transaction);
	}
}


//
//	TextEditor::moveDownLines
//

void TextEditor::moveDownLines() {
	// don't move up if last line is in one of the cursors
	if (!document.isLastLine(cursors[cursors.size() - 1].getSelectionStart().line)) {
		auto transaction = startTransaction();

		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			auto start = cursor->getSelectionStart();
			auto end = cursor->getSelectionEnd();

			// delete existing lines
			auto deleteStart = document.getStartOfLine(start);
			auto deleteEnd = (end.column == 0) ? end : document.getNextLine(end);;
			auto text = document.getSectionText(deleteStart, deleteEnd);
			deleteText(transaction, deleteStart, deleteEnd);
			cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

			// insert text one line down
			auto insertStart = document.getDown(deleteStart);
			auto insertEnd = insertText(transaction, insertStart, text);
			cursors.adjustForInsert(cursor, insertStart, insertEnd);

			// update cursor
			cursor->update(start + Coordinate(1, 0), end + Coordinate(1, 0));
		}

		endTransaction(transaction);
	}
}


//
//	TextEditor::toggleComments
//

void TextEditor::toggleComments() {
	auto transaction = startTransaction();
	auto comment = document.getLanguage().singleLineComment;

	// process al cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto cursorStart = cursor->getSelectionStart();
		auto cursorEnd = cursor->getInteractiveEnd();

		// process all lines in this cursor
		for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
			if (Coordinate(line, 0) != cursorEnd && document[line].glyphs()) {
				// see if line starts with a comment (after possible leading whitespaces)
				int start = 0;
				int i = 0;

				while (start < document[line].glyphs() && CodePointIsSpace(document[line][start].character)) {
					start++;
				}

				while (start + i < document[line].glyphs() && i < comment.size() && document[line][start + i].character == comment[i]) {
					i++;
				}

				if (i == comment.size()) {
					auto deleteStart = Coordinate(line, document.getColumn(line, start));
					auto deleteEnd = Coordinate(line, document.getColumn(line, start + static_cast<int>(comment.size()) + 1));
					deleteText(transaction, deleteStart, deleteEnd);
					cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

				} else {
					auto insertStart = Coordinate(line, document.getColumn(line, start));
					auto insertEnd = insertText(transaction, insertStart, comment + " ");
					cursors.adjustForInsert(cursor, insertStart, insertEnd);
				}
			}
		}
	}

	endTransaction(transaction);
}


//
//	TextEditor::stripTrailingWhitespaces
//

void TextEditor::stripTrailingWhitespaces() {
	auto transaction = startTransaction();

	// process all the lines
	for (int i = 0; i < document.lines(); i++) {
		auto& line = document[i];
		int size = line.glyphs();
		int whitespace = -1;
		bool done = false;

		// look for first non-whitespace glyph at the end of the line
		for (auto index = size - 1; !done && index >= 0; index--) {
			if (CodePointIsSpace(line[index].character)) {
				whitespace = index;

			} else {
				done = true;
			}
		}

		// remove whitespaces (if required)
		if (whitespace >= 0) {
			auto start = Coordinate(i, document.getColumn(line, whitespace));
			auto end = Coordinate(i, document.getColumn(line, size));
			deleteText(transaction, start, end);
		}
	}

	// update cursor if transaction wasn't empty
	if (endTransaction(transaction)) {
		cursors.setCursor(document.normalizeCoordinate(cursors.getCurrent().getSelectionEnd()));
	}
}


//
//	TextEditor::filterLines
//

void TextEditor::filterLines(std::function<std::string(std::string)> filter) {
	auto transaction = startTransaction();

	// process all the lines
	for (int i = 0; i < document.lines(); i++) {
		// get original text and run it through filter
		auto before = document.getLineText(i);
		std::string after = filter(before);

		// update line if anything changed
		if (after != before) {
			auto start = Coordinate(i, 0);
			auto end = document.getEndOfLine(start);
			deleteText(transaction, start, end);
			insertText(transaction, start, after);
		}
	}

	// update cursor if transaction wasn't empty
	if (endTransaction(transaction)) {
		cursors.setCursor(document.normalizeCoordinate(cursors.getCurrent().getSelectionEnd()));
	}
}


//
//	TextEditor::tabsToSpaces
//

void TextEditor::tabsToSpaces() {
	filterLines([this](const std::string& input) {
		auto tabSize = document.getTabSize();
		std::string output;

		for (auto c : input) {
			if (c == '\t') {
				output.append(tabSize - (output.size() % tabSize), ' ');

			} else {
				output += c;
			}
		}

		return output;
	});
}


//
//	TextEditor::spacesToTabs
//

void TextEditor::spacesToTabs() {
	FilterLines([this](const std::string& input) {
		auto tabSize = document.getTabSize();
		std::string output;
		int pos = 0;
		int spaces = 0;

		for (auto c : input) {
			if (c == ' ') {
				spaces++;

			} else {
				while (spaces) {
					int spacesUntilNextTab = tabSize - (pos % tabSize);

					if (spacesUntilNextTab == 1) {
						output += ' ';
						pos++;
						spaces--;

					} else if (spaces >= spacesUntilNextTab) {
						output += '\t';
						pos += spacesUntilNextTab;
						spaces -= spacesUntilNextTab;

					} else if (c != '\t')
						while (spaces) {
							output += ' ';
							pos++;
							spaces--;
						}

					else {
						spaces = 0;
					}
				}

				if (c == '\t') {
					output += '\t';
					pos += tabSize - (pos % tabSize);

				} else {
					output += c;
					pos++;
				}
			}
		}

		return output;
	});
}


//
//	TextEditor::startTransaction
//

std::shared_ptr<TextEditor::Transaction> TextEditor::startTransaction() {
	std::shared_ptr<Transaction> transaction = Transactions::create();
	transaction->setBeforeState(cursors);
	return transaction;
}


//
//	TextEditor::endTransaction
//

bool TextEditor::endTransaction(std::shared_ptr<Transaction> transaction) {
	if (transaction->actions() > 0) {
		cursors.update();
		transaction->setAfterState(cursors);
		transactions.add(transaction);
		return true;

	} else {
		return false;
	}
}


//
//	TextEditor::insertTextIntoAllCursors
//

void TextEditor::insertTextIntoAllCursors(std::shared_ptr<Transaction> transaction, const std::string& text) {
	// delete any selection content first
	deleteTextFromAllCursors(transaction);

	// insert the text
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();
		auto end = insertText(transaction, start, text);
		cursor->update(end, false);
		cursors.adjustForInsert(cursor, start, end);
	}
}


//
//	TextEditor::deleteTextFromAllCursors
//

void TextEditor::deleteTextFromAllCursors(std::shared_ptr<Transaction> transaction) {
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		if (cursor->hasSelection()) {
			auto start = cursor->getSelectionStart();
			auto end = cursor->getSelectionEnd();
			deleteText(transaction, start, end);
			cursors.adjustForDelete(cursor, start, end);
		}
	}
}


//
//	TextEditor::autoIndentAllCursors
//

void TextEditor::autoIndentAllCursors(std::shared_ptr<Transaction> transaction) {
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();

		// delete any selections
		if (cursor->hasSelection()) {
			auto end = cursor->getSelectionEnd();
			deleteText(transaction, start, end);
			cursors.adjustForDelete(cursor, start, end);
		}

		// get previous and next character
		auto index = document.getIndex(start);
		auto& line = document[start.line];
		ImWchar previousChar = index > 0 ? line[index - 1].character : 0;
		ImWchar nextChar = index < line.size() ? line[index].character : 0;

		// remove extra whitespaces if required
		if (CodePointIsSpace(nextChar)) {
			while (index < line.size() && CodePointIsSpace(line[index].character)) {
				index++;
			}

			auto end = Coordinate(start.line, document.getColumn(start.line, index));
			deleteText(transaction, start, end);
			cursors.adjustForDelete(cursor, start, end);
		}

		// determine whitespace at start of current line
		std::string whitespace;

		for (auto i = 0; i < line.size() && CodePointIsSpace(line[i].character); i++) {
			whitespace += line[i].character;
		}

		// determine text to insert
		std::string insert = "\n" + whitespace;
		auto newCursorIndex = static_cast<int>(whitespace.size());

		// handle special cases
		if (previousChar == '[' || previousChar == '{') {
			// add to an existing block
			insert += "\t";
			newCursorIndex++;

			if ((previousChar == '[' && nextChar == ']') || (previousChar == '{' && nextChar == '}')) {
				// open a new block
				insert += "\n" + whitespace;
			}
		}

		// insert new text
		auto end = insertText(transaction, start, insert);
		cursors.adjustForInsert(cursor, start, end);

		// set new cursor location
		cursor->update(Coordinate(start.line + 1, document.getColumn(start.line + 1, newCursorIndex)), false);
	}
}


//
//	TextEditor::insertText
//

TextEditor::Coordinate TextEditor::insertText(std::shared_ptr<Transaction> transaction, Coordinate start, const std::string& text) {
	// update document, add transaction and return coordinate of end of insert
	// this function does not touch the cursors
	auto end = document.insertText(start, text);
	transaction->addInsert(start, end, text);
	return end;
}


//
//	TextEditor::deleteText
//

void TextEditor::deleteText(std::shared_ptr<Transaction> transaction, Coordinate start, Coordinate end) {
	// update document and add transaction
	// this function does not touch the cursors
	auto text = document.getSectionText(start, end);
	document.deleteText(start, end);
	transaction->addDelete(start, end, text);
}


//
//	TextEditor::Cursor::adjustCoordinateForInsert
//

TextEditor::Coordinate TextEditor::Cursor::adjustCoordinateForInsert(Coordinate coordinate, Coordinate start, Coordinate end) {
	coordinate.line += end.line - start.line;

	if (end.line == coordinate.line) {
		coordinate.column += end.column;
	}

	return coordinate;
}


//
//	TextEditor::Cursor::adjustForInsert
//

void TextEditor::Cursor::adjustForInsert(Coordinate insertStart, Coordinate insertEnd) {
	start = adjustCoordinateForInsert(start, insertStart, insertEnd);
	end = adjustCoordinateForInsert(end, insertStart, insertEnd);
}


//
//	TextEditor::Cursor::adjustCoordinateForDelete
//

TextEditor::Coordinate TextEditor::Cursor::adjustCoordinateForDelete(Coordinate coordinate, Coordinate start, Coordinate end) {
	coordinate.line -= end.line - start.line;

	if (end.line == coordinate.line) {
		coordinate.column -= end.column;
	}

	return coordinate;
}


//
//	TextEditor::Cursor::adjustForDelete
//

void TextEditor::Cursor::adjustForDelete(Coordinate deleteStart, Coordinate deleteEnd) {
	start = adjustCoordinateForDelete(start, deleteStart, deleteEnd);
	end = adjustCoordinateForDelete(end, deleteStart, deleteEnd);
}


//
//	TextEditor::Cursors::setCursor
//

inline void TextEditor::Cursors::setCursor(Coordinate start, Coordinate end) {
	clear();
	emplace_back(start, end);
	front().setMain(true);
	front().setCurrent(true);
	main = 0;
	current = 0;
}


//
//	TextEditor::Cursors::addCursor
//

inline void TextEditor::Cursors::addCursor(Coordinate start, Coordinate end) {
	at(current).setCurrent(false);
	emplace_back(start, end);
	back().setCurrent(true);
	current = size() - 1;
}


//
//	TextEditor::Cursors::anyHasSelection
//

inline bool TextEditor::Cursors::anyHasSelection() const {
	for (auto cursor = begin(); cursor < end(); cursor++) {
		if (cursor->hasSelection()) {
			return true;
		}
	}

	return false;
}


//
//	TextEditor::Cursors::allHaveSelection
//

bool TextEditor::Cursors::allHaveSelection() const {
	for (auto cursor = begin(); cursor < end(); cursor++) {
		if (!cursor->hasSelection()) {
			return false;
		}
	}

	return true;
}


//
//	TextEditor::Cursors::anyHasUpdate
//

bool TextEditor::Cursors::anyHasUpdate() const {
	for (auto cursor = begin(); cursor < end(); cursor++) {
		if (cursor->isUpdated()) {
			return true;
		}
	}

	return false;
}


//
//	TextEditor::Cursors::clearAll
//

void TextEditor::Cursors::clearAll() {
	clear();
	emplace_back(Coordinate(0, 0));
	front().setMain(true);
	front().setCurrent(true);
	main = 0;
	current = 0;
}


//
//	TextEditor::Cursors::clearAdditional
//

void TextEditor::Cursors::clearAdditional(bool reset) {
	for (auto cursor = begin(); cursor < end();) {
		if (cursor->isMain()) {
			cursor++;

		} else {
			cursor = erase(cursor);
		}
	}

	main = 0;
	current = 0;
	front().setCurrent(true);

	if (reset) {
		front().resetToEnd();
	}
}


//
//	TextEditor::Cursors::clearUpdated
//

void TextEditor::Cursors::clearUpdated() {
	for (auto cursor = begin(); cursor < end(); cursor++) {
		cursor->setUpdated(false);
	}
}


//
//	TextEditor::Cursors::update
//

void TextEditor::Cursors::update() {
	// reset update flags
	clearUpdated();

	//  only sort and potential merge when we have multiple cursors
	if (hasMultiple()) {
		// sort cursors
		std::sort(begin(), end(), [](Cursor& a, Cursor& b) {
			return a.getSelectionStart() < b.getSelectionStart();
		});

		// merge cursors
		for (auto cursor = rbegin(); cursor < rend() - 1;) {
			auto previous = cursor + 1;

			if (previous->getSelectionEnd() >= cursor->getSelectionEnd()) {
				if (cursor->isMain()) {
					previous->setMain(true);
				}

				if (cursor->isCurrent()) {
					previous->setCurrent(true);
				}

				erase((++cursor).base());

			} else if (previous->getSelectionEnd() > cursor->getSelectionStart()) {
				if (cursor->getInteractiveEnd() < cursor->getInteractiveStart()) {
					previous->update(cursor->getSelectionEnd(), previous->getSelectionStart());

				} else {
					previous->update(previous->getSelectionStart(), cursor->getSelectionEnd());
				}

				if (cursor->isMain()) {
					previous->setMain(true);
				}

				if (cursor->isCurrent()) {
					previous->setCurrent(true);
				}

				erase((++cursor).base());

			} else {
				cursor++;
			}
		}

		// find current cursor
		for (auto c = 0; c < size(); c++) {
			if (at(c).isCurrent()) {
				current = c;
			}
		}
	}
}


//
//	TextEditor::Cursors::adjustForInsert
//

void TextEditor::Cursors::adjustForInsert(iterator start, Coordinate insertStart, Coordinate insertEnd) {
	for (auto cursor = start + 1; cursor < end(); cursor++) {
		cursor->adjustForInsert(insertStart, insertEnd);
	}
}


//
//	TextEditor::Cursors::adjustForDelete
//

void TextEditor::Cursors::adjustForDelete(iterator start, Coordinate deleteStart, Coordinate deleteEnd) {
	for (auto cursor = start + 1; cursor < end(); cursor++) {
		cursor->adjustForDelete(deleteStart, deleteEnd);
	}
}


//
//	TextEditor::Line::maxColumn
//

int TextEditor::Line::maxColumn(int tabSize) const {
	// determine the maximum column number for this line
	int column = 0;

	for (auto glyph = begin(); glyph < end(); glyph++) {
		column = (glyph->character == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	TextEditor::Line::columnToIndex
//

int TextEditor::Line::columnToIndex(int column, int tabSize) const {
	// convert a column reference to a glyph index for this line (taking tabs into account)
	int index = 0;
	int c = 0;

	for (auto glyph = begin(); glyph < end(); glyph++) {
		if (c == column) {
			return index;

		} if (c > column) {
			return index - 1;
		}

		c = (glyph->character == '\t') ? ((c / tabSize) + 1) * tabSize : c + 1;
		index++;
	}

	return index;
}


//
//	TextEditor::Line::indexToColumn
//

int TextEditor::Line::indexToColumn(int index, int tabSize) const {
	// convert a glyph index to a column reference for this line (taking tabs into account)
	int column = 0;

	for (auto glyph = begin(); glyph < begin() + index; glyph++) {
		column = (glyph->character == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	TextEditor::Document::setText
//

void TextEditor::Document::setText(const std::string& text) {
	// reset document
	clear();
	emplace_back();

	// process input UTF-8 and generate lines of glyphs
	auto i = text.begin();

	while (i < text.end()) {
		ImWchar character;
		i = CodePointGet(i, &character);

		if (character == '\n') {
			emplace_back();

		} else if (character != '\r') {
			back().emplace_back(Glyph(character, Color::text));
		}
	}

	// colorize the text
	colorize();
	updated = true;
}


//
//	TextEditor::Document::insertText
//

TextEditor::Coordinate TextEditor::Document::insertText(Coordinate start, const std::string& text) {
	auto line = begin() + start.line;
	auto index = getIndex(start);
	auto lineNo = start.line;

	// process input as UTF-8
	auto i = text.begin();

	// process all codepoints
	while (i < text.end()) {
		ImWchar character;
		i = CodePointGet(i, &character);

		if (character == '\n') {
			// split line
			insert(line + 1, Line());
			line = begin() + lineNo;
			auto nextLine = begin() + ++lineNo;

			for (auto i = line->begin() + index; i < line->end(); i++) {
				nextLine->push_back(*i);
			}

			line->erase(line->begin() + index, line->end());
			line = nextLine;
			index = 0;

		} else if (character != '\r') {
			// insert next glyph
			line->insert(line->begin() + (index++), Glyph(character, Color::text));
		}
	}

	// determine end-of insert
	auto end = Coordinate(lineNo, getColumn(static_cast<int>(line - begin()), index));

	// colorize the new lines
	colorize(start.line, end.line);
	updated = true;
	return end;
}


//
//	TextEditor::Document::deleteText
//

void TextEditor::Document::deleteText(Coordinate start, Coordinate end) {
	auto& startLine = at(start.line);
	auto& endLine = at(end.line);
	auto startIndex = getIndex(start);
	auto endIndex = getIndex(end);

	// see if start and end are on the same line
	if (start.line == end.line) {
		startLine.erase(startLine.begin() + startIndex, startLine.begin() + endIndex);

	// start and end are on different lines
	} else {
		// remove start of last line
		endLine.erase(endLine.begin(), endLine.begin() + endIndex);

		// remove full lines
		erase(begin() + start.line + 1, begin() + end.line);

		// remove end of first line
		auto& startLine = at(start.line);
		startLine.erase(startLine.begin() + startIndex, startLine.end());

		// join lines
		auto& nextLine = at(start.line + 1);
		startLine.insert(startLine.end(), nextLine.begin(), nextLine.end());
		erase(begin() + start.line + 1);
	}

	// colorize around the deleted line(s)
	colorize(start.line, (start.line == lines() - 1) ? start.line : start.line + 1);
	updated = true;
}


//
//	TextEditor::Document::getText
//

std::string TextEditor::Document::getText() const {
	// process all glyphs and generate UTF-8 output
	std::string text;
	std::string utf8(4, 0);

	for (auto i = begin(); i < end(); i++) {
		for (auto& glyph : *i) {
			text.append(utf8.begin(), CodePointPut(utf8.begin(), glyph.character));
		}

		if (i < end() - 1) {
			text += "\n";
		}
	}

	return text;
}


//
//	TextEditor::Document::getSectionText
//

std::string TextEditor::Document::getSectionText(Coordinate start, Coordinate end) const {
	std::string section;

	auto lineNo = start.line;
	auto index = getIndex(start);
	auto endIndex = getIndex(end);
	std::string utf8(4, 0);

	while (lineNo < end.line || index < endIndex) {
		auto& line = at(lineNo);

		if (index < line.glyphs()) {
			section.append(utf8.begin(), CodePointPut(utf8.begin(), line[index].character));

			index++;
		} else {
			section += '\n';
			lineNo++;
			index = 0;
		}
	}

	return section;
}


//
//	TextEditor::Document::getLineText
//

std::string TextEditor::Document::getLineText(int line) const {
	return getSectionText(Coordinate(line, 0), Coordinate(line, maxColumn(line)));
}


//
//	TextEditor::Document::maxColumn
//

int TextEditor::Document::maxColumn() const {
	// determine the maximum column number for this document
	int result = 0;

	for (auto& line : *this) {
		result = std::max(result, maxColumn(line));
	}

	return result;
}


//
//	TextEditor::Document::getUp
//

TextEditor::Coordinate TextEditor::Document::getUp(Coordinate from, int lines) const {
	return normalizeCoordinate(Coordinate(from.line - lines, from.column));
}


//
//	TextEditor::Document::getDown
//

TextEditor::Coordinate TextEditor::Document::getDown(Coordinate from, int lines) const {
	return normalizeCoordinate(Coordinate(from.line + lines, from.column));
}


//
//	TextEditor::Document::getLeft
//

TextEditor::Coordinate TextEditor::Document::getLeft(Coordinate from, bool wordMode) const {
	if (wordMode) {
		if (from.column == 0) {
			if (from.line) {
				from = Coordinate(from.line - 1, maxColumn(from.line - 1));
			}
		}

		return findWordStart(from);

	} else {
		auto index = getIndex(from);

		if (index == 0) {
			return (from.line > 0) ? Coordinate(from.line - 1, maxColumn(from.line - 1)) : from;

		} else {
			return Coordinate(from.line, getColumn(from.line, index - 1));
		}
	}
}


//
//	TextEditor::Document::getRight
//

TextEditor::Coordinate TextEditor::Document::getRight(Coordinate from, bool wordMode) const {
	if (wordMode) {
		if (isEndOfLine(from)) {
			if (from.line < lines() - 1) {
				from = Coordinate(from.line + 1, 0);
			}
		}

		return findWordEnd(from);

	} else {
		auto index = getIndex(from);

		if (index == at(from.line).glyphs()) {
			return (from.line < lines() - 1) ? Coordinate(from.line + 1, 0) : from;

		} else {
			return Coordinate(from.line, getColumn(from.line, index + 1));
		}
	}
}


//
//	TextEditor::Document::getTop
//

TextEditor::Coordinate TextEditor::Document::getTop() const {
	return Coordinate(0, 0);
}


//
//	TextEditor::Document::getBottom
//

TextEditor::Coordinate TextEditor::Document::getBottom() const {
	auto lastLine = lines() - 1;
	return Coordinate(lastLine, maxColumn(lastLine));
}


//
//	TextEditor::Document::getStartOfLine
//

TextEditor::Coordinate TextEditor::Document::getStartOfLine(Coordinate from) const {
	return Coordinate(from.line, 0);
}


//
//	TextEditor::Document::getEndOfLine
//

TextEditor::Coordinate TextEditor::Document::getEndOfLine(Coordinate from) const {
	return Coordinate(from.line, maxColumn(from.line));
}


//
//	TextEditor::Document::findWordStart
//

TextEditor::Coordinate TextEditor::Document::findWordStart(Coordinate from) const {
	if (from.column == 0) {
		return from;

	} else {
		auto& line = at(from.line);
		auto index = getIndex(from);
		auto firstCharacter = line[index - 1].character;

		if (CodePointIsSpace(firstCharacter)) {
			while (index > 0 && CodePointIsSpace(line[index - 1].character)) {
				index--;
			}

		} else if (language->isWord(firstCharacter)) {
			while (index > 0 && language->isWord(line[index - 1].character)) {
				index--;
			}

		} else if (language->isPunctuation(firstCharacter)) {
			while (index > 0 && language->isPunctuation(line[index - 1].character)) {
				index--;
			}

		} else if (index > 0) {
			index--;
		}

		return Coordinate(from.line, getColumn(line, index));
	}
}


//
//	TextEditor::Document::findWordEnd
//

TextEditor::Coordinate TextEditor::Document::findWordEnd(Coordinate from) const {
	auto& line = at(from.line);
	auto index = getIndex(from);
	auto size = line.glyphs();

	auto firstCharacter = line[index].character;

	if (CodePointIsSpace(firstCharacter)) {
		while (index < size && CodePointIsSpace(line[index].character)) {
			index++;
		}

	} else if (language->isWord(firstCharacter)) {
		while (index < size && language->isWord(line[index].character)) {
			index++;
		}

	} else if (language->isPunctuation(firstCharacter)) {
		while (index < size && language->isPunctuation(line[index].character)) {
			index++;
		}

	} else if (index < size) {
		index++;
	}

	return Coordinate(from.line, getColumn(line, index));
}


//
//	TextEditor::Document::findText
//

bool TextEditor::Document::findText(Coordinate from, const std::string& text, bool caseSensitive, bool wholeWord, Coordinate& start, Coordinate& end) const {
	// convert input string to vector of codepoints
	std::vector<ImWchar> search;
	auto i = text.begin();

	while (i < text.end()) {
		ImWchar character;
		i = CodePointGet(i, &character);
		search.emplace_back(caseSensitive ? character : CodePointToLower(character));
	}

	// search document
	auto startLine = from.line;
	auto startIndex = getIndex(from);
	auto searchLine = startLine;
	auto searchIndex = startIndex;

	do {
		auto line = searchLine;
		auto index = searchIndex;
		auto lineSize = at(line).glyphs();
		bool done = false;
		int i = 0;

		while (!done && i < search.size()) {
			if (search[i] == '\n') {
				if (index == lineSize) {
					if (line == lines() - 1) {
						done = true;

					} else {
						line++;
						index = 0;
						lineSize = at(line).glyphs();
						i++;
					}

				} else {
					done = true;
				}

			} else {
				if (index == lineSize) {
					done = true;

				} else {
					auto ch = at(line)[index].character;

					if (!caseSensitive) {
						ch = CodePointToLower(ch);
					}

					if (ch == search[i]) {
						index++;
						i++;

					} else {
						done = true;
					}
				}
			}
		}

		if (i == search.size()) {
			start = Coordinate(searchLine, getColumn(searchLine, searchIndex));
			end = Coordinate(line, getColumn(line, index));

			if (!wholeWord || isWholeWord(start, end)) {
				return true;
			}
		}

		if (searchIndex == at(searchLine).glyphs()) {
			searchLine = (searchLine == lines() - 1) ? 0 : searchLine + 1;
			searchIndex = 0;

		} else {
			searchIndex++;
		}

	} while (searchLine != startLine || searchIndex != startIndex);

	return false;
}


//
//	TextEditor::Document::isWholeWord
//

bool TextEditor::Document::isWholeWord(Coordinate start, Coordinate end) const {
	if (start.line != end.line || end.column - start.column < 1) {
		return false;

	} else {
		auto wordStart = findWordStart(Coordinate(start.line, start.column + 1));
		auto wordEnd = findWordEnd(Coordinate(end.line, end.column - 1));
		return start == wordStart && end == wordEnd;
	}
}


//
//	TextEditor::Document::normalizeCoordinate
//

TextEditor::Coordinate TextEditor::Document::normalizeCoordinate(Coordinate coordinate) const {
	Coordinate result;

	if (coordinate.line < 0) {
		result = Coordinate(0, 0);

	} else if (coordinate.line >= lines()) {
		result = Coordinate(lines() - 1, maxColumn(lines() - 1));

	} else {
		result = Coordinate(coordinate.line, std::max(0, std::min(coordinate.column, maxColumn(coordinate.line))));
	}

	return Coordinate(result.line, getColumn(result.line, getIndex(result)));
}

//
//	TextEditor::Document::setLanguage
//

void TextEditor::Document::setLanguage(const Language &definition) {
	language = &definition;
	colorize();
	updated = true;
}


//
//	TextEditor::Document::colorize
//

TextEditor::State TextEditor::Document::colorize(Line& line) {
	auto state = line.state;

	// process all glyphs in this line
	auto glyph = line.begin();
	auto nonWhiteSpace = false;

	while (glyph < line.end()) {
		if (state == State::inText) {
			// special handling for preprocessor lines
			if (language->preprocess && glyph->character != language->preprocess && !CodePointIsSpace(glyph->character)) {
				nonWhiteSpace = true;
			}

			// mark whitespace characters
			if (CodePointIsSpace(glyph->character)) {
				(glyph++)->color = Color::whitespace;

			// handle single line comments
			} else if (language->singleLineComment.size() && line.matches(glyph, language->singleLineComment)) {
				line.setColor(glyph, line.end(), Color::comment);
				glyph = line.end();

			} else if (language->singleLineCommentAlt.size() && line.matches(glyph, language->singleLineCommentAlt)) {
				line.setColor(glyph, line.end(), Color::comment);
				glyph = line.end();

			// are we starting a multiline comment
			} else if (language->commentStart.size() && line.matches(glyph, language->commentStart)) {
				state = State::inComment;
				auto size = language->commentEnd.size();
				line.setColor(glyph, glyph + size, Color::comment);
				glyph += size;

			// are we starting a special string
			} else if (language->otherStringStart.size() && line.matches(glyph, language->otherStringStart)) {
				state = State::inOtherString;
				auto size = language->otherStringStart.size();
				line.setColor(glyph, glyph + size, Color::string);
				glyph += size;

			} else if (language->otherStringAltStart.size() && line.matches(glyph, language->otherStringAltStart)) {
				state = State::inOtherStringAlt;
				auto size = language->otherStringAltStart.size();
				line.setColor(glyph, glyph + size, Color::string);
				glyph += size;

			// are we starting a single quoted string
			} else if (language->hasSingleQuotedStrings && glyph->character == '\'') {
				state = State::inSingleQuotedString;
				(glyph++)->color = Color::string;

			// are we starting a double quoted string
			} else if (language->hasDoubleQuotedStrings && glyph->character == '"') {
				state = State::inDoubleQuotedString;
				(glyph++)->color = Color::string;

			// is this a preprocessor line
			} else if (language->preprocess && !nonWhiteSpace && glyph->character == language->preprocess) {
				line.setColor(line.begin(), line.end(), Color::preprocessor);
				glyph = line.end();

			// nothing worked so far so it's time to do some tokenizing
			} else {
				Iterator start(static_cast<void*>(&line), static_cast<int>(glyph - line.begin()));
				Iterator end(static_cast<void*>(&line), line.glyphs());
				Iterator newEnd;

				// do we have an identifier
				if ((newEnd = language->getIdentifier(start, end)) != start) {
					int size = newEnd - start;

					// determine identifier text and color color
					std::string identifier;
					auto color = Color::identifier;

					for (auto i = start; i < newEnd; i++) {
						identifier += *i;
					}

					if (language->keywords.find(identifier) != language->keywords.end()) {
						color = Color::keyword;

					} else if (language->declarations.find(identifier) != language->declarations.end()) {
						color = Color::declaration;

					} else if (language->identifiers.find(identifier) != language->identifiers.end()) {
						color = Color::knownIdentifier;
					}

					// colorize identifier and move on
					line.setColor(glyph, glyph + size, color);
					glyph += size;

				// do we have a number
				} else if ((newEnd = language->getNumber(start, end)) != start) {
					int size = newEnd - start;
					line.setColor(glyph, glyph + size, Color::number);
					glyph += size;

				// is this punctuation
				} else if (language->isPunctuation(glyph->character)) {
					(glyph++)->color = Color::punctuation;

				} else {
					// I guess we didn't know what this character is
					(glyph++)->color = Color::text;
				}
			}

		} else if (state == State::inComment) {
			// stay in comment state until we see the end sequence
			if (line.matches(glyph, language->commentEnd)) {
				auto size = language->commentEnd.size();
				line.setColor(glyph, glyph + size, Color::comment);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inOtherString) {
			// stay in otherString state until we see the end sequence
			// skip escaped character
			if (glyph->character == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (line.matches(glyph, language->otherStringEnd)) {
				auto size = language->otherStringEnd.size();
				line.setColor(glyph, glyph + size, Color::string);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inOtherStringAlt) {
			// stay in otherStringAlt state until we see the end sequence
			// skip escaped character
			if (glyph->character == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (line.matches(glyph, language->otherStringAltEnd)) {
				auto size = language->otherStringAltEnd.size();
				line.setColor(glyph, glyph + size, Color::string);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inSingleQuotedString) {
			// stay in single quote state until we see an end
			// skip escaped character
			if (glyph->character == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->character == '\'') {
				(glyph++)->color = Color::string;
				state = State::inText;

			} else {
				(glyph++)->color = Color::string;
			}

		} else if (state == State::inDoubleQuotedString) {
			// stay in double quote state until we see an end
			if (glyph->character == language->stringEscape) {
				// skip escaped character
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->character == '"') {
				(glyph++)->color = Color::string;
				state = State::inText;

			} else {
				(glyph++)->color = Color::string;
			}
		}
	}

	return state;
}

void TextEditor::Document::colorize(int start, int end) {
	if (language) {
		for (auto line = start; line <= end; line++) {
			State state = colorize(at(line));

			if (line < lines() - 1) {
				auto& nextLine = at(line + 1);

				if (nextLine.state != state) {
					nextLine.state = state;

					if (line == end) {
						end++;
					}
				}
			}
		}
	}
}

void TextEditor::Document::colorize() {
	if (language) {
		colorize(0, lines() - 1);
	}
}


//
//	TextEditor::Transactions::add
//

void TextEditor::Transactions::add(std::shared_ptr<Transaction> transaction) {
	resize(undoIndex);
	push_back(transaction);
	undoIndex++;
}


//
//	TextEditor::Transactions::undo
//

void TextEditor::Transactions::undo(Document& document, Cursors& cursors) {
	auto& transaction = at(--undoIndex);

	for (auto i = transaction->rbegin(); i < transaction->rend(); i++) {
		if (i->type == Action::Type::insertText) {
			document.deleteText(i->start, i->end);

		} else {
			document.insertText(i->start, i->text);
		}
	}

	cursors = transaction->getBeforeState();
}


//
//	TextEditor::Transactions::redo
//

void TextEditor::Transactions::redo(Document& document, Cursors& cursors) {
	auto& transaction = at(undoIndex++);

	for (auto& action : *transaction) {
		if (action.type == Action::Type::insertText) {
			document.insertText(action.start, action.text);

		} else {
			document.deleteText(action.start, action.end);
		}
	}

	cursors = transaction->getAfterState();
}


//
//	TextEditor::Brackets::update
//

void TextEditor::Brackets::update(Document& document) {
	Color bracketColors[] = {
		Color::matchingBracket1,
		Color::matchingBracket2,
		Color::matchingBracket3
	};

	clear();
	std::vector<size_t> levels;
	int level = 0;

	// process all the glyphs
	for (auto line = 0; line < document.lines(); line++) {
		for (auto index = 0; index < document[line].glyphs(); index++) {
			auto& glyph = document[line][index];

			// handle a bracket opener that is not in a comment, string or preprocessor statement
			if (isBracketCandidate(glyph) && isBracketOpener(glyph.character)) {
				// start a new level
				levels.emplace_back(size());
				emplace_back(glyph.character, Coordinate(line, document.getColumn(line, index)), 0, Coordinate::invalid(), level);
				glyph.color = bracketColors[level % 3];
				level++;

			// handle a bracket closer that is not in a comment, string or preprocessor statement
			} else if (isBracketCandidate(glyph) && isBracketCloser(glyph.character)) {
				if (levels.size()) {
					auto& lastBracket = at(levels.back());
					levels.pop_back();
					level--;

					if (lastBracket.startChar == toBracketOpener(glyph.character)) {
						// handle matching bracket
						glyph.color = bracketColors[level % 3];
						lastBracket.endChar = glyph.character;
						lastBracket.end = Coordinate(line, document.getColumn(line, index));

					} else {
						// no matching bracket, mark brackets as errors
						glyph.color = Color::matchingBracketError;
						document[lastBracket.start.line][document.getIndex(lastBracket.start)].color = Color::matchingBracketError;
						pop_back();
					}

				// this is a closer without an opener
				} else {
					glyph.color = Color::matchingBracketError;
				}
			}
		}
	}

	// handle levels left open and mark them as errors
	if (levels.size()) {
		for (auto i = levels.rbegin(); i < levels.rend(); i++) {
			auto& start = at(*i).start;
			document[start.line][document.getIndex(start)].color = Color::matchingBracketError;
			erase(begin() + *i);
		}
	}
}


//
//	TextEditor::Brackets::getActive
//

TextEditor::Brackets::iterator TextEditor::Brackets::getActive(Coordinate location) {
	if (location != activeLocation) {
		active = end();
		bool done = false;

		for (auto i = begin(); i < end(); i++) {
			// skip pairs that start after specified location
			if (i->isAfter(location)) {
				done = true;
			}

			// brackets are active when they are around specified location
			else if (i->isAround(location)) {
				active = i;
			}
		}

		activeLocation = location;
	}

	return active;
}


//
//	TextEditor::Iterator::operator*
//

TextEditor::Iterator::reference TextEditor::Iterator::operator*() const {
	return static_cast<Line*>(line)->at(index).character;
}


//
//	TextEditor::Iterator::operator->
//

TextEditor::Iterator::pointer TextEditor::Iterator::operator->() const {
	return &(static_cast<Line*>(line)->at(index).character);
}


//
//	TextEditor::updatePalette
//

void TextEditor::updatePalette() {
	// Update palette with the current alpha from style
	paletteAlpha = ImGui::GetStyle().Alpha;

	for (int i = 0; i < static_cast<size_t>(Color::count); i++) {
		auto color = ImGui::ColorConvertU32ToFloat4(paletteBase[i]);
		color.w *= paletteAlpha;
		palette[i] = ImGui::ColorConvertFloat4ToU32(color);
	}
}


//
//	Color palettes
//

const TextEditor::Palette& TextEditor::GetDarkPalette() {
	const static Palette p = {{
		IM_COL32(220, 223, 228, 255),	// standard
		IM_COL32(224, 108, 117, 255),	// keyword
		IM_COL32( 90, 179, 155, 255),	// declaration
		IM_COL32(229, 192, 123, 255),	// number
		IM_COL32(152, 195, 121, 255),	// string
		IM_COL32(106, 115, 132, 255),	// punctuation
		IM_COL32(128, 128,  64, 255),	// preprocessor
		IM_COL32(220, 223, 228, 255),	// identifier
		IM_COL32( 97, 175, 239, 255),	// known identifier
		IM_COL32( 54, 150, 162, 255),	// comment
		IM_COL32( 40,  44,  52, 255),	// background
		IM_COL32(224, 224, 224, 255),	// cursor
		IM_COL32( 32,  96, 160, 128),	// selection
		IM_COL32(255,  32,   0, 128),	// errorMarker
		IM_COL32(255, 255, 255,  21),	// whitespace
		IM_COL32(246, 222,  36, 255),	// matchingBracket1
		IM_COL32( 66, 120, 198, 255),	// matchingBracket2
		IM_COL32(213,  96, 213, 255),	// matchingBracket3
		IM_COL32(198,   8,  32, 255),	// matchingBracketError
		IM_COL32(122, 131, 148, 255),	// line number
		IM_COL32(255, 255, 255, 255),	// current line number
	}};

	return p;
}

const TextEditor::Palette& TextEditor::GetLightPalette()
{
	const static Palette p = {{
		IM_COL32( 64,  64,  64, 255),	// standard
		IM_COL32(  6,  12, 255, 255),	// keyword
		IM_COL32(  6,  12, 255, 255),	// declaration
		IM_COL32(  0, 128,   0, 255),	// number
		IM_COL32(160,  32,  32, 255),	// string
		IM_COL32(  0,   0,   0, 255),	// punctuation
		IM_COL32( 96,  96,  64, 255),	// preprocessor
		IM_COL32( 64,  64,  64, 255),	// identifier
		IM_COL32( 16,  96,  96, 255),	// known identifier
		IM_COL32( 32,  80,  32, 255),	// comment
		IM_COL32(255, 255, 255, 255),	// background
		IM_COL32(  0,   0,   0, 255),	// cursor
		IM_COL32(  0,   0,  96,  64),	// selection
		IM_COL32(255,  16,   0, 160),	// errorMarker
		IM_COL32(144, 144, 144, 144),	// whitespace
		IM_COL32(246, 222,  36, 255),	// matchingBracket1
		IM_COL32( 66, 120, 198, 255),	// matchingBracket2
		IM_COL32(213,  96, 213, 255),	// matchingBracket3
		IM_COL32(198,   8,  32, 255),	// matchingBracketError
		IM_COL32(  0,  80,  80, 255),	// line number
		IM_COL32(  0,   0,   0, 255),	// current line number
	}};

	return p;
}

TextEditor::Palette TextEditor::defaultPalette = TextEditor::GetDarkPalette();
