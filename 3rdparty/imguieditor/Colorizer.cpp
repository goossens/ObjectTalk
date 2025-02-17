//	TextEditor - A syntax highlighting text editor for ImGui
//	Copyright (c) 2024-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "TextEditor.h"


//
//	TextEditor::Colorizer::update
//

TextEditor::State TextEditor::Colorizer::update(Line& line, const Language* language) {
	auto state = line.state;

	// process all glyphs on this line
	auto nonWhiteSpace = false;
	auto glyph = line.begin();

	while (glyph < line.end()) {
		if (state == State::inText) {
			// special handling for preprocessor lines
			if (!nonWhiteSpace && language->preprocess && glyph->codepoint != language->preprocess && !CodePoint::isWhiteSpace(glyph->codepoint)) {
				nonWhiteSpace = true;
			}

			// start parsing glyphs
			auto start = glyph;

			// mark whitespace characters
			if (CodePoint::isWhiteSpace(glyph->codepoint)) {
				(glyph++)->color = Color::whitespace;

			// handle single line comments
			} else if (language->singleLineComment.size() && matches(glyph, line.end(), language->singleLineComment)) {
				setColor(glyph, line.end(), Color::comment);
				glyph = line.end();

			} else if (language->singleLineCommentAlt.size() && matches(glyph, line.end(), language->singleLineCommentAlt)) {
				setColor(glyph, line.end(), Color::comment);
				glyph = line.end();

			// are we starting a multiline comment
			} else if (language->commentStart.size() && matches(glyph, line.end(), language->commentStart)) {
				state = State::inComment;
				auto size = language->commentEnd.size();
				setColor(glyph, glyph + size, Color::comment);
				glyph += size;

			// are we starting a special string
			} else if (language->otherStringStart.size() && matches(glyph, line.end(), language->otherStringStart)) {
				state = State::inOtherString;
				auto size = language->otherStringStart.size();
				setColor(glyph, glyph + size, Color::string);
				glyph += size;

			} else if (language->otherStringAltStart.size() && matches(glyph, line.end(), language->otherStringAltStart)) {
				state = State::inOtherStringAlt;
				auto size = language->otherStringAltStart.size();
				setColor(glyph, glyph + size, Color::string);
				glyph += size;

			// are we starting a single quoted string
			} else if (language->hasSingleQuotedStrings && glyph->codepoint == '\'') {
				state = State::inSingleQuotedString;
				(glyph++)->color = Color::string;

			// are we starting a double quoted string
			} else if (language->hasDoubleQuotedStrings && glyph->codepoint == '"') {
				state = State::inDoubleQuotedString;
				(glyph++)->color = Color::string;

			// is this a preprocessor line
			} else if (language->preprocess && !nonWhiteSpace && glyph->codepoint == language->preprocess) {
				setColor(line.begin(), line.end(), Color::preprocessor);
				glyph = line.end();

			// handle custom tokenizer (if we have one)
			} else if (language->customTokenizer) {
				Color color;
				Iterator tokenStart(&*glyph);
				Iterator lineEnd(line.data() + line.size());
				Iterator tokenEnd = language->customTokenizer(tokenStart, lineEnd, color);

				if (tokenEnd != tokenStart) {
					auto size = tokenEnd - tokenStart;
					setColor(glyph, glyph + size, color);
					glyph += size;
				}
			}

			if (glyph == start) {
				// nothing worked so far so it's time to do some tokenizing
				Color color;
				Iterator lineEnd(line.data() + line.size());
				Iterator tokenStart(&*glyph);
				Iterator tokenEnd;

				// do we have an identifier
				if (language->getIdentifier && (tokenEnd = language->getIdentifier(tokenStart, lineEnd)) != tokenStart) {
					// determine identifier text and color color
					auto size = tokenEnd - tokenStart;
					std::string identifier;
					color = Color::identifier;

					for (auto i = tokenStart; i < tokenEnd; i++) {
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
					setColor(glyph, glyph + size, color);
					glyph += size;

				// do we have a number
				} else if (language->getNumber && (tokenEnd = language->getNumber(tokenStart, lineEnd)) != tokenStart) {
					auto size = tokenEnd - tokenStart;
					setColor(glyph, glyph + size, Color::number);
					glyph += size;

				// is this punctuation
				} else if (language->isPunctuation && language->isPunctuation(glyph->codepoint)) {
					(glyph++)->color = Color::punctuation;

				} else {
					// I guess we don't know what this character is
					(glyph++)->color = Color::text;
				}
			}

		} else if (state == State::inComment) {
			// stay in comment state until we see the end sequence
			if (matches(glyph, line.end(), language->commentEnd)) {
				auto size = language->commentEnd.size();
				setColor(glyph, glyph + size, Color::comment);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inOtherString) {
			// stay in otherString state until we see the end sequence
			// skip escaped characters
			if (glyph->codepoint == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (matches(glyph, line.end(), language->otherStringEnd)) {
				auto size = language->otherStringEnd.size();
				setColor(glyph, glyph + size, Color::string);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inOtherStringAlt) {
			// stay in otherStringAlt state until we see the end sequence
			// skip escaped characters
			if (glyph->codepoint == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (matches(glyph, line.end(), language->otherStringAltEnd)) {
				auto size = language->otherStringAltEnd.size();
				setColor(glyph, glyph + size, Color::string);
				glyph += size;
				state = State::inText;

			} else {
				(glyph++)->color = Color::comment;
			}

		} else if (state == State::inSingleQuotedString) {
			// stay in single quote state until we see an end
			// skip escaped characters
			if (glyph->codepoint == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->codepoint == '\'') {
				(glyph++)->color = Color::string;
				state = State::inText;

			} else {
				(glyph++)->color = Color::string;
			}

		} else if (state == State::inDoubleQuotedString) {
			// stay in double quote state until we see an end
			// skip escaped characters
			if (glyph->codepoint == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < line.end()) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->codepoint == '"') {
				(glyph++)->color = Color::string;
				state = State::inText;

			} else {
				(glyph++)->color = Color::string;
			}
		}
	}

	line.colorize = false;
	return state;
}


//
//	TextEditor::Colorizer::updateEntireDocument
//

void TextEditor::Colorizer::updateEntireDocument(Document& document, const Language* language) {
	if (language) {
		for (auto line = document.begin(); line < document.end(); line++) {
			auto state = update(*line, language);
			auto next = line + 1;

			if (next < document.end()) {
				next->state = state;
			}
		}

	} else {
		for (auto line = document.begin(); line < document.end(); line++) {
			for (auto glyph = line->begin(); glyph < line->end(); glyph++) {
				glyph->color = Color::text;
			}

			line->state = State::inText;
			line->colorize = false;
		}
	}
}


//
//	TextEditor::Colorizer::updateChangedLines
//

void TextEditor::Colorizer::updateChangedLines(Document& document, const Language* language) {
	for (auto line = document.begin(); line < document.end(); line++) {
		if (line->colorize) {
			auto state = update(*line, language);
			auto next = line + 1;

			if (next < document.end() && next->state != state) {
				next->state = state;
				next->colorize = true;
			}
		}
	}
}


//
//	TextEditor::Colorizer::matches
//

bool TextEditor::Colorizer::matches(Line::iterator start, Line::iterator end, const std::string_view& text) {
	// see if text at iterators matches provided UTF-8 string
	auto i = text.begin();

	while (i < text.end()) {
		if (start == end) {
			return false;
		}

		ImWchar codepoint;
		i = CodePoint::read(i, text.end(), &codepoint);

		if ((start++)->codepoint != codepoint) {
			return false;
		}
	}

	return true;
}
