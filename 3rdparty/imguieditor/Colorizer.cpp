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
//	TextEditor::Colorizer::matches
//

bool TextEditor::Colorizer::matches(Line::iterator start, Line::iterator end, const std::string_view& text) {
	// see if text between iterators matches provided UTF-8 string
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


//
//	TextEditor::Colorizer::updateLine
//

TextEditor::LineState TextEditor::Colorizer::updateLine(Line& line) {
	// initialize local variables
	auto state = line.state;
	auto nonWhiteSpace = false;
	auto glyph = line.begin();
	auto end = line.end();
	Iterator lineEnd(line.data() + line.size());

	// process all glyphs on this line
	while (glyph < end) {
		// start parsing glyphs
		auto start = glyph;
		Iterator tokenStart(&*glyph);

		if (state == LineState::inText) {
			// special handling for preprocessor lines
			if (!nonWhiteSpace && language->preprocess && glyph->codepoint != language->preprocess && !CodePoint::isWhiteSpace(glyph->codepoint)) {
				nonWhiteSpace = true;
			}

			// are we starting a multilevel, multiline comment
			if (language->commentLevelStart) {
				size_t level;
				Iterator tokenEnd = language->commentLevelStart(tokenStart, lineEnd, level);

				if (tokenEnd != tokenStart) {
					level = std::min(level, maxCommentLevel);
					state = commentLevelToLineState(level);
					auto size = tokenEnd - tokenStart;
					setColor(glyph, glyph + size, Color::comment);
					glyph += size;
				}
			}

			// are we starting a multilevel, multiline string
			if (glyph == start && language->stringLevelStart) {
				size_t level;
				Iterator tokenEnd = language->stringLevelStart(tokenStart, lineEnd, level);

				if (tokenEnd != tokenStart) {
					level = std::min(level, maxStringLevel);
					state = stringLevelToLineState(level);
					auto size = tokenEnd - tokenStart;
					setColor(glyph, glyph + size, Color::string);
					glyph += size;
				}
			}

			if (glyph == start) {
				// mark whitespace characters
				if (CodePoint::isWhiteSpace(glyph->codepoint)) {
					(glyph++)->color = Color::whitespace;

				// are we starting a multiline comment
				} else if (language->commentStart.size() && matches(glyph, end, language->commentStart)) {
					state = LineState::inComment;
					auto size = language->commentStart.size();
					setColor(glyph, glyph + size, Color::comment);
					glyph += size;

				// handle single line comments
				} else if (language->singleLineComment.size() && matches(glyph, end, language->singleLineComment)) {
					setColor(glyph, end, Color::comment);
					glyph = end;

				} else if (language->singleLineCommentAlt.size() && matches(glyph, end, language->singleLineCommentAlt)) {
					setColor(glyph, end, Color::comment);
					glyph = end;

				// are we starting a special string
				} else if (language->otherStringStart.size() && matches(glyph, end, language->otherStringStart)) {
					state = LineState::inOtherString;
					auto size = language->otherStringStart.size();
					setColor(glyph, glyph + size, Color::string);
					glyph += size;

				} else if (language->otherStringAltStart.size() && matches(glyph, end, language->otherStringAltStart)) {
					state = LineState::inOtherStringAlt;
					auto size = language->otherStringAltStart.size();
					setColor(glyph, glyph + size, Color::string);
					glyph += size;

				// are we starting a single quoted string
				} else if (language->hasSingleQuotedStrings && glyph->codepoint == CodePoint::singleQuote) {
					state = LineState::inSingleQuotedString;
					(glyph++)->color = Color::string;

				// are we starting a double quoted string
				} else if (language->hasDoubleQuotedStrings && glyph->codepoint == CodePoint::doubleQuote) {
					state = LineState::inDoubleQuotedString;
					(glyph++)->color = Color::string;

				// is this a preprocessor line
				} else if (language->preprocess && !nonWhiteSpace && glyph->codepoint == language->preprocess) {
					setColor(line.begin(), end, Color::preprocessor);
					glyph = end;
				}
			}

			if (glyph == start) {
				// nothing worked so far so it's time to do some tokenizing
				Color color;
				Iterator tokenEnd;

				// handle custom tokenizer (if we have one)
				if (language->customTokenizer&& (tokenEnd = language->customTokenizer(tokenStart, lineEnd, color)) != tokenStart) {
					auto size = tokenEnd - tokenStart;
					setColor(glyph, glyph + size, color);
					glyph += size;

				// do we have an identifier
				} else if (language->getIdentifier && (tokenEnd = language->getIdentifier(tokenStart, lineEnd)) != tokenStart) {
					// determine identifier text and color
					auto size = tokenEnd - tokenStart;
					std::string identifier;
					color = Color::identifier;

					for (auto i = tokenStart; i < tokenEnd; i++) {
						ImWchar codepoint = *i;

						if (!language->caseSensitive) {
							codepoint = CodePoint::toLower(codepoint);
						}

						char utf8[4];
						identifier.append(utf8, CodePoint::write(utf8, codepoint));
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

		} else if (lineStateInComment(state)) {
			// stay in comment state until we see the end sequence
			auto size = language->commentEnd.size();

			if (size && matches(glyph, end, language->commentEnd)) {
				setColor(glyph, glyph + size, Color::comment);
				glyph += size;
				state = LineState::inText;

			} else if (language->commentLevelEnd) {
				size_t level;
				Iterator tokenEnd = language->commentLevelEnd(tokenStart, lineEnd, level);

				if (tokenEnd != tokenStart) {
					level = std::min(level, maxCommentLevel);

					if (state == commentLevelToLineState(level)) {
						size = tokenEnd - tokenStart;
						setColor(glyph, glyph + size, Color::comment);
						glyph += size;
						state = LineState::inText;
					}
				}
			}

			if (lineStateInComment(state)) {
				(glyph++)->color = Color::comment;
			}

		} else if (lineStateInStringLevel(state)) {
			// stay in string level until matching closing is detected
			size_t level;
			Iterator lineEnd(line.data() + line.size());
			Iterator tokenEnd = language->stringLevelEnd(tokenStart, lineEnd, level);

			if (tokenEnd != tokenStart) {
				level = std::min(level, maxStringLevel);

				if (state == stringLevelToLineState(level)) {
					auto size = tokenEnd - tokenStart;
					setColor(glyph, glyph + size, Color::string);
					glyph += size;
					state = LineState::inText;

				} else {
					(glyph++)->color = Color::string;
				}

			} else {
				(glyph++)->color = Color::string;
			}

		} else if (state == LineState::inOtherString) {
			// stay in otherString state until we see the end sequence
			// skip escaped characters
			if (glyph->codepoint == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < end) {
					(glyph++)->color = Color::string;
				}

			} else if (matches(glyph, end, language->otherStringEnd)) {
				auto size = language->otherStringEnd.size();
				setColor(glyph, glyph + size, Color::string);
				glyph += size;
				state = LineState::inText;

			} else {
				(glyph++)->color = Color::string;
			}

		} else if (state == LineState::inOtherStringAlt) {
			// stay in otherStringAlt state until we see the end sequence
			// skip escaped characters
			if (glyph->codepoint == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < end) {
					(glyph++)->color = Color::string;
				}

			} else if (matches(glyph, end, language->otherStringAltEnd)) {
				auto size = language->otherStringAltEnd.size();
				setColor(glyph, glyph + size, Color::string);
				glyph += size;
				state = LineState::inText;

			} else {
				(glyph++)->color = Color::string;
			}

		} else if (state == LineState::inSingleQuotedString) {
			// stay in single quote state until we see an end
			// skip escaped characters
			if (glyph->codepoint == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < end) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->codepoint == CodePoint::singleQuote) {
				(glyph++)->color = Color::string;
				state = LineState::inText;

			} else {
				(glyph++)->color = Color::string;
			}

		} else if (state == LineState::inDoubleQuotedString) {
			// stay in double quote state until we see an end
			// skip escaped characters
			if (glyph->codepoint == language->stringEscape) {
				(glyph++)->color = Color::string;

				if (glyph < end) {
					(glyph++)->color = Color::string;
				}

			} else if (glyph->codepoint == CodePoint::doubleQuote) {
				(glyph++)->color = Color::string;
				state = LineState::inText;

			} else {
				(glyph++)->color = Color::string;
			}
		}
	}

	return state;
}


//
//	TextEditor::Colorizer::update
//

bool TextEditor::Colorizer::update(const Config& config, Document& document) {
	// update all lines on configuration change
	bool configChanged = language != config.language;

	if (configChanged) {
		language = config.language;

		if (language) {
			for (auto line = document.begin(); line < document.end(); line++) {
				auto state = updateLine(*line);
				line->needsColorizing = false;
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

				line->state = LineState::inText;
				line->needsColorizing = false;
			}
		}

	// update changed lines when document is updated
	} else if (document.isUpdated()) {
		for (auto line = document.begin(); line < document.end(); line++) {
			if (line->needsColorizing) {
				if (language) {
					auto state = updateLine(*line);
					line->needsColorizing = false;
					auto next = line + 1;

					if (next < document.end() && next->state != state) {
						next->state = state;
						next->needsColorizing = true;
					}

				} else {
					for (auto glyph = line->begin(); glyph < line->end(); glyph++) {
						glyph->color = Color::text;
					}

					line->state = LineState::inText;
					line->needsColorizing = false;
				}
			}
		}
	}

	// return status
	return configChanged;
}
