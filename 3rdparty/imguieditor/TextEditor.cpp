//	TextEditor - A syntax highlighting text editor for Dear ImGui.
//	Copyright (c) 2024-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_internal.h"

#include "TextEditor.h"


//
//	TextEditor::TextEditor
//

TextEditor::TextEditor() {
	SetPalette(defaultPalette);
}


//
//	TextEditor::setText
//

void TextEditor::setText(const std::string_view& text) {
	// load text into document and reset overlays
	document.setText(config, text);
	transactions.reset();
	cursors.clearAll();
	clearMarkers();
	makeCursorVisible();
}


//
//	TextEditor::render
//

void TextEditor::render(const char* title, const ImVec2& size, bool border) {
	// get font information
	font = ImGui::GetFont();
	fontSize = ImGui::GetFontSize();
	glyphSize = ImVec2(ImGui::CalcTextSize("#").x, ImGui::GetTextLineHeightWithSpacing() * config.lineSpacing);

	// ensure editor has focus (if required)
	if (focusOnEditor) {
		ImGui::SetNextWindowFocus();
		focusOnEditor = false;
	}

	// determine window flags
	ImGuiChildFlags childFlags = border ? ImGuiChildFlags_Borders : ImGuiChildFlags_None;

	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_HorizontalScrollbar |
		ImGuiWindowFlags_NoNavInputs |
		ImGuiWindowFlags_NoMove;

	// start a new child window
	ImGui::SetNextWindowContentSize(totalSize);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(palette.get(Color::background)));
	editorVisible = ImGui::BeginChild(title, size, childFlags, windowFlags);

	if (editorVisible) {
		// determine current position and visible size
		cursorScreenPos = ImGui::GetCursorScreenPos();
		visibleSize = ImGui::GetCurrentWindow()->InnerRect.GetSize();

		// determine horizontal offsets for line numbers, decorations and text
		lineNumberLeftOffset = leftMargin * glyphSize.x;

		if (config.showLineNumbers) {
			size_t digits = static_cast<size_t>(std::log10(static_cast<float>(document.size() + 1)) + 1.0f);
			lineNumberRightOffset = lineNumberLeftOffset + digits * glyphSize.x;
			decorationOffset = lineNumberRightOffset + decorationMargin * glyphSize.x;

		} else {
			lineNumberRightOffset = lineNumberLeftOffset;
			decorationOffset = lineNumberLeftOffset;
		}

		if (decoratorWidth > 0.0f) {
			textLeftOffset = decorationOffset + decoratorWidth + decorationMargin * glyphSize.x;

		} else if (decoratorWidth < 0.0f) {
			textLeftOffset = decorationOffset + (-decoratorWidth + decorationMargin) * glyphSize.x;

		} else {
			textLeftOffset = decorationOffset + textMargin * glyphSize.x;
		}

		foldIndicatorOffset = textLeftOffset - glyphSize.x;
		textRightOffset = visibleSize.x - (config.showMiniMap ? miniMapWidth : 0.0f);
		miniMapOffset = textRightOffset;

		// determine number of columns at which text will wrap
		textSize = visibleSize - ImVec2(textLeftOffset + (config.showMiniMap ? miniMapWidth : 0.0f), 0.0f);
		config.wordWrapColumns = static_cast<size_t>(std::max(std::floor(textSize.x / glyphSize.x), 0.0f));

		// handle possible state changes caused by API calls before first frame or between frames
		updateState();
		handlePossibleScrolling();

		// handle keyboard inputs
		handleKeyboardInputs();
		updateState();

		// handle mouse inputs
		handleMouseInteractions();

		// determine visible row/column limits
		firstVisibleRow = std::max(static_cast<size_t>(std::floor(ImGui::GetScrollY() / glyphSize.y)), static_cast<size_t>(0));
		lastVisibleRow = std::min(static_cast<size_t>(std::ceil((ImGui::GetScrollY() + textSize.y) / glyphSize.y)), typeSetter.getRowCount() - 1);
		firstVisibleColumn = std::max(static_cast<size_t>(std::floor(ImGui::GetScrollX() / glyphSize.x)), static_cast<size_t>(0));
		lastVisibleColumn = static_cast<size_t>(std::ceil((ImGui::GetScrollX() + textSize.x) / glyphSize.x));

		// update color palette (if required)
		if (paletteAlpha != ImGui::GetStyle().Alpha) {
			updatePalettes();
		}

		// setup clipping over the text area
		auto drawList = ImGui::GetWindowDrawList();

		drawList->PushClipRect(
			ImVec2(textLeftOffset, drawList->GetClipRectMin().y),
			ImVec2(textRightOffset, drawList->GetClipRectMax().y),
			false);

		// render parts in the text area
		renderActiveBracketBackground();
		renderSelections();
		renderMarkers();
		renderMatchingBracketLines();
		renderText();
		renderCursors();

		// end to clipping
		drawList->PopClipRect();

		// render other parts
		renderLineNumbers();
		renderDecorations();
		renderFoldIndicators();
		renderMiniMap();
		renderScrollbarMiniMap();
		renderPanScrollIndicator();
		renderFindReplace();
		renderPopups();

		// handle scrolling caused by actions in this frame
		handlePossibleScrolling();
	}

	// handle change tracking if there is a callback in place
	if (delayedChangeCallback && delayedChangeDetected) {
		if (std::chrono::system_clock::now() > delayedChangeReportTime) {
			delayedChangeCallback();
			delayedChangeDetected = false;
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}


//
//	TextEditor::renderActiveBracketBackground
//

void TextEditor::renderActiveBracketBackground() {
	if (config.showMatchingBrackets && bracketeer.size()) {
		auto drawList = ImGui::GetWindowDrawList();

		// render active bracket pair
		auto active = bracketeer.getEnclosingBrackets(cursors.getMain().getInteractiveEnd());

		if (active != bracketeer.end() && active->visible) {
			if (document[active->start.line].foldingState != FoldingState::hidden) {
				auto startVis = docPos2VisPos(active->start);

				if (startVis.row >= firstVisibleRow && startVis.row <= lastVisibleRow) {
					auto x = cursorScreenPos.x + textLeftOffset + startVis.column * glyphSize.x;
					auto y = cursorScreenPos.y + startVis.row * glyphSize.y;

					drawList->AddRectFilled(
						ImVec2(x, y),
						ImVec2(x + glyphSize.x, y + glyphSize.y),
						palette.get(Color::matchingBracketBackground));
				}
			}

			if (document[active->end.line].foldingState != FoldingState::hidden) {
				auto endVis = docPos2VisPos(active->end);

				if (endVis.row >= firstVisibleRow && endVis.row <= lastVisibleRow) {
					auto x = cursorScreenPos.x + textLeftOffset + endVis.column * glyphSize.x;
					auto y = cursorScreenPos.y + endVis.row * glyphSize.y;

					drawList->AddRectFilled(
						ImVec2(x, y),
						ImVec2(x + glyphSize.x, y + glyphSize.y),
						palette.get(Color::matchingBracketBackground));
				}
			}
		}
	}
}


//
//	TextEditor::renderSelections
//

void TextEditor::renderSelections() {
	auto drawList = ImGui::GetWindowDrawList();

	// draw background for selections
	for (auto& cursor : cursors) {
		if (cursor.hasSelection()) {
			auto begin = cursor.getSelectionStart();
			auto end = cursor.getSelectionEnd();

			for (size_t i = begin.line; i <= end.line; i++) {
				auto& line = document[i];

				if (line.foldingState != FoldingState::hidden) {
					if (line.rows == 1) {
						if (line.row >= firstVisibleRow && line.row <= lastVisibleRow) {
							auto lineLeft = DocPos(i, 0);
							auto lineRight =DocPos(i, line.size());
							auto docLeft = begin <= lineLeft ? lineLeft : begin;
							auto docRight = end > lineRight ? lineRight : end;
							auto visLeft = docPos2VisPos(docLeft);
							auto visRight = docPos2VisPos(docRight);

							auto x = cursorScreenPos.x + textLeftOffset;
							auto left = x + visLeft.column * glyphSize.x;
							auto right = x + visRight.column * glyphSize.x;
							auto y = cursorScreenPos.y + line.row * glyphSize.y;

							drawList->AddRectFilled(
								ImVec2(left, y),
								ImVec2(right, y + glyphSize.y),
								palette.get(Color::selection));
							}

					} else {
						for (size_t j = 0; j < line.rows; j++) {
							auto& section = line.sections->at(j);
							auto row = line.row + j;

							if (row >= firstVisibleRow && row <= lastVisibleRow) {
								auto sectionLeft = DocPos(i, section.startIndex);
								auto sectionRight = DocPos(i, section.endIndex);

								if (begin < sectionRight && end > sectionLeft) {
									auto visLeft = begin <= sectionLeft ? VisPos(row, 0) : docPos2VisPos(begin);
									auto visRight = end > sectionRight ? VisPos(row, section.columns) : docPos2VisPos(end);

									auto x = cursorScreenPos.x + textLeftOffset;
									auto left = x + visLeft.column * glyphSize.x;
									auto right = x + visRight.column * glyphSize.x;
									auto y = cursorScreenPos.y + row * glyphSize.y;

									drawList->AddRectFilled(
										ImVec2(left, y),
										ImVec2(right, y + glyphSize.y),
										palette.get(Color::selection));
								}
							}
						}
					}
				}
			}
		}
	}
}


//
//	TextEditor::renderMarkers
//

void TextEditor::renderMarkers() {
	if (markers.size()) {
		auto drawList = ImGui::GetWindowDrawList();

		for (size_t row = firstVisibleRow; row <= lastVisibleRow; row++) {
			auto markerIndex = document[typeSetter[row].line].marker;

			if (markerIndex) {
				auto& marker = markers[markerIndex - 1];
				auto y = cursorScreenPos.y + row * glyphSize.y;

				if (((marker.lineNumberColor >> IM_COL32_A_SHIFT) & 0xFF) != 0) {
					auto left = cursorScreenPos.x + lineNumberLeftOffset;
					auto right = cursorScreenPos.x + lineNumberRightOffset;
					auto start = ImVec2(left, y);
					auto end = ImVec2(right, y + glyphSize.y);
					drawList->AddRectFilled(start, end, marker.lineNumberColor);

					if (marker.lineNumberTooltip.size() && ImGui::IsMouseHoveringRect(start, end)) {
						ImGui::PushStyleColor(ImGuiCol_PopupBg, marker.lineNumberColor);
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(marker.lineNumberTooltip.c_str());
						ImGui::EndTooltip();
						ImGui::PopStyleColor();
					}
				}

				if (((marker.textColor >> IM_COL32_A_SHIFT) & 0xFF) != 0) {
					auto left = cursorScreenPos.x + textLeftOffset;
					auto right = left + lastVisibleColumn * glyphSize.x;
					auto start = ImVec2(left, y);
					auto end = ImVec2(right, y + glyphSize.y);
					drawList->AddRectFilled(start, end, marker.textColor);

					if (marker.textTooltip.size() && ImGui::IsMouseHoveringRect(start, end)) {
						ImGui::PushStyleColor(ImGuiCol_PopupBg, marker.textColor);
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(marker.textTooltip.c_str());
						ImGui::EndTooltip();
						ImGui::PopStyleColor();
					}
				}
			}
		}
	}
}


//
//	TextEditor::renderMatchingBracketLines
//

void TextEditor::renderMatchingBracketLines() {
	if (config.showMatchingBrackets && bracketeer.size()) {
		auto active = bracketeer.getEnclosingBrackets(cursors.getMain().getInteractiveEnd());
		auto drawList = ImGui::GetWindowDrawList();

		// render bracket pair lines
		for (auto bracket = bracketeer.begin(); bracket < bracketeer.end(); bracket++) {
			if (bracket->visible && bracket->end.line - bracket->start.line > 1) {
				auto column = std::min(docPos2VisPos(bracket->start).column, docPos2VisPos(bracket->end).column);

				for (size_t i = bracket->start.line + 1; i < bracket->end.line; i++) {
					auto& line = document[i];

					if (line.foldingState != FoldingState::hidden) {
						auto lineX = cursorScreenPos.x + textLeftOffset + column * glyphSize.x;
						auto startY = cursorScreenPos.y + line.row * glyphSize.y;
						auto endY = startY +  line.rows * glyphSize.y;

						auto color = palette.get(bracket == active ? Color::matchingBracketActive : Color::whitespace);
						drawList->AddLine(ImVec2(lineX, startY), ImVec2(lineX, endY), color);
					}
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
	ImVec2 rowScreenPos = cursorScreenPos + ImVec2(textLeftOffset, firstVisibleRow * glyphSize.y);
	auto firstRenderableColumn = (firstVisibleColumn / config.tabSize) * config.tabSize;

	// only process all visible rows
	for (size_t i = firstVisibleRow; i <= lastVisibleRow; i++) {
		// determine visible boundaries for this row
		auto& line = document[typeSetter[i].line];
		size_t index;
		size_t column;
		size_t endColumn;

		if (config.wordWrap && line.sections) {
			auto& section = line.sections->at(typeSetter[i].section);
			index = section.startIndex;
			column = section.indent;
			endColumn = section.columns;

		} else {
			index = 0;
			column = 0;
			endColumn = line.columns;
		}

		// only process all visible columns
		while (column < endColumn && column <= lastVisibleColumn) {
			auto& glyph = line[index++];
			auto codepoint = glyph.codepoint;
			ImVec2 glyphPos(rowScreenPos.x + column * glyphSize.x, rowScreenPos.y);

			// handle tabs
			if (codepoint == '\t') {
				if (config.showTabs && column >= firstRenderableColumn) {
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

				column += config.tabSize - (column % config.tabSize);

			// handle spaces
			} else if (codepoint == ' ') {
				if (config.showSpaces && column >= firstRenderableColumn) {
					const auto x = glyphPos.x + glyphSize.x * 0.5f;
					const auto y = glyphPos.y + fontSize * 0.5f;
					drawList->AddCircleFilled(ImVec2(x, y), 1.5f, palette.get(Color::whitespace), 4);
				}

				column++;

			// handle regular glyphs
			} else {
				if (column >= firstRenderableColumn) {
					font->RenderChar(drawList, fontSize, glyphPos, palette.get(glyph.color), codepoint);
				}

				column++;
			}
		}

		// draw ellipsis at the end of folded lines
		if (i == line.row + line.rows - 1) {
			if (line.foldingState == FoldingState::folded) {
				auto glyphPos = cursorScreenPos + ImVec2(textLeftOffset + typeSetter[i].columns * glyphSize.x, i * glyphSize.y);
				font->RenderChar(drawList, fontSize, glyphPos, palette.get(Color::text), font->EllipsisChar);
			}
		}

		rowScreenPos.y += glyphSize.y;
	}
}


//
//	TextEditor::renderCursors
//

void TextEditor::renderCursors() {
	// update cursor animation timer
	cursorAnimationTimer = std::fmod(cursorAnimationTimer + ImGui::GetIO().DeltaTime, 1.0f);

	if (ImGui::IsWindowFocused()) {
		if (!ImGui::GetIO().ConfigInputTextCursorBlink || cursorAnimationTimer < 0.5f) {
			auto drawList = ImGui::GetWindowDrawList();

			for (auto& cursor : cursors) {
				auto docPos = cursor.getInteractiveEnd();

				if (document[docPos.line].foldingState != FoldingState::hidden) {
					auto pos = docPos2VisPos(docPos);

					if (pos.row >= firstVisibleRow && pos.row <= lastVisibleRow) {
						auto x = cursorScreenPos.x + textLeftOffset + pos.column * glyphSize.x - 1;
						auto y = cursorScreenPos.y + pos.row * glyphSize.y;
						drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + cursorWidth, y + glyphSize.y), palette.get(Color::cursor));
					}
				}
			}
		}

		// notify OS of text input position for advanced Input Method Editor (IME)
		// this is required for the SDL3 backend as it will not report text input events unless we do this
		// see https://github.com/ocornut/imgui/issues/8584 for details
		if (!config.readOnly) {
			auto context = ImGui::GetCurrentContext();
			context->PlatformImeData.WantVisible = true;
			context->PlatformImeData.WantTextInput = true;
			context->PlatformImeData.InputPos = ImVec2(cursorScreenPos.x - 1.0f, cursorScreenPos.y - context->FontSize);
			context->PlatformImeData.InputLineHeight = context->FontSize;
			context->PlatformImeData.ViewportId = ImGui::GetCurrentWindow()->Viewport->ID;
		}
	}
}


//
//	TextEditor::renderLineNumbers
//

void TextEditor::renderLineNumbers() {
	if (config.showLineNumbers) {
		auto drawList = ImGui::GetWindowDrawList();
		auto curserRow = docPos2VisPos(cursors.getCurrent().getInteractiveEnd()).row;
		auto position = ImVec2(ImGui::GetWindowPos().x + lineNumberRightOffset, cursorScreenPos.y);

		for (size_t i = firstVisibleRow; i <= lastVisibleRow; i++) {
			if (typeSetter[i].section == 0) {
				auto lineNo = typeSetter[i].line + 1;
				auto width = static_cast<size_t>(std::log10(lineNo) + 1.0f) * glyphSize.x;
				auto foreground = (i == curserRow) ? Color::currentLineNumber : Color::lineNumber;
				auto number = std::to_string(lineNo);
				drawList->AddText(position + ImVec2(-width, i * glyphSize.y), palette.get(foreground), number.c_str());
			}
		}
	}
}


//
//	TextEditor::renderDecorations
//

void TextEditor::renderDecorations() {
	if (decoratorWidth != 0.0f && decoratorCallback) {
		auto position = ImVec2(ImGui::GetWindowPos().x + decorationOffset, cursorScreenPos.y + glyphSize.y * firstVisibleRow);
		auto widthInPixels = (decoratorWidth < 0.0f) ? -decoratorWidth * glyphSize.x: decoratorWidth;
		Decorator decorator{0, widthInPixels, glyphSize.y, glyphSize, nullptr};

		for (size_t i = firstVisibleRow; i <= lastVisibleRow; i++) {
			if (typeSetter[i].section == 0) {
				decorator.line = typeSetter[i].line;
				decorator.userData = document.getUserData(i);
				ImGui::SetCursorScreenPos(position);
				ImGui::PushID(static_cast<int>(i));
				decoratorCallback(decorator);
				ImGui::PopID();
				position.y += glyphSize.y;
			}
		}

		ImGui::SetCursorScreenPos(cursorScreenPos);
	}
}


//
//	TextEditor::renderFoldIndicators
//

void TextEditor::renderFoldIndicators() {
	if (config.lineFolding) {
		auto drawList = ImGui::GetWindowDrawList();
		auto color = palette.get(Color::lineNumber);
		auto hoveredColor = palette.get(Color::currentLineNumber);
		auto size = fontSize * 0.25f;

		for (size_t i = firstVisibleRow; i <= lastVisibleRow; i++) {
			auto& line = document[typeSetter[i].line];
			auto foldingState = line.foldingState;

			if (typeSetter[i].section == 0) {
				if (foldingState == FoldingState::foldable || foldingState == FoldingState::folded) {
					auto center = ImVec2(
						ImGui::GetWindowPos().x + foldIndicatorOffset,
						cursorScreenPos.y + glyphSize.y * i + fontSize / 2.0f);

					auto topLeft = center - ImVec2(glyphSize.x / 2.0f, fontSize / 2.0f);
					auto bottomRight = center + ImVec2(glyphSize.x / 2.0f, fontSize / 2.0f);
					auto isHovered = ImGui::IsMouseHoveringRect(topLeft, bottomRight);

					if (foldingState == FoldingState::foldable) {
						drawList->AddLine(
							ImVec2(center.x - size, center.y - size * 0.5f),
							ImVec2(center.x, center.y + size),
							isHovered ? hoveredColor : color);

						drawList->AddLine(
							ImVec2(center.x, center.y + size),
							ImVec2(center.x + size, center.y - size * 0.5f),
							isHovered ? hoveredColor : color);

					} else {
						drawList->AddLine(
							ImVec2(center.x - size * 0.5f, center.y - size),
							ImVec2(center.x + size, center.y),
							isHovered ? hoveredColor : color);

						drawList->AddLine(
							ImVec2(center.x + size, center.y),
							ImVec2(center.x - size * 0.5f, center.y + size),
							isHovered ? hoveredColor : color);
					}

					if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
						lineFold.toggleAtLine(document, typeSetter[i].line);
					}
				}
			}
		}

		ImGui::SetCursorScreenPos(cursorScreenPos);
	}
}


//
//	void TextEditor::renderMiniMap
//

void TextEditor::renderMiniMap() {
	if (config.showMiniMap) {
		// reset background colors
		for (auto& row : miniMap.rows) {
			row.color = 0;
		}

		// color cursor lines
		for (auto& cursor : cursors) {
			auto begin = cursor.getSelectionStart();
			auto end = cursor.getSelectionEnd();

			for (size_t i = begin.line; i <= end.line; i++) {
				auto& line = document[i];

				if (line.foldingState != FoldingState::hidden) {
					for (size_t j = 0; j < line.rows; j++) {
						miniMap.rows[line.row + j].color = miniMapPalette.get(Color::selection);
					}
				}
			}
		}

		// color marker lines
		if (markers.size()) {
			for (size_t row = 0; row < typeSetter.getRowCount(); row++) {
				auto& line = document[typeSetter[row].line];

				if (line.marker) {
					auto color = markers[line.marker - 1].textColor;

					if (!color) {
						color = markers[line.marker - 1].lineNumberColor;
					}

					auto fColor = ImGui::ColorConvertU32ToFloat4(color);
					fColor.w *= miniMapAlpha;
					miniMap.rows[row].color = ImGui::ColorConvertFloat4ToU32(fColor);
				}
			}
		}

		// determine viewport information
		auto totalMiniMapRows = miniMap.rows.size();
		auto visibleMiniMapRows = (textSize.y / miniMapRowHeight);

		if (totalMiniMapRows < visibleMiniMapRows) {
			firstMiniMapRow = 0;
			lastMiniMapRow = miniMap.rows.size();

		} else {
			auto scrollRatio = ImGui::GetScrollY() / ImGui::GetScrollMaxY();
			firstMiniMapRow = static_cast<size_t>(scrollRatio * (totalMiniMapRows - visibleMiniMapRows));
			lastMiniMapRow = firstMiniMapRow + static_cast<size_t>(std::ceil(visibleMiniMapRows));
			lastMiniMapRow = std::min(lastMiniMapRow, totalMiniMapRows);
		}

		// process all visible minimap rows
		auto drawList = ImGui::GetWindowDrawList();
		auto pos = ImGui::GetWindowPos() + ImVec2(miniMapOffset, 0.0f);

		for (size_t i = firstMiniMapRow; i < lastMiniMapRow; i++) {
			auto& row = miniMap.rows[i];

			// render line background
			if (row.color) {
				drawList->AddRectFilled(
					pos,
					pos + ImVec2(miniMapWidth, miniMapRowHeight),
					row.color);
			}

			// render text sections
			for (auto& section : row.sections) {
				drawList->AddRectFilled(
					pos + ImVec2(section.start * miniMapTextWidth, 0.0f),
					pos + ImVec2(section.end * miniMapTextWidth, miniMapTextHeight),
					miniMapPalette.get(section.color));
			}

			pos += ImVec2(0.0f, miniMapRowHeight);
		}

		// render viewport
		if (totalSize.y > textSize.y) {
			auto viewPortStart = (firstVisibleRow - firstMiniMapRow) * miniMapRowHeight;
			auto viewportHeight = (lastVisibleRow - firstVisibleRow) * miniMapRowHeight;
			auto ViewPortTopLeft = ImGui::GetWindowPos() + ImVec2(miniMapOffset, viewPortStart);
			auto viewPortBottomRight = ViewPortTopLeft + ImVec2(miniMapWidth, viewportHeight);

			auto fColor = ImGui::ColorConvertU32ToFloat4(palette.get(Color::text));
			fColor.w *= miniMapViewPortAlpha;
			drawList->AddRectFilled(ViewPortTopLeft, viewPortBottomRight, ImGui::ColorConvertFloat4ToU32(fColor));
		}
	}
}


//
//	TextEditor::renderScrollbarMiniMap
//

void TextEditor::renderScrollbarMiniMap() {
	if (config.showScrollbarMiniMap) {
		// based on https://github.com/ocornut/imgui/issues/3114
		auto window = ImGui::GetCurrentWindow();

		if (window->ScrollbarY) {
			auto drawList = ImGui::GetWindowDrawList();
			auto rect = ImGui::GetWindowScrollbarRect(window, ImGuiAxis_Y);
			auto rowHeight = rect.GetHeight() / static_cast<float>(typeSetter.getRowCount());
			auto offset = (rect.Max.x - rect.Min.x) * 0.3f;
			auto left = rect.Min.x + offset;
			auto right = rect.Max.x - offset;

			drawList->PushClipRect(rect.Min, rect.Max, false);

			// render cursor locations
			for (auto& cursor : cursors) {
				auto begin = cursor.getSelectionStart();
				auto end = cursor.getSelectionEnd();

				for (size_t i = begin.line; i <= end.line; i++) {
					auto& line = document[i];

					if (line.foldingState != FoldingState::hidden) {
						auto ly1 = std::round(rect.Min.y + line.row * rowHeight);
						auto ly2 = std::round(rect.Min.y + (line.row + line.rows) * rowHeight);

						drawList->AddRectFilled(
							ImVec2(left, ly1),
							ImVec2(right, ly2),
							palette.get(Color::selection));
					}
				}
			}

			// render marker locations
			if (markers.size()) {
				for (size_t row = 0; row < typeSetter.getRowCount(); row++) {
					auto& line = document[typeSetter[row].line];

					if (line.marker) {
						auto color = markers[line.marker - 1].textColor;

						if (!color) {
							color = markers[line.marker - 1].lineNumberColor;
						}

						auto ly = std::round(rect.Min.y + row * rowHeight);
						drawList->AddRectFilled(ImVec2(left, ly), ImVec2(right, ly + rowHeight), color);
					}
				}
			}

			drawList->PopClipRect();
		}
	}
}


//
//	TextEditor::renderPanScrollIndicator
//

void TextEditor::renderPanScrollIndicator() {
	if (config.showPanScrollIndicator && (panning || scrolling)) {
		auto drawList = ImGui::GetWindowDrawList();
		auto center = ImGui::GetWindowPos() + ImGui::GetWindowSize() / 2.0f;
		static constexpr int alpha = 160;
		drawList->AddCircleFilled(center, 20.0f, IM_COL32(255, 255, 255, alpha));
		drawList->AddCircle(center, 5.0f, IM_COL32(0, 0, 0, alpha), 0, 2.0f);

		drawList->AddTriangle(
			ImVec2(center.x - 15.0f, center.y),
			ImVec2(center.x - 8.0f, center.y - 4.0f),
			ImVec2(center.x - 8.0f, center.y + 4.0f),
			IM_COL32(0, 0, 0, alpha),
			2.0f);

		drawList->AddTriangle(
			ImVec2(center.x + 15.0f, center.y),
			ImVec2(center.x + 8.0f, center.y - 4.0f),
			ImVec2(center.x + 8.0f, center.y + 4.0f),
			IM_COL32(0, 0, 0, alpha),
			2.0f);

		drawList->AddTriangle(
			ImVec2(center.x, center.y - 15.0f),
			ImVec2(center.x - 4.0f, center.y - 8.0f),
			ImVec2(center.x + 4.0f, center.y - 8.0f),
			IM_COL32(0, 0, 0, alpha),
			2.0f);

		drawList->AddTriangle(
			ImVec2(center.x, center.y + 15.0f),
			ImVec2(center.x - 4.0f, center.y + 8.0f),
			ImVec2(center.x + 4.0f, center.y + 8.0f),
			IM_COL32(0, 0, 0, alpha),
			2.0f);
	}
}


//
//	TextEditor::renderPopups
//

void TextEditor::renderPopups() {
	PopupData popupData;
	popupData.pos = popupDocPos;
	popupData.userData = document.getUserData(popupDocPos.line);

	if (ImGui::BeginPopup("LineNumberContextMenu")) {
		if (lineNumberContextMenuCallback) {
			lineNumberContextMenuCallback(popupData);

		} else {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup("TextContextMenu")) {
		if (textContextMenuCallback) {
			textContextMenuCallback(popupData);

		} else {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::IsPopupOpen("TextHoverPopup")) {
		ImGui::SetNextWindowPos(popUpWindowPos, ImGuiCond_Always, ImVec2(0.0f, 1.0f));

		if (ImGui::BeginPopup("TextHoverPopup", ImGuiWindowFlags_NoFocusOnAppearing)) {
			if (textHoverCallback) {
				ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
				textHoverCallback(popupData);

			} else {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	// render autocomplete popup
	if (autocomplete.render(document, cursors, typeSetter, config.language, textLeftOffset, glyphSize)) {
		// user picked a suggestion so insert it
		auto start = autocomplete.getStart();
		auto end = document.findWordEnd(start, true);
		auto replacement = autocomplete.getReplacement();
		replaceSectionText(start, end, replacement);
	}
}


//
//	TextEditor::updateState
//

void TextEditor::updateState() {
	// this function gets called to handle possible changes caused by the API or user interactions
	// the overlays determine what they need to do to update their state (could be nothing)
	colorizer.update(config, document);
	bracketeer.update(config, document);
	lineFold.update(config, document, bracketeer);
	cursors.update(document);
	typeSetter.update(config, document, lineFold);
	miniMap.update(config, document, typeSetter);

	// handle possible delayed change callback
	if (document.isUpdated()) {
		if (delayedChangeCallback && !delayedChangeDetected) {
			delayedChangeDetected = true;
			delayedChangeReportTime = std::chrono::system_clock::now() + delayedChangeDelay;
		}
	}

	// reset overlay "dirty" flags
	document.resetUpdated();
	bracketeer.resetUpdated();
	lineFold.resetUpdated();
	typeSetter.resetUpdated();

	// get "new" total editor size in pixels
	auto width =
		textLeftOffset +
		typeSetter.getColumnCount() * glyphSize.x +
		cursorWidth +
		(config.showMiniMap ? miniMapWidth : 0.0f);

	auto height = typeSetter.getRowCount() * glyphSize.y;
	totalSize = ImVec2(width, height);
}


//
//	TextEditor::handleKeyboardInputs
//

void TextEditor::handleKeyboardInputs() {
	if (ImGui::IsWindowFocused()) {
		auto& io = ImGui::GetIO();
		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

		// get state of modifier keys
		// Dear ImGui switches the Cmd(Super) and Ctrl keys on MacOS
		auto shift = ImGui::IsKeyDown(ImGuiMod_Shift);
		auto ctrl = ImGui::IsKeyDown(ImGuiMod_Ctrl);
		auto alt = ImGui::IsKeyDown(ImGuiMod_Alt);
		auto super = ImGui::IsKeyDown(ImGuiMod_Super);
	    auto meta = ImGui::GetIO().ConfigMacOSXBehaviors ? alt : ctrl;

		auto isNoModifiers = !ctrl && !shift && !alt;
		auto isShortcut = ctrl && !shift && !alt;
		auto isShiftShortcut = ctrl && shift && !alt;
		auto isOptionalShiftShortcut = ctrl && !alt;
		auto isAltOnly = !ctrl && !shift && alt;
		auto isShiftOnly = !ctrl && shift && !alt;
		auto isOptionalShift = !ctrl && !alt;
		auto isOptionalAlt = !ctrl && !shift;
		auto isMetaShift = ImGui::GetIO().ConfigMacOSXBehaviors ? !ctrl && shift && !alt && super : !ctrl && shift && alt;
    	auto isOptionalMetaShift = ImGui::GetIO().ConfigMacOSXBehaviors ? !ctrl : !alt;

		// ignore specific keys when autocomplete is active, they will be handled later
		if (autocomplete.isActive() && autocomplete.isSpecialKeyPressed()) {
			if (autocomplete.hasSuggestions()) {
				return;

			} else {
				// cancel autocomplete when special keys are used without any suggestions
				autocomplete.cancel();
			}
		}

		// cursor movements and selections
		if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUp(1, shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDown(1, shift); }

		else if (isMetaShift && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { shrinkSelections(); }
		else if (isMetaShift && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { growSelections(); }
		else if (isOptionalMetaShift && ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) { moveLeft(shift, meta); }
		else if (isOptionalMetaShift && ImGui::IsKeyPressed(ImGuiKey_RightArrow)) { moveRight(shift, meta); }

		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_PageUp)) { moveUp(lastVisibleRow - firstVisibleRow - 2, shift); }
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_PageDown)) { moveDown(lastVisibleRow - firstVisibleRow - 2, shift); }
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
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_C)) { copy() ;}
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_Insert)) { copy(); }

		else if (!config.readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_V)) { paste(); }
		else if (!config.readOnly && isShiftOnly && ImGui::IsKeyPressed(ImGuiKey_Insert)) { paste(); }
		else if (!config.readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Z)) { undo(); }
		else if (!config.readOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Z)) { redo(); }
		else if (!config.readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Y)) { redo(); }

		// remove text
		else if (!config.readOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Delete)) { handleDelete(alt); }
		else if (!config.readOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Backspace)) { handleBackspace(alt); }
		else if (!config.readOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_K)) { removeSelectedLines(); }

		// text manipulation
		else if (!config.readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_LeftBracket)) { deindentLines(); }
		else if (!config.readOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_RightBracket)) { indentLines(); }
		else if (!config.readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_UpArrow)) { moveUpLines(); }
		else if (!config.readOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_DownArrow)) { moveDownLines(); }
		else if (!config.readOnly && config.language && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Slash)) { toggleComments(); }

		// find/replace support
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_F)) {
			if (autocomplete.isActive()) {
				autocomplete.cancel();
				findCancelledAutocomplete = true;
			}

			openFindReplace();
		}

		else if (isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_F)) { findAll(); }
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_G)) { findNext(); }

		// autocomplete support
		else if (!config.readOnly && ImGui::IsKeyChordPressed(autocomplete.getTriggerShortcut())) {
			// don't activate if we have multiple cursors active
			if (cursors.hasMultiple()) {
				// TODO: inform user

			} else {
				if (autocomplete.startShortcut(cursors)) {
					makeCursorVisible();
				}
			}
		}

		// change insert mode
		else if (isNoModifiers && ImGui::IsKeyPressed(ImGuiKey_Insert)) { config.overwrite = !config.overwrite; }

		// handle new line
		else if (!config.readOnly && isNoModifiers && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { handleCharacter('\n'); }
		else if (!config.readOnly && isShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineBelow(); }
		else if (!config.readOnly && isShiftShortcut && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter))) { insertLineAbove(); }

		// handle tabs
		else if (!config.readOnly && isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_Tab)) {
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

		// handle escape key
		else if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
			if (autocomplete.isActive()) {
				autocomplete.cancel();

			} else if (findReplaceVisible) {
				closeFindReplace();

			} else if (cursors.hasMultiple()) {
				cursors.clearAdditional();
			}
		}

		// handle regular text
		if (!io.InputQueueCharacters.empty()) {
			// ignore Ctrl inputs, but need to allow Alt+Ctrl as some keyboards (e.g. German) use AltGR (which is Alt+Ctrl) to input certain characters
			if (!(ctrl && !alt) && !config.readOnly) {
				for (auto i = 0; i < io.InputQueueCharacters.size(); i++) {
					auto character = io.InputQueueCharacters[i];

					if (character == '\n' || character >= 32) {
						handleCharacter(character);
					}
				}
			}

			io.InputQueueCharacters.resize(0);
		}
	}
}


//
//	TextEditor::handleMouseInteractions
//

void TextEditor::handleMouseInteractions() {
	// handle middle mouse button modes
	panning &= config.panMode && ImGui::IsMouseDown(ImGuiMouseButton_Middle);
	auto absoluteMousePos = ImGui::GetMousePos() - ImGui::GetWindowPos();

	if (panning && ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
		// handle middle mouse button panning
		auto windowSize = ImGui::GetWindowSize();
		auto mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
		float dragFactor = ImGui::GetIO().DeltaTime * 15.0f;
		ImVec2 autoPanMargin(glyphSize.x * 4.0f, glyphSize.y * 2.0f);

		if (absoluteMousePos.x < textLeftOffset + autoPanMargin.x) {
			mouseDelta.x = (absoluteMousePos.x - (textLeftOffset + autoPanMargin.x)) * dragFactor;

		} else if (absoluteMousePos.x > windowSize.x - autoPanMargin.x) {
			mouseDelta.x = (absoluteMousePos.x - (windowSize.x - autoPanMargin.x)) * dragFactor;
		}

		if (absoluteMousePos.y < autoPanMargin.y) {
			mouseDelta.y = (absoluteMousePos.y - autoPanMargin.y) * dragFactor;

		} else if (absoluteMousePos.y > windowSize.y - autoPanMargin.y) {
			mouseDelta.y = (absoluteMousePos.y - (windowSize.y - autoPanMargin.y)) * dragFactor;
		}

		ImGui::SetScrollX(ImGui::GetScrollX() - mouseDelta.x);
		ImGui::SetScrollY(ImGui::GetScrollY() - mouseDelta.y);
		ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);

	} else if (scrolling) {
		// handle middle mouse button scrolling
		float deadzone = glyphSize.x;
		auto offset = scrollStart - absoluteMousePos;
		offset.x = (offset.x < 0.0f) ? std::min(offset.x + deadzone, 0.0f) : std::max(offset.x - deadzone, 0.0f);
		offset.y = (offset.y < 0.0f) ? std::min(offset.y + deadzone, 0.0f) : std::max(offset.y - deadzone, 0.0f);

		float scrollFactor = ImGui::GetIO().DeltaTime * 5.0f;
		offset *= scrollFactor;

		ImGui::SetScrollX(ImGui::GetScrollX() - offset.x);
		ImGui::SetScrollY(ImGui::GetScrollY() - offset.y);

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
			ImGui::IsMouseClicked(ImGuiMouseButton_Middle) ||
			ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {

			scrolling = false;
		}

	// ignore other interactions when the editor is not hovered
	} else if (ImGui::IsWindowHovered()) {
		auto io = ImGui::GetIO();
		auto mousePos = ImGui::GetMousePos() - cursorScreenPos;
		auto overLineNumbers = config.showLineNumbers && (absoluteMousePos.x > lineNumberLeftOffset) && (absoluteMousePos.x < lineNumberRightOffset);
		auto overText = mousePos.x - ImGui::GetScrollX() > textLeftOffset && mousePos.x - ImGui::GetScrollX() < textRightOffset;
		auto overMiniMap = config.showMiniMap && absoluteMousePos.x > miniMapOffset;

		DocPos glyphPos;
		DocPos cursorPos;

		typeSetter.screenPos2DocPos(
			document,
			ImVec2((mousePos.x - textLeftOffset) / glyphSize.x, mousePos.y / glyphSize.y),
			glyphPos,
			cursorPos);

		// show text cursor if required
		if (ImGui::IsWindowFocused() && overText) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			// update selection with dragging left mouse button
			io.WantCaptureMouse = true;

			if (overLineNumbers) {
				auto& cursor = cursors.getCurrent();
				auto start = DocPos(cursorPos.line, 0);
				auto end = normalizePos(DocPos(cursorPos.line + 1, 0));
				cursor.update(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);

			} else if (overText) {
				cursors.updateCurrentCursor(cursorPos);
			}

			makeCursorVisible();

		} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
			// start panning/scrolling mode on middle mouse click
			if (config.panMode) {
				panning = true;

			} else {
				scrolling = true;
				scrollStart = absoluteMousePos;
			}

		} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
			// handle right clicks by setting up context menu (if required)
			if (overLineNumbers && lineNumberContextMenuCallback) {
				popupDocPos = glyphPos;
				ImGui::OpenPopup("LineNumberContextMenu");

			} else if (overText && textContextMenuCallback) {
				popupDocPos = glyphPos;
				ImGui::OpenPopup("TextContextMenu");
			}

		} else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			// handle left mouse button actions
			auto doubleClick = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
			auto now = static_cast<float>(ImGui::GetTime());
			auto tripleClick = !doubleClick && lastClickTime != -1.0f && (now - lastClickTime) < io.MouseDoubleClickTime;
			lastClickTime = tripleClick ? -1.0f : now;

			if (tripleClick) {
				// left mouse button triple click
				if (overText) {
					auto start = document.getStartOfLine(cursorPos);
					auto end = normalizePos(DocPos(start.line + 1, 0));
					cursors.updateCurrentCursor(start, end);
				}

			} else if (doubleClick) {
				// left mouse button double click
				if (overText) {
					auto codepoint = document.getCodePoint(glyphPos);
					bool handled = false;

					// select bracketed section (if required)
					if (CodePoint::isBracketOpener(codepoint)) {
						auto brackets = bracketeer.getEnclosingBrackets(document.getRight(glyphPos));

						if (brackets != bracketeer.end()) {
							if (ImGui::IsKeyDown(ImGuiMod_Shift)) {
								cursors.setCursor(brackets->start, document.getRight(brackets->end));

							} else {
								cursors.setCursor(document.getRight(brackets->start), brackets->end);
							}

							handled = true;
						}

					} else if (CodePoint::isBracketCloser(codepoint)) {
						auto brackets = bracketeer.getEnclosingBrackets(glyphPos);

						if (brackets != bracketeer.end()) {
							cursors.setCursor(brackets->start, document.getRight(brackets->end));
							handled = true;
						}
					}

					// select "word" if it wasn't a bracketed section
					// includes whitespace and operator sequences as well
					if (!handled && !document.isEndOfLine(glyphPos)) {
						auto start = document.findWordStart(glyphPos);
						auto end = document.findWordEnd(glyphPos);
						cursors.updateCurrentCursor(start, end);
					}
				}

			} else {
				// left mouse button single click
				auto extendCursor = ImGui::IsKeyDown(ImGuiMod_Shift);

				auto addCursor = ImGui::GetIO().ConfigMacOSXBehaviors
					? ImGui::IsKeyDown(ImGuiMod_Alt) :
					ImGui::IsKeyDown(ImGuiMod_Ctrl);

				if (overLineNumbers) {
					// handle line number clicks
					auto start = DocPos(cursorPos.line, 0);
					auto end = normalizePos(DocPos(cursorPos.line + 1, 0));

					if (extendCursor) {
						auto& cursor = cursors.getCurrent();
						cursor.update(cursor.getInteractiveEnd() < cursor.getInteractiveStart() ? start : end);
						autocomplete.cancel();

					} else if (addCursor) {
						cursors.addCursor(start, end);
						autocomplete.cancel();

					} else {
						cursors.setCursor(start, end);
					}

					makeCursorVisible();

				} else if (overText) {
					// handle mouse clicks in text
					if (extendCursor) {
						cursors.growCurrentCursor(cursorPos);
						autocomplete.cancel();

					} else if (addCursor) {
						cursors.addCursor(cursorPos);
						autocomplete.cancel();

					} else {
						cursors.setCursor(cursorPos);
					}

					makeCursorVisible();

				} else if (overMiniMap) {
					moveTo(visPos2DocPos(VisPos(
						firstMiniMapRow + static_cast<size_t>(absoluteMousePos.y / miniMapRowHeight),
						static_cast<size_t>((absoluteMousePos.x - miniMapOffset) / miniMapTextWidth))), false);
				}
			}

		} else if (textHoverCallback && IsMousePosOverGlyph(ImGui::GetMousePos())) {
			popupDocPos = document.findWordStart(glyphPos, true);
			auto vizPos = docPos2VisPos(popupDocPos);

			popUpWindowPos = ImVec2(
				vizPos.column * glyphSize.x + textLeftOffset + cursorScreenPos.x,
				vizPos.row * glyphSize.y + cursorScreenPos.y);

			if (!ImGui::IsPopupOpen("TextHoverPopup")) {
				ImGui::OpenPopup("TextHoverPopup");
			}
		}

		// update cursors and reset vertical navigation if any button was pressed
		bool anyMouseDown = false;

		for (size_t i = 0; !anyMouseDown && i < static_cast<size_t>(ImGuiMouseButton_Middle); i++) {
			if (ImGui::IsMouseDown(static_cast<ImGuiMouseButton>(i))) {
				anyMouseDown = true;
			}
		}

		if (anyMouseDown) {
			cursors.update(document);
			navigatingVertically = false;
		}
	}
}


//
//	TextEditor::isDocPosVisible
//

bool TextEditor::isDocPosVisible(DocPos docPos) const {
	if (isLineHidden(docPos.line)) {
		return false;

	} else {
		auto visPos = docPos2VisPos(docPos);

		return
			visPos.row >= firstVisibleRow && visPos.row < lastVisibleRow &&
			visPos.column >= firstVisibleColumn && visPos.column < lastVisibleColumn;
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

void TextEditor::selectLine(size_t line) {
	moveTo(DocPos(line, 0), false);
	moveTo(normalizePos(DocPos(line + 1, 0)), true);
}


//
//	TextEditor::selectLines
//

void TextEditor::selectLines(size_t startLine, size_t endLine) {
	moveTo(DocPos(startLine, 0), false);
	moveTo(DocPos(endLine, 0), true);
}


//
//	TextEditor::selectRegion
//

void TextEditor::selectRegion(DocPos start, DocPos end) {
	if (end < start) {
		std::swap(start, end);
	}

	cursors.setCursor(start, end);
}


//
//	TextEditor::selectToBrackets
//

void TextEditor::selectToBrackets(bool includeBrackets) {
	for (auto& cursor : cursors) {
		auto bracket = bracketeer.getEnclosingBrackets(cursor.getSelectionStart());

		if (bracket != bracketeer.end()) {
			if (includeBrackets) {
				cursor.update(bracket->start, document.getRight(bracket->end));

			} else {
				cursor.update(document.getRight(bracket->start), bracket->end);
			}
		}
	}
}


//
//	TextEditor::growSelections
//

void TextEditor::growSelections() {
	for (auto& cursor : cursors) {
		if (cursor.hasSelection()) {
			auto start = cursor.getSelectionStart();
			auto end = cursor.getSelectionEnd();
			auto startCodePoint = document.getCodePoint(document.getLeft(start));
			auto endCodePoint = document.getCodePoint(end);

			if (CodePoint::isBracketOpener(startCodePoint) && endCodePoint == CodePoint::toPairCloser(startCodePoint)) {
				cursor.update(document.getLeft(start),document.getRight(end));

			} else {
				auto bracket = bracketeer.getEnclosingBrackets(start, end);

				if (bracket != bracketeer.end()) {
					cursor.update(document.getRight(bracket->start), bracket->end);
				}
			}

		} else {
			auto pos = cursor.getSelectionEnd();
			auto start = document.findWordStart(pos);
			auto end = document.findWordEnd(pos);
			cursor.update(start, end);
		}
	}
}


//
//	TextEditor::shrinkSelections
//

void TextEditor::shrinkSelections() {
	for (auto& cursor : cursors) {
		if (cursor.hasSelection()){
			auto start = cursor.getSelectionStart();
			auto end = cursor.getSelectionEnd();
			auto startCodePoint = document.getCodePoint(start);
			auto endCodePoint = document.getCodePoint(document.getLeft(end));

			if (CodePoint::isBracketOpener(startCodePoint) && endCodePoint == CodePoint::toPairCloser(startCodePoint)) {
				cursor.update(document.getRight(start),document.getLeft(end));

			} else {
				auto bracket = bracketeer.getInnerBrackets(start, end);

				if (bracket != bracketeer.end()) {
					cursor.update(bracket->start, document.getRight(bracket->end));
				}
			}
		}
	}
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
	auto clipboard = ImGui::GetClipboardText();

	if (clipboard) {
		auto transaction = startTransaction();
		insertTextIntoAllCursors(transaction, clipboard);
		endTransaction(transaction);
	}
}


//
//	TextEditor::undo
//

void TextEditor::undo() {
	if (transactions.canUndo()) {
		transactions.undo(config, document, cursors);
		makeCursorVisible();
	}
}


//
//	TextEditor::redo
//

void TextEditor::redo() {
	if (transactions.canRedo()) {
		transactions.redo(config, document, cursors);
		makeCursorVisible();
	}
}


//
//	TextEditor::getCursorPosition
//

TextEditor::DocPos TextEditor::getCursorPosition(size_t cursor) const {
	cursor = std::min(cursor, cursors.size() - 1);
	return cursors[cursor].getInteractiveEnd();
}


//
//	TextEditor::getCursor
//

TextEditor::DocSelection TextEditor::getCursorSelection(size_t cursor) const {
	cursor = std::min(cursor, cursors.size() - 1);
	auto start = cursors[cursor].getSelectionStart();
	auto end = cursors[cursor].getSelectionEnd();
	return DocSelection(start, end);
}


//
//	TextEditor::isMousePosOverGlyph
//

bool TextEditor::isMousePosOverGlyph(const ImVec2& mousePos) const {
	// convert mouse position to screen coordinates
	auto local = mousePos - cursorScreenPos;

	// ignore negative coordinates
	if (local.x < 0.0f || local.y < 0.0f) {
		return false;
	}

	// convert to visual position and check it
	VisPos visPos(static_cast<size_t>(local.y / glyphSize.y), static_cast<size_t>((local.x - textLeftOffset) / glyphSize.x));
	return typeSetter.isVisPosOverGlyph(visPos);
}


//
//	TextEditor::getDocPosAtMousePos
//

TextEditor::DocPos TextEditor::getDocPosAtMousePos(const ImVec2& mousePos) const {
	// convert mouse position to screen coordinates
	auto local = mousePos - cursorScreenPos;

	// ignore negative coordinates
	if (local.y < 0.0f) {
		return DocPos(0, 0);

	} else if (local.x < 0.0f) {
		local.x = 0.0f;
	}

	// convert to document position
	VisPos visPos(static_cast<size_t>(local.y / glyphSize.y), static_cast<size_t>((local.x - textLeftOffset) / glyphSize.x));
	return visPos2DocPos(normalizePos(visPos));
}


//
//	TextEditor::getWordAtMousePos
//

std::string TextEditor::getWordAtMousePos(const ImVec2& mousePos) const {
	if (IsMousePosOverGlyph(mousePos)) {
		// convert to document position
		DocPos docPos = GetDocPosAtMousePos(mousePos);

		// Find word boundaries and extract text
		auto start = document.findWordStart(docPos, true);
		auto end = document.findWordEnd(start, true);
		return document.getSectionText(start, end);

	} else {
		return "";
	}
}


//
//	TextEditor::setCursor
//

void TextEditor::setCursor(DocPos pos) {
	ensureVisiblePos = normalizePos(pos);
	cursors.setCursor(ensureVisiblePos);

	if (config.lineFolding) {
		lineFold.unfoldAroundLine(document, ensureVisiblePos.line);
	}
}


//
//	TextEditor::scrollToLine
//

void TextEditor::scrollToLine(size_t line, Scroll alignment) {
	ensureVisiblePos = DocPos(invalidLine, 0);
	scrollToLineNumber = std::min(line, document.size());
	scrollToAlignment = alignment;

	if (config.lineFolding) {
		lineFold.unfoldAroundLine(document, line);
	}
}


//
//	TextEditor::handlePossibleScrolling
//

void TextEditor::handlePossibleScrolling() {
	// apply scrolling (if needed)
	float scrollX = -1.0f;
	float scrollY = -1.0f;

	// do we need to make a certain position visible
	if (ensureVisiblePos.line != invalidLine) {
		auto pos = docPos2VisPos(ensureVisiblePos);

		if (pos.row <= firstVisibleRow + 1) {
			scrollY = std::max(0.0f, (pos.row - 2.0f) * glyphSize.y);

		} else if (pos.row >= lastVisibleRow - 1) {
			scrollY = std::max(0.0f, (pos.row + 2.0f) * glyphSize.y - textSize.y);
		}

		if (pos.column <= firstVisibleColumn + 1) {
			scrollX = std::max(0.0f, (pos.column - 2.0f) * glyphSize.x);

		} else if (pos.column >= lastVisibleColumn - 1) {
			scrollX = std::max(0.0f, (pos.column + 2.0f) * glyphSize.x - textSize.x);
		}

		ensureVisiblePos.line = invalidLine;
	}

	// scroll to specified line (if required)
	if (scrollToLineNumber != invalidLine) {
		scrollToLineNumber = std::min(scrollToLineNumber, document.size());
		auto row = docPos2VisPos(DocPos(scrollToLineNumber, 0)).row;
		auto visibleRows = lastVisibleRow - firstVisibleRow;
		scrollX = 0.0f;

		switch (scrollToAlignment) {
			case Scroll::alignTop:
				scrollY = static_cast<float>(row * glyphSize.y);
				break;

			case Scroll::alignMiddle:
				scrollY = std::max(0.0f, static_cast<float>(scrollToLineNumber - visibleRows / 2) * glyphSize.y);
				break;

			case Scroll::alignBottom:
				scrollY = std::max(0.0f, static_cast<float>(scrollToLineNumber - (visibleRows - 1)) * glyphSize.y);
				break;
		}

		scrollToLineNumber = invalidLine;
	}

	// set scroll (if required)
	if (scrollX >= 0.0f) {
		ImGui::SetScrollX(scrollX);
	}

	if (scrollY >= 0.0f) {
		ImGui::SetScrollY(scrollY);
	}
}


//
//	TextEditor::makeCursorVisible
//

void TextEditor::makeCursorVisible() {
	ensureVisiblePos = cursors.getCurrent().getInteractiveEnd();
	scrollToLineNumber = invalidLine;

	if (config.lineFolding) {
		lineFold.unfoldAroundLine(document, cursors.getCurrent().getInteractiveEnd().line);
	}
}


//
//	TextEditor::addMarker
//

void TextEditor::addMarker(size_t line, ImU32 lineNumberColor, ImU32 textColor, const std::string_view& lineNumberTooltip, const std::string_view& textTooltip) {
	if (line < document.size()) {
		markers.emplace_back(lineNumberColor, textColor, lineNumberTooltip, textTooltip);
		document[line].marker = markers.size();
	}
}


//
//	TextEditor::clearMarkers
//

void TextEditor::clearMarkers() {
	for (auto& line : document) {
		line.marker = 0;
	}

	markers.clear();
}


//
//	TextEditor::moveUp
//

void TextEditor::moveUp(size_t rows, bool select) {
	// these must be done with visual coordinates
	for (auto& cursor : cursors) {
		auto visPos = docPos2VisPos(cursor.getInteractiveEnd());

		if (navigatingVertically) {
			visPos.column = cursor.getPreferredColumn();

		} else {
			cursor.setPreferredColumn(visPos.column);
		}

		if (rows > visPos.row) {
			visPos.row = 0;

		} else {
			visPos.row -= rows;
		}

		visPos = normalizePos(visPos);
		cursor.update(visPos2DocPos(visPos), select);
	}

	makeCursorVisible();
	navigatingVertically = true;
}


//
//	TextEditor::moveDown
//

void TextEditor::moveDown(size_t rows, bool select) {
	// these must be done with visual coordinates
	for (auto& cursor : cursors) {
		auto visPos = docPos2VisPos(cursor.getInteractiveEnd());

		if (navigatingVertically) {
			visPos.column = cursor.getPreferredColumn();

		} else {
			cursor.setPreferredColumn(visPos.column);
		}

		visPos.row += rows;
		visPos = normalizePos(visPos);
		cursor.update(visPos2DocPos(visPos), select);
	}

	makeCursorVisible();
	navigatingVertically = true;
}


//
//	TextEditor::moveLeft
//

void TextEditor::moveLeft(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		if (cursor.hasSelection() && !select && !wordMode) {
			cursor.resetToStart();

		} else {
			cursor.update(document.getLeft(cursor.getInteractiveEnd(), wordMode), select);
		}
	}

	makeCursorVisible();
	navigatingVertically = false;
}


//
//	TextEditor::moveRight
//

void TextEditor::moveRight(bool select, bool wordMode) {
	for (auto& cursor : cursors) {
		if (cursor.hasSelection() && !select && !wordMode) {
			cursor.resetToEnd();

		} else {
			cursor.update(document.getRight(cursor.getInteractiveEnd(), wordMode), select);
		}
	}

	makeCursorVisible();
	navigatingVertically = false;
}


//
//	TextEditor::moveToTop
//

void TextEditor::moveToTop(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getTop(), select);
	makeCursorVisible();
	navigatingVertically = false;
}


//
//	TextEditor::moveToBottom
//

void TextEditor::moveToBottom(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getBottom(), select);
	makeCursorVisible();
	navigatingVertically = false;
}


//
//	TextEditor::moveToStartOfLine
//

void TextEditor::moveToStartOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getStartOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	makeCursorVisible();
	navigatingVertically = false;
}


//
//	TextEditor::moveToEndOfLine
//

void TextEditor::moveToEndOfLine(bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(document.getEndOfLine(cursors.getCurrent().getInteractiveEnd()), select);
	makeCursorVisible();
	navigatingVertically = false;
}


//
//	TextEditor::moveTo
//

void TextEditor::moveTo(DocPos coordinate, bool select) {
	cursors.clearAdditional();
	cursors.updateCurrentCursor(coordinate, select);

	if (config.lineFolding) {
		lineFold.unfoldAroundLine(document, coordinate.line);
	}

	makeCursorVisible();
	navigatingVertically = false;
}


//
//	TextEditor::handleCharacter
//

void TextEditor::handleCharacter(ImWchar character) {
	auto transaction = startTransaction(false);

	auto opener = character;
	auto isPaired = !config.overwrite && config.completePairedGlyphs && CodePoint::isPairOpener(opener);
	auto closer = CodePoint::toPairCloser(opener);

	// ignore input if it was the closing character for a pair that was automatically inserted
	if (completePairCloser) {
		if (completePairCloser == character && completePairLocation == cursors.getCurrent().getSelectionEnd()) {
			completePairCloser = 0;
			moveRight(false, false);
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

				// insert the closing glyph
				char utf8[4];
				auto end1 = insertText(transaction, end, std::string_view(utf8, CodePoint::write(utf8, closer)));
				cursors.adjustForInsert(cursor, start, end1);

				// insert the opening glyph
				auto end2 = insertText(transaction, start, std::string_view(utf8, CodePoint::write(utf8, opener)));
				cursors.adjustForInsert(cursor, start, end2);

				// update old selection
				cursor->update(document.getRight(start), document.getRight(end));
			}
		}

	} else if (isPaired) {
		// insert the requested pair
		char utf8[8];
		auto size = CodePoint::write(utf8, opener);
		size += CodePoint::write(utf8 + size, closer);
		std::string_view pair(utf8, size);

		for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
			auto start = cursor->getSelectionStart();
			auto end = insertText(transaction, start, pair);
			cursors.adjustForInsert(cursor, start, end);
			cursor->update(document.getRight(start), false);
		}

		// remember the closer
		completePairCloser = closer;
		completePairLocation = cursors.getCurrent().getSelectionEnd();

	} else if (!config.overwrite && config.autoIndent && character == '\n') {
		// handle auto indent case
		autoIndentAllCursors(transaction);

	} else {
		// handle overwrite by deleting next glyph before insert
		if (config.overwrite) {
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
		char utf8[4];
		insertTextIntoAllCursors(transaction, std::string_view(utf8, CodePoint::write(utf8, character)));
	}

	endTransaction(transaction);

	if (CodePoint::isWord(character)) {
		if (!cursors.hasMultiple() && autocomplete.startTyping(cursors)) {
			makeCursorVisible();
		}
	}
}


//
//	TextEditor::handleBackspace
//

void TextEditor::handleBackspace(bool wordMode) {
	auto transaction = startTransaction(false);

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
	auto transaction = startTransaction(false);

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
		end = (end.index == 0) ? end : document.getNextLine(end);
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
		start = (start.index == 0) ? start : document.getNextLine(start);
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
		auto cursorEnd = cursor->getSelectionEnd();

		// process all lines in this cursor
		for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
			if (DocPos(line, 0) != cursorEnd && document[line].size()) {
				auto insertStart = DocPos(line, 0);
				auto insertEnd = insertText(transaction, insertStart, "\t");
				cursors.adjustForInsert(cursor, insertStart, insertEnd);
			}
		}

		cursorStart.index += cursorStart.index ? 1 : 0;
		cursorEnd.index += cursorEnd.index ? 1 : 0;
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
		auto cursorEnd = cursor->getSelectionEnd();

		for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
			// determine how many whitespaces are available at the start with a max of tabSize columns
			size_t column = 0;
			size_t index = 0;

			while (column < config.tabSize && index < document[line].size() && std::isblank(document[line][index].codepoint)) {
				column += document[line][index].codepoint == '\t' ? config.tabSize - (column % config.tabSize) : 1;
				index++;
			}

			// delete that whitespace (if required)
			DocPos deleteStart(line, 0);
			DocPos deleteEnd(line, index);

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
			auto deleteEnd = (end.index == 0) ? end : document.getNextLine(end);
			auto text = document.getSectionText(deleteStart, deleteEnd);
			deleteText(transaction, deleteStart, deleteEnd);
			cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

			// insert text one line up
			DocPos insertStart(deleteStart.line - 1, 0);
			auto insertEnd = insertText(transaction, insertStart, text);
			cursors.adjustForInsert(cursor, insertStart, insertEnd);

			// update cursor
			cursor->update(start - DocPos(1, 0), end - DocPos(1, 0));
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
			auto deleteEnd = (end.index == 0) ? end : document.getNextLine(end);
			auto text = document.getSectionText(deleteStart, deleteEnd);
			deleteText(transaction, deleteStart, deleteEnd);
			cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

			// insert text one line down
			auto insertStart = document.getNextLine(deleteStart);
			auto insertEnd = insertText(transaction, insertStart, text);
			cursors.adjustForInsert(cursor, insertStart, insertEnd);

			// update cursor
			cursor->update(start + DocPos(1, 0), end + DocPos(1, 0));
		}

		endTransaction(transaction);
	}
}


//
//	TextEditor::toggleComments
//

void TextEditor::toggleComments() {
	auto transaction = startTransaction();
	auto comment = config.language->singleLineComment;

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto cursorStart = cursor->getSelectionStart();
		auto cursorEnd = cursor->getSelectionEnd();

		// process all lines in this cursor
		for (auto line = cursorStart.line; line <= cursorEnd.line; line++) {
			if (DocPos(line, 0) != cursorEnd && document[line].size()) {
				// see if line starts with a comment (after possible leading whitespaces)
				size_t start = 0;
				size_t i = 0;

				while (start < document[line].size() && CodePoint::isWhiteSpace(document[line][start].codepoint)) {
					start++;
				}

				while (start + i < document[line].size() && i < comment.size() && document[line][start + i].codepoint == comment[i]) {
					i++;
				}

				if (i == comment.size()) {
					auto deleteStart = DocPos(line, start);
					auto deleteEnd = DocPos(line, start + comment.size() + 1);
					deleteText(transaction, deleteStart, deleteEnd);
					cursors.adjustForDelete(cursor, deleteStart, deleteEnd);

				} else {
					auto insertStart = DocPos(line, start);
					auto insertEnd = insertText(transaction, insertStart, comment + " ");
					cursors.adjustForInsert(cursor, insertStart, insertEnd);
				}
			}
		}
	}

	endTransaction(transaction);
}


//
//	TextEditor::filterSelections
//

void TextEditor::filterSelections(std::function<std::string(std::string_view)> filter) {
	auto transaction = startTransaction();

	// process all cursors
	for (auto cursor = cursors.begin(); cursor < cursors.end(); cursor++) {
		auto start = cursor->getSelectionStart();
		auto end = cursor->getSelectionEnd();

		// process all lines in this cursor
		for (auto line = start.line; line <= end.line; line++) {
			if (DocPos(line, 0) != end && document[line].size()) {
				// get original text and run it through filter
				auto before = document.getSectionText(start, end);
				std::string after = filter(before);

				// update selection if anything changed
				if (after != before) {
					deleteText(transaction, start, end);
					cursors.adjustForDelete(cursor, start, end);
					auto newEnd = insertText(transaction, start, after);
					cursor->update(start, newEnd);
					cursors.adjustForInsert(cursor, start, newEnd);
				}
			}
		}
	}

	endTransaction(transaction);
}


//
//	TextEditor::selectionToLowerCase
//

void TextEditor::selectionToLowerCase() {
	FilterSelections([](const std::string_view& text) {
		std::string result;
		auto end = text.end();
		auto i = text.begin();
		char utf8[4];

		while (i < end) {
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);
			result.append(utf8, CodePoint::write(utf8, CodePoint::toLower(codepoint)));
		}

		return result;
	});
}


//
//	TextEditor::selectionToUpperCase
//

void TextEditor::selectionToUpperCase() {
	FilterSelections([](const std::string_view& text) {
		std::string result;
		auto end = text.end();
		auto i = text.begin();
		char utf8[4];

		while (i < end) {
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);
			result.append(utf8, CodePoint::write(utf8, CodePoint::toUpper(codepoint)));
		}

		return result;
	});
}


//
//	TextEditor::stripTrailingWhitespaces
//

void TextEditor::stripTrailingWhitespaces() {
	auto transaction = startTransaction();

	// process all the lines
	for (size_t i = 0; i < document.size(); i++) {
		auto& line = document[i];
		size_t lineSize = line.size();
		size_t whitespace = std::numeric_limits<std::size_t>::max();
		bool done = false;

		// look for first non-whitespace glyph at the end of the line
		if (lineSize) {
			for (auto index = lineSize - 1; !done; index--) {
				if (CodePoint::isWhiteSpace(line[index].codepoint)) {
					whitespace = index;

					if (index == 0) {
						done = true;
					}

				} else {
					done = true;
				}
			}
		}

		// remove whitespaces (if required)
		if (whitespace != std::numeric_limits<std::size_t>::max()) {
			deleteText(transaction, DocPos(i, whitespace), DocPos(i, lineSize));
		}
	}

	// update cursor if transaction wasn't empty
	if (endTransaction(transaction)) {
		cursors.setCursor(normalizePos(cursors.getCurrent().getSelectionEnd()));
	}
}


//
//	TextEditor::filterLines
//

void TextEditor::filterLines(std::function<std::string(std::string_view)> filter) {
	auto transaction = startTransaction();

	// process all the lines
	for (size_t i = 0; i < document.size(); i++) {
		// get original text and run it through filter
		auto before = document.getLineText(i);
		std::string after = filter(before);

		// update line if anything changed
		if (after != before) {
			auto start = DocPos(i, 0);
			auto end = document.getEndOfLine(start);
			deleteText(transaction, start, end);
			insertText(transaction, start, after);
		}
	}

	// update cursor if transaction wasn't empty
	if (endTransaction(transaction)) {
		cursors.setCursor(normalizePos(cursors.getCurrent().getSelectionEnd()));
	}
}


//
//	TextEditor::tabsToSpaces
//

void TextEditor::tabsToSpaces() {
	filterLines([this](const std::string_view& input) {
		std::string output;
		auto end = input.end();
		auto i = input.begin();
		size_t pos = 0;

		while (i < end) {
			char utf8[4];
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);

			if (codepoint == '\t') {
				auto spaces = config.tabSize - (pos % config.tabSize);
				output.append(spaces, ' ');
				pos += spaces;

			} else {
				output.append(utf8, CodePoint::write(utf8, codepoint));
				pos++;
			}
		}

		return output;
	});
}


//
//	TextEditor::spacesToTabs
//

void TextEditor::spacesToTabs() {
	FilterLines([this](const std::string_view& input) {
		std::string output;
		auto end = input.end();
		auto i = input.begin();
		size_t pos = 0;
		size_t spaces = 0;

		while (i < end) {
			char utf8[4];
			ImWchar codepoint;
			i = CodePoint::read(i, end, &codepoint);

			if (codepoint == ' ') {
				spaces++;

			} else {
				while (spaces) {
					auto spacesUntilNextTab = config.tabSize - (pos % config.tabSize);

					if (spacesUntilNextTab == 1) {
						output += ' ';
						pos++;
						spaces--;

					} else if (spaces >= spacesUntilNextTab) {
						output += '\t';
						pos += spacesUntilNextTab;
						spaces -= spacesUntilNextTab;

					} else if (codepoint != '\t')
						while (spaces) {
							output += ' ';
							pos++;
							spaces--;
						}

					else {
						spaces = 0;
					}
				}

				if (codepoint == '\t') {
					output += '\t';
					pos += config.tabSize - (pos % config.tabSize);

				} else {
					output.append(utf8, CodePoint::write(utf8, codepoint));
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

std::shared_ptr<TextEditor::Transaction> TextEditor::startTransaction(bool cancelsAutoComplete) {
	if (cancelsAutoComplete) {
		autocomplete.cancel();
	}

	std::shared_ptr<Transaction> transaction = Transactions::create();
	transaction->setBeforeState(cursors);
	return transaction;
}


//
//	TextEditor::endTransaction
//

bool TextEditor::endTransaction(std::shared_ptr<Transaction> transaction) {
	if (transaction->size() > 0) {
		cursors.update(document);
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

void TextEditor::insertTextIntoAllCursors(std::shared_ptr<Transaction> transaction, const std::string_view& text) {
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
			cursor->update(start, false);
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
		auto index = start.index;
		auto& line = document[start.line];
		ImWchar previousChar = index > 0 ? line[index - 1].codepoint : 0;
		ImWchar nextChar = index < line.size() ? line[index].codepoint : 0;

		// remove extra whitespaces if required
		if (CodePoint::isWhiteSpace(nextChar)) {
			while (index < line.size() && CodePoint::isWhiteSpace(line[index].codepoint)) {
				index++;
			}

			auto end = DocPos(start.line, index);
			deleteText(transaction, start, end);
			cursors.adjustForDelete(cursor, start, end);
		}

		// determine whitespace at start of current line
		std::string whitespace;

		for (size_t i = 0; i < line.size() && CodePoint::isWhiteSpace(line[i].codepoint); i++) {
			char utf8[4];
			whitespace.append(utf8, CodePoint::write(utf8, line[i].codepoint));
		}

		// determine text to insert
		std::string insert = "\n" + whitespace;
		auto newCursorIndex = whitespace.size();

		// handle special cases
		if (previousChar == CodePoint::openCurlyBracket || previousChar == CodePoint::openSquareBracket) {
			// add to an existing block
			insert += "\t";
			newCursorIndex++;

			if ((previousChar == CodePoint::openCurlyBracket && nextChar == CodePoint::closeCurlyBracket) ||
				(previousChar == CodePoint::openSquareBracket && nextChar == CodePoint::closeSquareBracket)) {

				// open a new block
				insert += "\n" + whitespace;
			}
		}

		// insert new text
		auto end = insertText(transaction, start, insert);
		cursors.adjustForInsert(cursor, start, end);

		// set new cursor location
		cursor->update(DocPos(start.line + 1, newCursorIndex), false);
	}
}


//
//	TextEditor::insertText
//

TextEditor::DocPos TextEditor::insertText(std::shared_ptr<Transaction> transaction, DocPos start, const std::string_view& text) {
	// update document, add transaction and return position of end of insert
	// this function does not change the cursors
	auto end = document.insertText(config, start, text);
	transaction->addInsert(start, end, text);
	makeCursorVisible();
	return end;
}


//
//	TextEditor::deleteText
//

void TextEditor::deleteText(std::shared_ptr<Transaction> transaction, DocPos start, DocPos end) {
	// update document and add transaction
	// this function does not change the cursors
	auto text = document.getSectionText(start, end);
	document.deleteText(config, start, end);
	transaction->addDelete(start, end, text);
	makeCursorVisible();
}


//
//	TextEditor::SetLanguage
//

void TextEditor::SetLanguage(const Language* language) {
	config.language = language;

	if (languageChangeCallback) {
		languageChangeCallback();
	}
}
