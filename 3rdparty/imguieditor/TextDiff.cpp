//	TextDiff - A syntax highlighting text diff widget for Dear ImGui.
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "dtl.h"
#include "TextDiff.h"


//
//	TextDiff::TextDiff
//

TextDiff::TextDiff() {
	readOnly = true;
	showLineNumbers = false;
	showMatchingBrackets = false;
}


//
//	TextDiff::SetText
//

void TextDiff::SetText(const std::string_view& left, const std::string_view& right) {
	// split text into lines
	std::vector<std::string_view> leftLines;
	std::vector<std::string_view> rightLines;
	splitLines(leftLines, left);
	splitLines(rightLines, right);

	// create two documents and colorize them
	leftDocument.setText(leftLines);
	rightDocument.setText(rightLines);

	colorizer.updateEntireDocument(leftDocument, language);
	colorizer.updateEntireDocument(rightDocument, language);

	// setup line number decoration
	leftLineNumberDigits = static_cast<int>(std::log10(leftDocument.lineCount() + 1) + 1.0f);
	rightLineNumberDigits = static_cast<int>(std::log10(leftDocument.lineCount() + 1) + 1.0f);
	decoratorWidth = -(leftLineNumberDigits + rightLineNumberDigits + 4.0f);

	decoratorCallback = [this](TextEditor::Decorator& decorator) {
		this->decorateLine(decorator);
	};

	// calculate the difference
	dtl::Diff<std::string_view> diff(leftLines, rightLines);
	diff.compose();
	auto ranges = diff.getSes().getSequence();

	// determine status of each line in diff view
	lineInfo.clear();
	size_t leftIndex = 0;
	size_t rightIndex = 0;

	for (auto& [text, info] : ranges) {
		switch (info.type) {
			case dtl::SES_COMMON:
				lineInfo.emplace_back(leftIndex++, rightIndex++, Status::common);
				break;

			case dtl::SES_ADD:
				lineInfo.emplace_back(leftIndex, rightIndex++, Status::added);
				break;

			case dtl::SES_DELETE:
				lineInfo.emplace_back(leftIndex++, rightIndex, Status::deleted);
				break;
		}
	}

	// create a combined view
	createCombinedView();
}


//
//	TextDiff::SetLanguage
//

void TextDiff::SetLanguage(const Language* l) {
	language = l;
	colorizer.updateEntireDocument(leftDocument, language);
	colorizer.updateEntireDocument(rightDocument, language);
	createCombinedView();
}


//
//	TextDiff::splitLines
//

void TextDiff::splitLines(std::vector<std::string_view>& result, const std::string_view& text) {
	size_t prev = CodePoint::skipBOM(text.begin(), text.end()) - text.begin();
	size_t pos;

	while ((pos = text.find('\n', prev)) != std::string_view::npos) {
		result.push_back(text.substr(prev, pos - prev));
		prev = pos + 1;
	}

	result.push_back(text.substr(prev));
}


//
//	TextDiff::createCombinedView
//

void TextDiff::createCombinedView() {
	document.clear();
	cursors.clearAll();
	clearMarkers();

	for (size_t i = 0; i < lineInfo.size(); i++) {
		auto& line = lineInfo[i];

		switch(line.status) {
			case Status::common:
				document.emplace_back(leftDocument[line.leftLine]);
				break;

			case Status::added:
				document.emplace_back(rightDocument[line.rightLine]);
				addMarker(static_cast<int>(i), 0, addedColor, "", "");
				break;

			case Status::deleted:
				document.emplace_back(leftDocument[line.leftLine]);
				addMarker(static_cast<int>(i), 0, deletedColor, "", "");
				break;
		}
	}
}


//
//	TextDiff::decorateLine
//

void TextDiff::decorateLine(TextEditor::Decorator& decorator) {
	auto& line = lineInfo[decorator.line];

	switch(line.status) {
		case Status::common:
			ImGui::Text(" %*ld %*ld  ", leftLineNumberDigits, line.leftLine + 1, rightLineNumberDigits, line.rightLine + 1);
			break;

		case Status::added:
			ImGui::Text(" %*s %*ld +", leftLineNumberDigits, "", rightLineNumberDigits, line.rightLine + 1);
			break;

		case Status::deleted:
			ImGui::Text(" %*ld %*s -", leftLineNumberDigits, line.leftLine + 1, rightLineNumberDigits, "");
			break;
	}
}
