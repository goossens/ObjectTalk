//	Color Text Editor
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cctype>
#include <unordered_set>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include "OtCodePoint.h"

#include "OtTextWidget.h"


//
//	OtTextWidget::setText
//

void OtTextWidget::setText(const std::string& text) {
	document.setText(text);
	transactions.clear();
	cursors.clearAll();
}


//
//	OtTextWidget::render
//

void OtTextWidget::render(const char* title, const ImVec2& size, bool border) {
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

	// set style and start a new child window
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(getColor(Color::background)));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	int longestLine = document.getMaxColumn(tabSize);
	ImGui::SetNextWindowContentSize({textStart + longestLine * glyphSize.x + cursorWidth, document.lines() * glyphSize.y});
	ImGui::BeginChild(title, size, border, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNavInputs);

	// handle keyboard and mouse inputs
	handleKeyboardInputs();
	handleMouseInteractions();
	cursors.update();

	// ensure cursor is visible (if requested)
	if (ensureCursorIsVisible) {
		makeCursorVisible();
		ensureCursorIsVisible = false;
	}

	// determine view parameters
	float scrollbarSize = ImGui::GetStyle().ScrollbarSize;
	visibleHeight = ImGui::GetWindowHeight() - ((longestLine >= visibleColumns) ? scrollbarSize : 0.0f);
	visibleLines = std::max(static_cast<int>(std::ceil(visibleHeight / glyphSize.y)), 0);
	firstVisibleLine = std::max(static_cast<int>(std::floor(ImGui::GetScrollY() / glyphSize.y)), 0);
	lastVisibleLine = static_cast<int>(std::ceil((ImGui::GetScrollY() + visibleHeight) / glyphSize.y));

	visibleWidth = ImGui::GetWindowWidth() - textStart - ((document.lines() >= visibleLines) ? scrollbarSize : 0.0f);
	visibleColumns = std::max(static_cast<int>(std::ceil(visibleWidth / glyphSize.x)), 0);
	firstVisibleColumn = (std::max(static_cast<int>(std::floor(ImGui::GetScrollX() / glyphSize.x)), 0) / tabSize) * tabSize;
	lastVisibleColumn = ((static_cast<int>(std::ceil((ImGui::GetScrollX() + visibleWidth) / glyphSize.x)) + 1) / tabSize) * tabSize;

	// update cursor animation timer
	auto io = ImGui::GetIO();
	cursorAnimationTimer = std::fmod(cursorAnimationTimer + io.DeltaTime, 1.0f);

	// render all visible lines
	auto drawList = ImGui::GetWindowDrawList();
	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	ImVec2 lineScreenPos = cursorScreenPos + ImVec2(textStart, firstVisibleLine * glyphSize.y);

	for (int i = firstVisibleLine; i <= lastVisibleLine && i < document.lines(); i++) {
		// render single line
		auto& line = document[i];

		// draw selections
		for (auto& cursor : cursors) {
			if (cursor.hasSelection()) {
				int rectStart = -1;
				int rectEnd = -1;
				Coordinate lineStart{i, 0};
				Coordinate lineEnd{i, line.getMaxColumn(tabSize)};
				auto selectionStart = cursor.getSelectionStart();
				auto selectionEnd = cursor.getSelectionEnd();

				// see if this cursor overlaps this line
				if (selectionStart <= lineEnd) {
					rectStart = (selectionStart > lineStart ? selectionStart.column : 0) * glyphSize.x;
				}

				if (selectionEnd > lineStart) {
					rectEnd = (selectionEnd < lineEnd ? selectionEnd.column : lineEnd.column) * glyphSize.x;
				}

				// draw selection background if it does
				if (rectStart != -1 && rectEnd != -1) {
					drawList->AddRectFilled(
						ImVec2{lineScreenPos.x + rectStart, lineScreenPos.y},
						ImVec2{lineScreenPos.x + rectEnd, lineScreenPos.y + glyphSize.y},
						getColor(Color::selection));
				}
			}
		}

		// draw colored line glyphs
		auto column = firstVisibleColumn;
		auto index = line.columnToIndex(column, tabSize);
		auto glyphs = line.glyphs();

		while (index < glyphs && column <= lastVisibleColumn) {
			auto& glyph = line[index];
			auto character = glyph.codepoint;
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

					drawList->AddLine(p1, p2, getColor(Color::whitespace));
					drawList->AddLine(p2, p3, getColor(Color::whitespace));
					drawList->AddLine(p2, p4, getColor(Color::whitespace));
				}

			} else if (character == ' ') {
				if (showWhitespaces) {
					const auto x = glyphPos.x + glyphSize.x * 0.5f;
					const auto y = glyphPos.y + fontSize * 0.5f;
					drawList->AddCircleFilled(ImVec2(x, y), 1.5f, getColor(Color::whitespace), 4);
				}

			} else {
				font->RenderChar(drawList, fontSize, glyphPos, getColor(glyph.color), static_cast<ImWchar>(character));
			}

			index++;
			column += (character == '\t') ? tabSize - (column % tabSize) : 1;
		}

		// draw cursors (if required)
		if (ImGui::IsWindowFocused() && (!io.ConfigInputTextCursorBlink || cursorAnimationTimer < 0.5f)) {
			for (auto& cursor : cursors) {
				auto pos = cursor.getInteractiveEnd();

				if (pos.line == i) {
					auto x = lineScreenPos.x + pos.column * glyphSize.x;
					drawList->AddRectFilled(ImVec2{x, lineScreenPos.y}, ImVec2{x + cursorWidth, lineScreenPos.y + glyphSize.y}, getColor(Color::cursor));
				}
			}
		}

		lineScreenPos.y += glyphSize.y;
	}

	// draw line numbers (if required)
	if (showLineNumbers) {
		// erase background in case we are scrolling horizontally
		if (ImGui::GetScrollX() > 0.0f) {
			drawList->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + ImVec2(textStart, ImGui::GetWindowSize().y), getColor(Color::background));
		}

		auto curserLine = cursors.getCurrent().getInteractiveEnd().line;
		auto lineNumberEnd = ImGui::GetWindowPos().x + textStart;

		for (int i = firstVisibleLine; i <= lastVisibleLine && i < document.lines(); i++) {
				int digits = static_cast<int>(std::log10(i + 1) + 1.0f);
				auto lineNumberWidth = (digits + lineNumberMargin) * glyphSize.x;
				Color foreground =  i == curserLine ? Color::currentLineNumber : Color::lineNumber;
				drawList->AddText(ImVec2(lineNumberEnd - lineNumberWidth, cursorScreenPos.y + i * glyphSize.y), getColor(foreground), std::to_string(i + 1).c_str());
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}


//
//	OtTextWidget::handleKeyboardInputs
//

void OtTextWidget::handleKeyboardInputs() {
	if (ImGui::IsWindowFocused()) {
		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

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
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveTop(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Home)) { moveTop(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveBottom(shift); }
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_End)) { moveBottom(shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_Home)) { moveHome(shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_End)) { moveEnd(shift); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_A)) { selectAll(); }
//		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_D)) { addCursorForNextOccurrence(); }

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
//		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) { changeCurrentLinesIndentation(false);}
//		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_RightBracket)) { changeCurrentLinesIndentation(true); }
//		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUpCurrentLines(); }
//		else if (!readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDownCurrentLines(); }
//		else if (!readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Slash)) { toggleLineComment(); }

		// handle new line
		else if (!readOnly && isNoModifiers && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { handleCharacter('\n'); }
		else if (!readOnly && isShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineBelow(); }
		else if (!readOnly && isShiftShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineAbove(); }

		// handle tabs
		else if (!readOnly && ImGui::IsKeyPressed(ImGuiKey_Tab)) {
			if (cursors.anyHasSelection()) {
//				changeCurrentLinesIndentation(!shift);

			} else {
				handleCharacter('\t');
			}
		}

		// handle regular text
		if (!readOnly && !io.InputQueueCharacters.empty()) {
			for (int i = 0; i < io.InputQueueCharacters.Size; i++) {
				auto character = static_cast<char32_t>(io.InputQueueCharacters[i]);

				if (character == '\n' || character >= 32)
					handleCharacter(character);
			}

			io.InputQueueCharacters.resize(0);
		}
	}
}


//
//	OtTextWidget::handleMouseInteractions
//

void OtTextWidget::handleMouseInteractions() {
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
		), tabSize);

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
				auto end = document.getDown(start, 1, tabSize);
				cursors.updateCursor(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

			} else {
				cursors.updateCursor(mouseCoord);
			}

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
					auto start = document.getHome(mouseCoord);
					auto end = document.getDown(start, 1, tabSize);
					cursors.updateCursor(start, end);
				}

			} else if (doubleClick) {
				// left mouse button double click
				if (!overLineNumbers) {
					auto start = document.findWordStart(mouseCoord, tabSize);
					auto end = document.findWordEnd(mouseCoord, tabSize);
					cursors.updateCursor(start, end);
				}

			} else if (click) {
				// left mouse button single click
				auto extendCursor = ImGui::IsKeyDown(ImGuiMod_Shift);

#if __APPLE__
				auto addCursor = ImGui::IsKeyDown(ImGuiMod_Alt);
#else
				auto addCursor = ImGui::IsKeyDown(ImGuiMod_Ctrl);
#endif

				if (overLineNumbers) {
					auto start = Coordinate(mouseCoord.line, 0);
					auto end = document.getDown(start, 1, tabSize);

					if (extendCursor) {
						auto& cursor = cursors.getCurrent();
						cursors.updateCursor(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

					} else if (addCursor) {
						cursors.addCursor(start, end);

					} else {
						cursors.setCursor(start, end);
					}

				} else {
					if (extendCursor) {
						cursors.updateCursor(mouseCoord);

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
//	OtTextWidget::selectAll
//

void OtTextWidget::selectAll() {
	moveTop(false);
	moveBottom(true);
}


//
//	OtTextWidget::selectLine
//

void OtTextWidget::selectLine(int line) {
	Coordinate start{line, 0};
	moveTo(start, false);
	moveTo(document.getDown(start, 1, tabSize), true);
}


//
//	OtTextWidget::selectLines
//

void OtTextWidget::selectLines(int startLine, int endLine) {
	Coordinate start{startLine, 0};
	moveTo(start, false);
	moveTo(document.getDown(start, endLine - startLine + 1, tabSize), true);
}


//
//	OtTextWidget::cut
//

void OtTextWidget::cut() {
	copy();
	auto transaction = startTransaction();
	deleteTextAtCursors(transaction);
	endTransaction(transaction);
}


//
//	OtTextWidget::copy
//

void OtTextWidget::copy() const {
	std::string text;

	if (cursors.anyHasSelection()) {
		for (auto& cursor : cursors) {
			if (text.size()) {
				text += "\n";
			}

			if (cursor.hasSelection()) {
				text += document.getSectionText(cursor.getSelectionStart(), cursor.getSelectionEnd(), tabSize);

			} else {
				text += document.getLineText(cursor.getSelectionStart().line, tabSize);
			}
		}

	} else {
		for (auto& cursor : cursors) {
			text += document.getLineText(cursor.getSelectionStart().line, tabSize) + "\n";
		}
	}

	ImGui::SetClipboardText(text.c_str());
}


//
//	OtTextWidget::paste
//

void OtTextWidget::paste() {
	if (ImGui::GetClipboardText() != nullptr) {
		auto transaction = startTransaction();
		std::string clipboardText = ImGui::GetClipboardText();
		insertTextAtCursors(transaction, ImGui::GetClipboardText());
		endTransaction(transaction);
	}
}


//
//	OtTextWidget::undo
//

void OtTextWidget::undo() {
	if (transactions.canUndo()) {
		transactions.undo(document, cursors, tabSize);
	}
}


//
//	OtTextWidget::redo
//

void OtTextWidget::redo() {
	if (transactions.canRedo()) {
		transactions.redo(document, cursors, tabSize);
	}
}


//
//	OtTextWidget::selectFirstOccurrenceOf
//

void OtTextWidget::selectFirstOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord) {
}


//
//	OtTextWidget::selectNextOccurrenceOf
//

void OtTextWidget::selectNextOccurrenceOf(const std::string& text, bool caseSensitive, bool wholeWord) {
}


//
//	OtTextWidget::selectAllOccurrencesOf
//

void OtTextWidget::selectAllOccurrencesOf(const std::string& text, bool caseSensitive, bool wholeWord) {
}


//
//	OtTextWidget::replaceTextInCurrentCursor
//

void OtTextWidget::replaceTextInCurrentCursor(const std::string& text) {
}


//
//	OtTextWidget::replaceTextInAllCursors
//

void OtTextWidget::replaceTextInAllCursors(const std::string& text) {
}


//
//	OtTextWidget::moveUp
//

void OtTextWidget::moveUp(int lines, bool select) {
	for (auto& cursor : cursors) {
		cursor.update(document.getUp(cursor.getInteractiveEnd(), lines, tabSize), select);
	}

	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::moveDown
//

void OtTextWidget::moveDown(int lines, bool select) {
	for (auto& cursor : cursors) {
		cursor.update(document.getDown(cursor.getInteractiveEnd(), lines, tabSize), select);
	}

	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::moveLeft
//

void OtTextWidget::moveLeft(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		cursor.update(document.getLeft(cursor.getInteractiveEnd(), wordMode, tabSize), select);
	}

	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::moveRight
//

void OtTextWidget::moveRight(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		cursor.update(document.getRight(cursor.getInteractiveEnd(), wordMode, tabSize), select);
	}

	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::moveTop
//

void OtTextWidget::moveTop(bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(document.getTop(), select);
	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::moveBottom
//

void OtTextWidget::moveBottom(bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(document.getBottom(tabSize), select);
	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::moveHome
//

void OtTextWidget::moveHome(bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(document.getHome(cursors.getCurrent().getInteractiveEnd()), select);
	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::moveEnd
//

void OtTextWidget::moveEnd(bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(document.getEnd(cursors.getCurrent().getInteractiveEnd(), tabSize), select);
	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::moveTo
//

void OtTextWidget::moveTo(Coordinate coordinate, bool select) {
	cursors.clearAdditional();
	cursors.updateCursor(coordinate, select);
	ensureCursorIsVisible = true;
}


//
//	OtTextWidget::makeCursorVisible
//

void OtTextWidget::makeCursorVisible() {
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
}


//
//	OtTextWidget::handleCharacter
//

void OtTextWidget::handleCharacter(char32_t codepoint) {
	auto transaction = startTransaction();
	std::string utf8(4, 0);
	std::string text(utf8.begin(), OtCodePoint::put(utf8.begin(), codepoint));
	insertTextAtCursors(transaction, text);
	endTransaction(transaction);
}


//
//	OtTextWidget::handleBackspace
//

void OtTextWidget::handleBackspace(bool wordMode) {
	auto transaction = startTransaction();

	for (auto i = cursors.rbegin(); i < cursors.rend(); i++) {
		if (i->hasSelection()) {
			deleteText(transaction, i->getSelectionStart(), i->getSelectionEnd());
			i->update(i->getSelectionStart(), false);

		} else {
			auto start = document.getLeft(i->getSelectionEnd(), wordMode, tabSize);
			deleteText(transaction, start, i->getSelectionEnd());
			i->update(start, false);
		}
	}

	endTransaction(transaction);
}


//
//	OtTextWidget::handleDelete
//

void OtTextWidget::handleDelete(bool wordMode) {
	auto transaction = startTransaction();

	for (auto i = cursors.rbegin(); i < cursors.rend(); i++) {
		if (i->hasSelection()) {
			deleteText(transaction, i->getSelectionStart(), i->getSelectionEnd());
			i->update(i->getSelectionStart(), false);

		} else {
			auto end = document.getRight(i->getSelectionEnd(), wordMode, tabSize);
			deleteText(transaction, i->getSelectionEnd(), end);
			i->update(i->getSelectionEnd(), false);
		}
	}

	endTransaction(transaction);
}


//
//	OtTextWidget::removeSelectedLines
//

void OtTextWidget::removeSelectedLines() {
	auto transaction = startTransaction();

	for (auto i = cursors.begin(); i < cursors.end(); i++) {
		Coordinate start = document.getHome(i->getSelectionStart());
		Coordinate end = document.getDown(start, 1, tabSize);
		deleteText(transaction, start, end);
		i->update(start, false);

		// adjust subsequent cursors
		for (auto j = i + 1; j < cursors.end(); j++) {
			j->adjustForDelete(start, end);
		}
	}

	endTransaction(transaction);
}


//
//	OtTextWidget::insertLineAbove
//

void OtTextWidget::insertLineAbove() {
	auto transaction = startTransaction();

	for (auto i = cursors.begin(); i < cursors.end(); i++) {
		Coordinate start = document.getHome(i->getSelectionStart());
		Coordinate end = insertText(transaction, start, "\n");
		i->update(start, false);

		// adjust subsequent cursors
		for (auto j = i + 1; j < cursors.end(); j++) {
			j->adjustForInsert(start, end);
		}
	}

	endTransaction(transaction);
}


//
//	OtTextWidget::insertLineBelow
//

void OtTextWidget::insertLineBelow() {
	auto transaction = startTransaction();

	for (auto i = cursors.begin(); i < cursors.end(); i++) {
		Coordinate start = document.getDown(document.getHome(i->getSelectionStart()), 1, tabSize);
		Coordinate end = insertText(transaction, start, "\n");
		i->update(start, false);

		// adjust subsequent cursors
		for (auto j = i + 1; j < cursors.end(); j++) {
			j->adjustForInsert(start, end);
		}
	}

	endTransaction(transaction);
}


//
//	OtTextWidget::insertTextAtCursors
//

void OtTextWidget::insertTextAtCursors(std::shared_ptr<Transaction> transaction, const std::string& text) {
	// delete any selection content first
	deleteTextAtCursors(transaction);

	for (auto i = cursors.begin(); i < cursors.end(); i++) {
		// insert the text
		Coordinate start = i->getSelectionStart();
		Coordinate end = insertText(transaction, start, text);
		i->update(end, false);

		// adjust subsequent cursors
		for (auto j = i + 1; j < cursors.end(); j++) {
			j->adjustForInsert(start, end);
		}
	}
}


//
//	OtTextWidget::deleteTextAtCursors
//

void OtTextWidget::deleteTextAtCursors(std::shared_ptr<Transaction> transaction) {
	for (auto i = cursors.begin(); i < cursors.end(); i++) {
		if (i->hasSelection()) {
			// delete selection
			Coordinate start = i->getSelectionStart();
			Coordinate end = i->getSelectionEnd();
			deleteText(transaction, start, end);
			i->update(start, false);

			// adjust subsequent cursors
			for (auto j = i + 1; j < cursors.end(); j++) {
				j->adjustForDelete(start, end);
			}
		}
	}
}


//
//	OtTextWidget::startTransaction
//

std::shared_ptr<OtTextWidget::Transaction> OtTextWidget::startTransaction() {
	std::shared_ptr<Transaction> transaction = Transactions::create();
	transaction->setBeforeState(cursors);
	return transaction;
}


//
//	OtTextWidget::endTransaction
//

bool OtTextWidget::endTransaction(std::shared_ptr<Transaction> transaction) {
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
//	OtTextWidget::insertText
//

OtTextWidget::Coordinate OtTextWidget::insertText(std::shared_ptr<Transaction> transaction, Coordinate start, const std::string& text) {
	// update document, add transaction and return coordinate of end of insert
	auto end = document.insertText(start, text, tabSize);
	transaction->addInsert(start, end, text);
	return end;
}


//
//	OtTextWidget::deleteText
//

void OtTextWidget::deleteText(std::shared_ptr<Transaction> transaction, Coordinate start, Coordinate end) {
	auto text = document.getSectionText(start, end, tabSize);
	document.deleteText(start, end, tabSize);
	transaction->addDelete(start, end, text);
}


//
//	OtTextWidget::stripTrailingWhitespaces
//

void OtTextWidget::stripTrailingWhitespaces() {
	auto transaction = startTransaction();

	// process all the lines
	for (int i = 0; i < document.lines(); i++) {
		auto& line = document[i];
		int size = line.glyphs();
		int whitespace = -1;
		bool done = false;

		// look for first non-whitespace glyph at the end of the line
		for (auto index = size - 1; !done && index >= 0; index--) {
			if (OtCodePoint::isWhiteSpace(line[index].codepoint)) {
				whitespace = index;

			} else {
				done = true;
			}
		}

		// remove whitespaces (if required)
		if (whitespace >= 0) {
			auto start = Coordinate(i, line.indexToColumn(whitespace, tabSize));
			auto end = Coordinate(i, line.indexToColumn(size, tabSize));
			deleteText(transaction, start, end);
		}
	}

	// update cursor if transaction wasn't empty
	if (endTransaction(transaction)) {
		cursors.setCursor(document.normalizeCoordinate(cursors.getCurrent().getSelectionEnd(), tabSize));
	}
}


//
//	OtTextWidget::filterLines
//

void OtTextWidget::filterLines(std::function<std::string(std::string)> filter) {
	auto transaction = startTransaction();

	// process all the lines
	for (int i = 0; i < document.lines(); i++) {
		// get original text and run it through filter
		auto before = document.getLineText(i, tabSize);
		std::string after = filter(before);

		// update line if anything changed
		if (after != before) {
			auto start = Coordinate(i, 0);
			auto end = document.getEnd(start, tabSize);
			deleteText(transaction, start, end);
			insertText(transaction, start, after);
		}
	}

	// update cursor if transaction wasn't empty
	if (endTransaction(transaction)) {
		cursors.setCursor(document.normalizeCoordinate(cursors.getCurrent().getSelectionEnd(), tabSize));
	}
}


//
//	OtTextWidget::tabsToSpaces
//

void OtTextWidget::tabsToSpaces() {
	filterLines([this](const std::string& input) {
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
//	OtTextWidget::spacesToTabs
//

void OtTextWidget::spacesToTabs() {
	FilterLines([this](const std::string& input) {
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
//	OtTextWidget::Cursor::adjustCoordinateForInsert
//

OtTextWidget::Coordinate OtTextWidget::Cursor::adjustCoordinateForInsert(Coordinate coordinate, Coordinate start, Coordinate end) {
	coordinate.line += end.line - start.line;

	if (end.line == coordinate.line) {
		coordinate.column += end.column;
	}

	return coordinate;
}


//
//	OtTextWidget::Cursor::adjustForInsert
//

void OtTextWidget::Cursor::adjustForInsert(Coordinate insertStart, Coordinate insertEnd) {
	start = adjustCoordinateForInsert(start, insertStart, insertEnd);
	end = adjustCoordinateForInsert(end, insertStart, insertEnd);
}


//
//	OtTextWidget::Cursor::adjustCoordinateForDelete
//

OtTextWidget::Coordinate OtTextWidget::Cursor::adjustCoordinateForDelete(Coordinate coordinate, Coordinate start, Coordinate end) {
	coordinate.line -= end.line - start.line;

	if (end.line == coordinate.line) {
		coordinate.column -= end.column;
	}

	return coordinate;
}

//
//	OtTextWidget::Cursor::adjustForDelete
//

void OtTextWidget::Cursor::adjustForDelete(Coordinate deleteStart, Coordinate deleteEnd) {
	start = adjustCoordinateForDelete(start, deleteStart, deleteEnd);
	end = adjustCoordinateForDelete(end, deleteStart, deleteEnd);
}


//
//	OtTextWidget::Cursors::setCursor
//

inline void OtTextWidget::Cursors::setCursor(Coordinate start, Coordinate end) {
	clear();
	emplace_back(start, end);
	current = 0;
}


//
//	OtTextWidget::Cursors::addCursor
//

inline void OtTextWidget::Cursors::addCursor(Coordinate start, Coordinate end) {
	at(current).setCurrent(false);
	emplace_back(start, end);
	current = size() - 1;
}


//
//	OtTextWidget::Cursors::anyHasSelection
//

inline bool OtTextWidget::Cursors::anyHasSelection() const {
	for (auto i = begin(); i < end(); i++) {
		if (i->hasSelection()) {
			return true;
		}
	}

	return false;
}

//
//	OtTextWidget::Cursors::allHaveSelection
//

bool OtTextWidget::Cursors::allHaveSelection() const {
	for (auto i = begin(); i < end(); i++) {
		if (!i->hasSelection()) {
			return false;
		}
	}

	return true;
}


//
//	OtTextWidget::Cursors::anyHasUpdate
//

bool OtTextWidget::Cursors::anyHasUpdate() const {
	for (auto i = begin(); i < end(); i++) {
		if (i->isUpdated()) {
			return true;
		}
	}

	return false;
}


//
//	OtTextWidget::Cursors::clearAll
//

void OtTextWidget::Cursors::clearAll()  {
	clear();
	emplace_back(Coordinate(0, 0));
	current = 0;
}


//
//	OtTextWidget::Cursors::clearAdditional
//

void OtTextWidget::Cursors::clearAdditional() {
	for (auto i = begin(); i < end();) {
		if (!i->isCurrent()) {
			i = erase(i);

		} else {
			i++;
		}
	}

	current = 0;
}


//
//	OtTextWidget::Cursors::clearUpdated
//

void OtTextWidget::Cursors::clearUpdated() {
	for (auto i = begin(); i < end(); i++) {
		i->setUpdated(false);
	}
}


//
//	OtTextWidget::Cursors::update
//

void OtTextWidget::Cursors::update() {
	if (anyHasUpdate()) {
		// reset update flags
		clearUpdated();

		//  only sort and potential merge when we have multiple cursors
		if (hasMultiple()) {
			// sort cursors
			std::sort(begin(), end(), [](Cursor& a, Cursor& b) {
				return a.getSelectionStart() < b.getSelectionStart();
			});

			// merge cursors
			for (auto i = rbegin(); i < rend() - 1;) {
				auto previous = i + 1;

				if (previous->getSelectionEnd() >= i->getSelectionEnd()) {
					if (i->isCurrent()) {
						previous->setCurrent(true);
					}

					erase((++i).base());

				} else if (previous->getSelectionEnd() > i->getSelectionStart()) {
					if (i->getInteractiveEnd() < i->getInteractiveStart()) {
						previous->update(i->getSelectionEnd(), previous->getSelectionStart());

					} else {
						previous->update(previous->getSelectionStart(), i->getSelectionEnd());
					}

					if (i->isCurrent()) {
						previous->setCurrent(true);
					}

					erase((++i).base());

				} else {
					i++;
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
}

//
//	OtTextWidget::Line::getMaxColumn
//

int OtTextWidget::Line::getMaxColumn(int tabSize) const {
	// determine the maximum column number for this line
	int column = 0;

	for (auto i = begin(); i < end(); i++) {
		column = (i->codepoint == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	OtTextWidget::Line::columnToIndex
//

int OtTextWidget::Line::columnToIndex(int column, int tabSize) const {
	// convert a column reference to a glyph index for this line (taking tabs into account)
	int index = 0;
	int c = 0;

	for (auto i = begin(); i < end(); i++) {
		if (c >= column) {
			return index;
		}

		c = (i->codepoint == '\t') ? ((c / tabSize) + 1) * tabSize : c + 1;
		index++;
	}

	return index;
}


//
//	OtTextWidget::Line::indexToColumn
//

int OtTextWidget::Line::indexToColumn(int index, int tabSize) const {
	// convert a glyph index to a column reference for this line (taking tabs into account)
	int column = 0;

	for (auto i = begin(); i < begin() + index; i++) {
		column = (i->codepoint == '\t') ? ((column / tabSize) + 1) * tabSize : column + 1;
	}

	return column;
}


//
//	OtTextWidget::Document::setText
//

void OtTextWidget::Document::setText(const std::string& text) {
	// reset document
	clear();
	emplace_back();

	// process input UTF-8 and generate lines of glyphs
	auto i = text.begin();

	while (i < text.end()) {
		char32_t codepoint;
		i = OtCodePoint::get(i, &codepoint);

		if (codepoint == '\n') {
			emplace_back();

		} else if (codepoint != '\r') {
			back().emplace_back(Glyph(codepoint, Color::standard));
		}
	}
}


//
//	OtTextWidget::Document::getText
//

std::string OtTextWidget::Document::getText() const {
	// process all glyphs and generate UTF-8 output
	std::string text;
	std::string utf8(4, 0);

	for (auto i = begin(); i < end(); i++) {
		for (auto& glyph : *i) {
			text.append(utf8.begin(), OtCodePoint::put(utf8.begin(), glyph.codepoint));
		}

		text += "\n";
	}

	return text;
}


//
//	OtTextWidget::Document::getSectionText
//

std::string OtTextWidget::Document::getSectionText(Coordinate start, Coordinate end, int tabSize) const {
	std::string section;

	auto lineNo = start.line;
	auto index = at(start.line).columnToIndex(start.column, tabSize);
	auto endIndex = at(end.line).columnToIndex(end.column, tabSize);
	std::string utf8(4, 0);

	while (lineNo < end.line || index < endIndex) {
		auto& line = at(lineNo);

		if (index < line.glyphs()) {
			section.append(utf8.begin(), OtCodePoint::put(utf8.begin(), line[index].codepoint));

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
//	OtTextWidget::Document::getLineText
//

std::string OtTextWidget::Document::getLineText(int line, int tabSize) const {
	return getSectionText(Coordinate(line, 0), Coordinate(line, at(line).getMaxColumn(tabSize)), tabSize);
}


//
//	OtTextWidget::Document::insertText
//

OtTextWidget::Coordinate OtTextWidget::Document::insertText(Coordinate from, const std::string& text, int tabSize) {
	auto line = begin() + from.line;
	auto index = line->columnToIndex(from.column, tabSize);
	auto lineNo = from.line;

	// process input as UTF-8
	auto i = text.begin();

	// process all codepoints
	while (i < text.end()) {
		char32_t codepoint;
		i = OtCodePoint::get(i, &codepoint);

		if (codepoint == '\n') {
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

		} else if (codepoint != '\r') {
			// insert next glyph
			line->insert(line->begin() + (index++), Glyph(codepoint, Color::standard));
		}
	}

	return Coordinate(lineNo, line->indexToColumn(index, tabSize));
}


//
//	OtTextWidget::Document::deleteText
//

void OtTextWidget::Document::deleteText(Coordinate start, Coordinate end, int tabSize) {
	auto& startLine = at(start.line);
	auto& endLine = at(end.line);
	auto startIndex = startLine.columnToIndex(start.column, tabSize);
	auto endIndex = endLine.columnToIndex(end.column, tabSize);

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
}


//
//	OtTextWidget::Document::getMaxColumn
//

int OtTextWidget::Document::getMaxColumn(int tabSize) const {
	// determine the maximum column number for this document
	int maxColumn = 0;

	for (auto& line : *this) {
		maxColumn = std::max(maxColumn, line.getMaxColumn(tabSize));
	}

	return maxColumn;
}


//
//	OtTextWidget::Document::getUp
//

OtTextWidget::Coordinate OtTextWidget::Document::getUp(Coordinate from, int lines, int tabSize) const {
	return normalizeCoordinate(Coordinate(from.line - lines, from.column), tabSize);
}


//
//	OtTextWidget::Document::getDown
//

OtTextWidget::Coordinate OtTextWidget::Document::getDown(Coordinate from, int lines, int tabSize) const {
	return normalizeCoordinate(Coordinate(from.line + lines, from.column), tabSize);
}


//
//	OtTextWidget::Document::getLeft
//

OtTextWidget::Coordinate OtTextWidget::Document::getLeft(Coordinate from, bool wordMode, int tabSize) const {
	if (wordMode) {
		if (from.column == 0) {
			if (from.line) {
				from = Coordinate(from.line - 1, at(from.line - 1).getMaxColumn(tabSize));
			}
		}

		return findWordStart(from, tabSize);

	} else {
		auto index = at(from.line).columnToIndex(from.column, tabSize);

		if (index == 0) {
			return (from.line > 0) ? Coordinate(from.line - 1, at(from.line - 1).getMaxColumn(tabSize)) : from;

		} else {
			return Coordinate(from.line, at(from.line).indexToColumn(index - 1, tabSize));
		}
	}
}


//
//	OtTextWidget::Document::getRight
//

OtTextWidget::Coordinate OtTextWidget::Document::getRight(Coordinate from, bool wordMode, int tabSize) const {
	if (wordMode) {
		if (isEndOfLine(from, tabSize)) {
			if (from.line < lines() - 1) {
				from = Coordinate(from.line + 1, 0);
			}
		}

		return findWordEnd(from, tabSize);

	} else {
		auto index = at(from.line).columnToIndex(from.column, tabSize);

		if (index == at(from.line).glyphs()) {
			return (from.line < lines() - 1) ? Coordinate(from.line + 1, 0) : from;

		} else {
			return Coordinate(from.line, at(from.line).indexToColumn(index + 1, tabSize));
		}
	}
}


//
//	OtTextWidget::Document::getTop
//

OtTextWidget::Coordinate OtTextWidget::Document::getTop() const {
	return Coordinate(0, 0);
}


//
//	OtTextWidget::Document::getBottom
//

OtTextWidget::Coordinate OtTextWidget::Document::getBottom(int tabSize) const {
	auto lastLine = lines() - 1;
	return Coordinate(lastLine, at(lastLine).getMaxColumn(tabSize));
}


//
//	OtTextWidget::Document::getHome
//

OtTextWidget::Coordinate OtTextWidget::Document::getHome(Coordinate from) const {
	return Coordinate(from.line, 0);
}


//
//	OtTextWidget::Document::getEnd
//

OtTextWidget::Coordinate OtTextWidget::Document::getEnd(Coordinate from, int tabSize) const {
	return Coordinate(from.line, at(from.line).getMaxColumn(tabSize));
}


//
//	OtTextWidget::Document::normalizeCoordinate
//

OtTextWidget::Coordinate OtTextWidget::Document::normalizeCoordinate(Coordinate coordinate, int tabSize) const {
	if (coordinate.line < 0) {
		return Coordinate(0, 0);

	} else if (coordinate.line >= lines()) {
		return Coordinate(lines() - 1, at(lines() - 1).getMaxColumn(tabSize));

	} else {
		return Coordinate(coordinate.line, std::max(0, std::min(coordinate.column, at(coordinate.line).getMaxColumn(tabSize))));
	}
}


//
//	OtTextWidget::Document::isEndOfLine
//

bool OtTextWidget::Document::isEndOfLine(Coordinate from, int tabSize) const {
	auto& line = at(from.line);
	auto index = line.columnToIndex(from.column, tabSize);
	return index == line.glyphs();
}


//
//	OtTextWidget::Document::findWordStart
//

OtTextWidget::Coordinate OtTextWidget::Document::findWordStart(Coordinate from, int tabSize) const {
	if (from.column == 0) {
		return from;

	} else {
		auto& line = at(from.line);
		auto index = line.columnToIndex(from.column, tabSize);
		auto firstCharacter = line[index - 1].codepoint;

		if (OtCodePoint::isWhiteSpace(firstCharacter)) {
			while (index > 0 && OtCodePoint::isWhiteSpace(line[index - 1].codepoint)) {
				index--;
			}

		} else if (OtCodePoint::isAlphaNumeric(firstCharacter)) {
			while (index > 0 && OtCodePoint::isAlphaNumeric(line[index - 1].codepoint)) {
				index--;
			}

		} else if (index > 0) {
			index--;
		}

		return Coordinate(from.line, line.indexToColumn(index, tabSize));
	}
}


//
//	OtTextWidget::Document::findWordEnd
//

OtTextWidget::Coordinate OtTextWidget::Document::findWordEnd(Coordinate from, int tabSize) const {
	auto& line = at(from.line);
	auto index = line.columnToIndex(from.column, tabSize);
	auto size = line.glyphs();

	auto firstCharacter = line[index].codepoint;

	if (OtCodePoint::isWhiteSpace(firstCharacter)) {
		while (index < size && OtCodePoint::isWhiteSpace(line[index].codepoint)) {
			index++;
		}

	} else if (OtCodePoint::isAlphaNumeric(firstCharacter)) {
		while (index < size && OtCodePoint::isAlphaNumeric(line[index].codepoint)) {
			index++;
		}

	} else if (index < size) {
		index++;
	}

	return Coordinate(from.line, line.indexToColumn(index, tabSize));
}


//
//	OtTextWidget::Transactions::add
//

void OtTextWidget::Transactions::add(std::shared_ptr<Transaction> transaction) {
	resize(undoIndex);
	push_back(transaction);
	undoIndex++;
}


//
//	OtTextWidget::Transactions::undo
//

void OtTextWidget::Transactions::undo(Document& document, Cursors& cursors, int tabSize) {
	auto& transaction = at(--undoIndex);

	for (auto i = transaction->rbegin(); i < transaction->rend(); i++) {
		if (i->type == Action::Type::insertText) {
			document.deleteText(i->start, i->end, tabSize);

		} else {
			document.insertText(i->start, i->text, tabSize);
		}
	}

	cursors = transaction->getBeforeState();
}


//
//	OtTextWidget::Transactions::redo
//

void OtTextWidget::Transactions::redo(Document& document, Cursors& cursors, int tabSize) {
	auto& transaction = at(undoIndex++);

	for (auto& action : *transaction) {
		if (action.type == Action::Type::insertText) {
			document.insertText(action.start, action.text, tabSize);

		} else {
			document.deleteText(action.start, action.end, tabSize);
		}
	}

	cursors = transaction->getAfterState();
}


//
//	OtTextWidget::updatePalette
//

void OtTextWidget::updatePalette() {
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

const OtTextWidget::Palette& OtTextWidget::GetDarkPalette() {
	const static Palette p = { {
		IM_COL32(220, 223, 228, 255),	// standard
		IM_COL32(224, 108, 117, 255),	// keyword
		IM_COL32(229, 192, 123, 255),	// number
		IM_COL32(152, 195, 121, 255),	// string
		IM_COL32(224, 160, 112, 255),	// character
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
		IM_COL32(122, 131, 148, 255),	// line number
		IM_COL32(255, 255, 255, 255),	// current line number
	} };

	return p;
}

const OtTextWidget::Palette& OtTextWidget::GetLightPalette()
{
	const static Palette p = { {
		IM_COL32( 64,  64,  64, 255),	// standard
		IM_COL32(  6,  12, 255, 255),	// keyword
		IM_COL32(  0, 128,   0, 255),	// number
		IM_COL32(160,  32,  32, 255),	// string
		IM_COL32(112,  64,  48, 255),	// character
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
		IM_COL32(  0,  80,  80, 255),	// line number
		IM_COL32(  0,   0,   0, 255),	// current line number
	} };

	return p;
}

const OtTextWidget::Palette& OtTextWidget::GetRetroBluePalette() {
	const static Palette p = { {
		IM_COL32(255, 255,   0, 255),	// standard
		IM_COL32(  0, 255, 255, 255),	// keyword
		IM_COL32(  0, 255,   0, 255),	// number
		IM_COL32(  0, 128, 128, 255),	// string
		IM_COL32(  0, 128, 128, 255),	// character
		IM_COL32(255, 255, 255, 255),	// punctuation
		IM_COL32(  0, 128,   0, 255),	// preprocessor
		IM_COL32(255, 255,   0, 255),	// identifier
		IM_COL32(255, 255, 255, 255),	// known identifier
		IM_COL32(128, 128, 128, 255),	// comment
		IM_COL32(  0,   0, 128, 255),	// background
		IM_COL32(255, 128,   0, 255),	// cursor
		IM_COL32(  0, 255, 255, 128),	// selection
		IM_COL32(255,   0,   0, 160),	// errorMarker
		IM_COL32(255, 255, 255,  80),	// whitespace
		IM_COL32(  0, 128, 128, 255),	// line number
		IM_COL32(255, 255, 255, 255),	// current line number
	} };

	return p;
}

const OtTextWidget::Palette& OtTextWidget::GetMarianaPalette() {
	const static Palette p = { {
		IM_COL32(255, 255, 255, 255),	// standard
		IM_COL32(198, 149, 198, 255),	// keyword
		IM_COL32(249, 174,  88, 255),	// number
		IM_COL32(153, 199, 148, 255),	// string
		IM_COL32(224, 160, 112, 255),	// character
		IM_COL32( 95, 180, 180, 255),	// punctuation
		IM_COL32(128, 128,  64, 255),	// preprocessor
		IM_COL32(255, 255, 255, 255),	// identifier
		IM_COL32(224, 160, 255, 255),	// preprocessor
		IM_COL32(166, 172, 185, 255),	// comment
		IM_COL32( 48,  56,  65, 255),	// background
		IM_COL32(224, 224, 224, 255),	// cursor
		IM_COL32(110, 122, 133, 128),	// selection
		IM_COL32(236,  95, 102, 128),	// errorMarker
		IM_COL32(255, 255, 255,  48),	// whitespace
		IM_COL32(255, 255, 255, 176),	// line number
		IM_COL32(255, 255, 255, 255),	// current line number
	} };

	return p;
}

OtTextWidget::Palette OtTextWidget::defaultPalette = OtTextWidget::GetDarkPalette();
