#include <cctype>
#include <cmath>
#include <iterator>
#include <set>

#include "TextEditor.h"

// --------------------------------------- //
// ------------- Exposed API ------------- //

TextEditor::TextEditor()
{
	SetPalette(defaultPalette);
	mLines.push_back(Line());
}

void TextEditor::SetPalette(const Palette& aValue)
{
	mPaletteBase = aValue;
	mPaletteAlpha = -1.0f;
}

void TextEditor::SetLanguageDefinition(const LanguageDefinition& aLanguageDef)
{
	mLanguageDefinition = &aLanguageDef;
	mRegexList.clear();
	for (const auto& r : mLanguageDefinition->mTokenRegexStrings)
		mRegexList.push_back(std::make_pair(boost::regex(r.first, boost::regex_constants::optimize), r.second));

	Colorize();
}

const char* TextEditor::GetLanguageDefinitionName() const
{
	return mLanguageDefinition != nullptr ? mLanguageDefinition->mName.c_str() : "None";
}

void TextEditor::SelectAll()
{
	ClearSelections();
	ClearExtraCursors();
	MoveTop();
	MoveBottom(true);
}

void TextEditor::SelectLine(int aLine)
{
	ClearSelections();
	ClearExtraCursors();
	SetSelection({ aLine, 0 }, { aLine, GetLineMaxColumn(aLine) });
}

void TextEditor::SelectRegion(int aStartLine, int aStartChar, int aEndLine, int aEndChar)
{
	ClearSelections();
	ClearExtraCursors();
	SetSelection(aStartLine, aStartChar, aEndLine, aEndChar);
}

void TextEditor::SelectNextOccurrenceOf(const char* aText, int aTextSize, bool aCaseSensitive, bool aWholeWord)
{
	ClearSelections();
	ClearExtraCursors();
	SelectNextOccurrenceOf(aText, aTextSize, -1, aCaseSensitive, aWholeWord);
}

void TextEditor::SelectAllOccurrencesOf(const char* aText, int aTextSize, bool aCaseSensitive, bool aWholeWord)
{
	ClearSelections();
	ClearExtraCursors();
	SelectNextOccurrenceOf(aText, aTextSize, -1, aCaseSensitive, aWholeWord);
	Coordinates startPos = mState.mCursors[mState.GetLastAddedCursorIndex()].mInteractiveEnd;
	while (true)
	{
		AddCursorForNextOccurrence(aCaseSensitive, aWholeWord);
		Coordinates lastAddedPos = mState.mCursors[mState.GetLastAddedCursorIndex()].mInteractiveEnd;
		if (lastAddedPos == startPos)
			break;
	}
}

bool TextEditor::AnyCursorHasSelection() const
{
	for (int c = 0; c <= mState.mCurrentCursor; c++)
		if (mState.mCursors[c].HasSelection())
			return true;
	return false;
}

bool TextEditor::AllCursorsHaveSelection() const
{
	for (int c = 0; c <= mState.mCurrentCursor; c++)
		if (!mState.mCursors[c].HasSelection())
			return false;
	return true;
}

void TextEditor::ClearExtraCursors()
{
	mState.mCurrentCursor = 0;
}

void TextEditor::ClearSelections()
{
	for (int c = mState.mCurrentCursor; c > -1; c--)
		mState.mCursors[c].mInteractiveEnd =
		mState.mCursors[c].mInteractiveStart =
		mState.mCursors[c].GetSelectionEnd();
}

void TextEditor::ReplaceTextInCurrentCursor(const std::string& aText)
{
	if (mReadOnly)
		return;

	if (!AnyCursorHasSelection())
		return;

	UndoRecord u;
	u.mBefore = mState;
	int cursor = mState.mCurrentCursor;

	u.mOperations.push_back({ GetSelectedText(cursor), mState.mCursors[cursor].GetSelectionStart(), mState.mCursors[cursor].GetSelectionEnd(), UndoOperationType::Delete });
	DeleteSelection(cursor);
	Coordinates start = GetActualCursorCoordinates(cursor);
	InsertTextAtCursor(aText.c_str(), cursor);
	u.mOperations.push_back({ aText, start, GetActualCursorCoordinates(cursor), UndoOperationType::Add });

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::ReplaceTextInAllCursors(const std::string& aText)
{
	if (mReadOnly)
		return;

	if (!AnyCursorHasSelection())
		return;

	UndoRecord u;
	u.mBefore = mState;

	for (int c = mState.mCurrentCursor; c > -1; c--)
	{
		u.mOperations.push_back({ GetSelectedText(c), mState.mCursors[c].GetSelectionStart(), mState.mCursors[c].GetSelectionEnd(), UndoOperationType::Delete });
		DeleteSelection(c);
	}

	for (int c = mState.mCurrentCursor; c > -1; c--)
	{
		Coordinates start = GetActualCursorCoordinates(c);
		InsertTextAtCursor(aText.c_str(), c);
		u.mOperations.push_back({ aText, start, GetActualCursorCoordinates(c), UndoOperationType::Add });
	}

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::SetCursorPosition(int aLine, int aCharIndex)
{
	SetCursorPosition({ aLine, GetCharacterColumn(aLine, aCharIndex) }, -1, true);
}

int TextEditor::GetFirstVisibleLine()
{
	return mFirstVisibleLine;
}

int TextEditor::GetLastVisibleLine()
{
	return mLastVisibleLine;
}

void TextEditor::SetViewAtLine(int aLine, SetViewAtLineMode aMode)
{
	mSetViewAtLine = aLine;
	mSetViewAtLineMode = aMode;
}

void TextEditor::Copy()
{
	if (AnyCursorHasSelection())
		ImGui::SetClipboardText(GetAllSelectedText().c_str());
	else
	{
		auto line = GetActualCursorCoordinates().mLine;
		auto start = Coordinates{line, 0};
		auto end = Coordinates{line, GetLineMaxColumn(line)};

		if (start != end)
			ImGui::SetClipboardText((GetText(start, end) + '\n').c_str());
		else
			ImGui::SetClipboardText("\n");
	}
}

void TextEditor::Cut()
{
	if (mReadOnly)
	{
		Copy();
	}
	else
	{
		if (AnyCursorHasSelection())
		{
			UndoRecord u;
			u.mBefore = mState;

			Copy();
			for (int c = mState.mCurrentCursor; c > -1; c--)
			{
				u.mOperations.push_back({ GetSelectedText(c), mState.mCursors[c].GetSelectionStart(), mState.mCursors[c].GetSelectionEnd(), UndoOperationType::Delete });
				DeleteSelection(c);
			}

			u.mAfter = mState;
			AddUndo(u);
		}
	}
}

void TextEditor::Paste()
{
	if (mReadOnly)
		return;

	if (ImGui::GetClipboardText() == nullptr)
		return; // something other than text in the clipboard

	// check if we should do multicursor paste
	std::string clipText = ImGui::GetClipboardText();
	bool canPasteToMultipleCursors = false;
	std::vector<std::pair<int, int>> clipTextLines;
	if (mState.mCurrentCursor > 0)
	{
		clipTextLines.push_back({ 0,0 });
		for (int i = 0; i < clipText.length(); i++)
		{
			if (clipText[i] == '\n')
			{
				clipTextLines.back().second = i;
				clipTextLines.push_back({ i + 1, 0 });
			}
		}
		clipTextLines.back().second = int(clipText.length());
		canPasteToMultipleCursors = clipTextLines.size() == mState.mCurrentCursor + 1;
	}

	if (clipText.length() > 0)
	{
		UndoRecord u;
		u.mBefore = mState;

		if (AnyCursorHasSelection())
		{
			for (int c = mState.mCurrentCursor; c > -1; c--)
			{
				u.mOperations.push_back({ GetSelectedText(c), mState.mCursors[c].GetSelectionStart(), mState.mCursors[c].GetSelectionEnd(), UndoOperationType::Delete });
				DeleteSelection(c);
			}
		}

		for (int c = mState.mCurrentCursor; c > -1; c--)
		{
			Coordinates start = GetActualCursorCoordinates(c);
			if (canPasteToMultipleCursors)
			{
				std::string clipSubText = clipText.substr(clipTextLines[c].first, clipTextLines[c].second - clipTextLines[c].first);
				InsertTextAtCursor(clipSubText.c_str(), c);
				u.mOperations.push_back({ clipSubText, start, GetActualCursorCoordinates(c), UndoOperationType::Add });
			}
			else
			{
				InsertTextAtCursor(clipText.c_str(), c);
				u.mOperations.push_back({ clipText, start, GetActualCursorCoordinates(c), UndoOperationType::Add });
			}
		}

		u.mAfter = mState;
		AddUndo(u);
	}
}

void TextEditor::Undo(int aSteps)
{
	while (CanUndo() && aSteps-- > 0)
		mUndoBuffer[--mUndoIndex].Undo(this);
}

void TextEditor::Redo(int aSteps)
{
	while (CanRedo() && aSteps-- > 0)
		mUndoBuffer[mUndoIndex++].Redo(this);
}

void TextEditor::SetText(const std::string& aText)
{
	mLines.clear();
	mLines.emplace_back(Line());
	for (auto chr : aText)
	{
		if (chr == '\n')
			mLines.emplace_back(Line());
		else if (chr != '\r')
		{
			mLines.back().emplace_back(Glyph(chr, PaletteIndex::Default));
		}
	}

	mScrollToTop = true;

	mUndoBuffer.clear();
	mUndoIndex = 0;

	Colorize();
}

std::string TextEditor::GetText() const
{
	auto lastLine = int(mLines.size()) - 1;
	auto lastLineLength = GetLineMaxColumn(lastLine);
	Coordinates startCoords = Coordinates();
	Coordinates endCoords = Coordinates(lastLine, lastLineLength);
	return startCoords < endCoords ? GetText(startCoords, endCoords) : "";
}

void TextEditor::SetTextLines(const std::vector<std::string>& aLines)
{
	mLines.clear();

	if (aLines.empty())
		mLines.emplace_back(Line());
	else
	{
		mLines.resize(aLines.size());

		for (size_t i = 0; i < aLines.size(); ++i)
		{
			const std::string& aLine = aLines[i];

			mLines[i].reserve(aLine.size());
			for (size_t j = 0; j < aLine.size(); ++j)
				mLines[i].emplace_back(Glyph(aLine[j], PaletteIndex::Default));
		}
	}

	mScrollToTop = true;

	mUndoBuffer.clear();
	mUndoIndex = 0;

	Colorize();
}

std::vector<std::string> TextEditor::GetTextLines() const
{
	std::vector<std::string> result;

	result.reserve(mLines.size());

	for (auto& line : mLines)
	{
		std::string text;

		text.resize(line.size());

		for (size_t i = 0; i < line.size(); ++i)
			text[i] = line[i].mChar;

		result.emplace_back(std::move(text));
	}

	return result;
}

void TextEditor::StripTrailingWhitespaces()
{
	UndoRecord u;
	u.mBefore = mState;

	// process all the lines
	for (int i = 0; i < mLines.size(); i++)
	{
		int size = int(mLines[i].size());
		int cindex = size;
		int whitespace = -1;

		// look for first non-whitespace at the end of the line
		while (Move(i, cindex, true, true))
		{
			if (isspace(mLines[i][cindex].mChar))
				whitespace = cindex;
			else
				break;
		}

		// remove all white spaces if required
		if (whitespace >= 0)
		{
			auto start = Coordinates(i, GetCharacterColumn(i, whitespace));
			auto end = Coordinates(i, GetCharacterColumn(i, size));
			u.mOperations.push_back({GetText(start, end), start, end, UndoOperationType::Delete});
			DeleteRange(start, end);
		}
	}

	// only create an undo record if any line had whitespaces at the end
	if (u.mOperations.size())
	{
		u.mAfter = mState;
		AddUndo(u);
	}
}

void TextEditor::Render(const char* aTitle, const ImVec2& aSize, bool aBorder)
{
	if (mCursorPositionChanged)
		OnCursorPositionChanged();
	mCursorPositionChanged = false;

	if (mPaletteAlpha != ImGui::GetStyle().Alpha)
		UpdatePalette();

	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::ColorConvertU32ToFloat4(mPalette[int(PaletteIndex::Background)]));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	ImGui::BeginChild(aTitle, aSize, aBorder, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavInputs);
	HandleKeyboardInputs();
	HandleMouseInputs();
	ColorizeInternal();
	Render();
	ImGui::EndChild();

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
}

// ------------------------------------ //
// ---------- Generic utils ----------- //

static inline bool IsUTFSequence(char c)
{
	return (c & 0xC0) == 0x80;
}

// https://en.wikipedia.org/wiki/UTF-8
// We assume that the char is a standalone character (<128) or a leading byte of an UTF-8 code sequence (non-10xxxxxx code)
static inline int UTF8CharLength(char c)
{
	if ((c & 0xFE) == 0xFC)
		return 6;
	if ((c & 0xFC) == 0xF8)
		return 5;
	if ((c & 0xF8) == 0xF0)
		return 4;
	else if ((c & 0xF0) == 0xE0)
		return 3;
	else if ((c & 0xE0) == 0xC0)
		return 2;
	return 1;
}

// "Borrowed" from ImGui source
static inline int ImTextCharToUtf8(char* buf, int buf_size, unsigned int c)
{
	if (c < 0x80)
	{
		buf[0] = (char)c;
		return 1;
	}
	if (c < 0x800)
	{
		if (buf_size < 2) return 0;
		buf[0] = (char)(0xc0 + (c >> 6));
		buf[1] = (char)(0x80 + (c & 0x3f));
		return 2;
	}
	if (c >= 0xdc00 && c < 0xe000)
	{
		return 0;
	}
	if (c >= 0xd800 && c < 0xdc00)
	{
		if (buf_size < 4) return 0;
		buf[0] = (char)(0xf0 + (c >> 18));
		buf[1] = (char)(0x80 + ((c >> 12) & 0x3f));
		buf[2] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[3] = (char)(0x80 + ((c) & 0x3f));
		return 4;
	}
	//else if (c < 0x10000)
	{
		if (buf_size < 3) return 0;
		buf[0] = (char)(0xe0 + (c >> 12));
		buf[1] = (char)(0x80 + ((c >> 6) & 0x3f));
		buf[2] = (char)(0x80 + ((c) & 0x3f));
		return 3;
	}
}

static inline bool CharIsWordChar(char ch)
{
	int sizeInBytes = UTF8CharLength(ch);
	return sizeInBytes > 1 ||
		(ch >= 'a' && ch <= 'z') ||
		(ch >= 'A' && ch <= 'Z') ||
		(ch >= '0' && ch <= '9') ||
		ch == '_';
}

// ------------------------------------ //
// ------------- Internal ------------- //


// ---------- Editor state functions --------- //

void TextEditor::EditorState::AddCursor()
{
	// vector is never resized to smaller size, mCurrentCursor points to last available cursor in vector
	mCurrentCursor++;
	mCursors.resize(mCurrentCursor + 1);
	mLastAddedCursor = mCurrentCursor;
}

int TextEditor::EditorState::GetLastAddedCursorIndex()
{
	return mLastAddedCursor > mCurrentCursor ? 0 : mLastAddedCursor;
}

void TextEditor::EditorState::SortCursorsFromTopToBottom()
{
	Coordinates lastAddedCursorPos = mCursors[GetLastAddedCursorIndex()].mInteractiveEnd;
	std::sort(mCursors.begin(), mCursors.begin() + (mCurrentCursor + 1), [](const Cursor& a, const Cursor& b) -> bool
		{
			return a.GetSelectionStart() < b.GetSelectionStart();
		});
	// update last added cursor index to be valid after sort
	for (int c = mCurrentCursor; c > -1; c--)
		if (mCursors[c].mInteractiveEnd == lastAddedCursorPos)
			mLastAddedCursor = c;
}

// ---------- Undo record functions --------- //

TextEditor::UndoRecord::UndoRecord(const std::vector<UndoOperation>& aOperations,
	TextEditor::EditorState& aBefore, TextEditor::EditorState& aAfter)
{
	mOperations = aOperations;
	mBefore = aBefore;
	mAfter = aAfter;
	for (const UndoOperation& o : mOperations)
		IM_ASSERT(o.mStart <= o.mEnd);
}

void TextEditor::UndoRecord::Undo(TextEditor* aEditor)
{
	for (int i = int(mOperations.size()) - 1; i > -1; i--)
	{
		const UndoOperation& operation = mOperations[i];
		if (!operation.mText.empty())
		{
			switch (operation.mType)
			{
			case UndoOperationType::Delete:
			{
				auto start = operation.mStart;
				aEditor->InsertTextAt(start, operation.mText.c_str());
				aEditor->Colorize(operation.mStart.mLine - 1, operation.mEnd.mLine - operation.mStart.mLine + 2);
				break;
			}
			case UndoOperationType::Add:
			{
				aEditor->DeleteRange(operation.mStart, operation.mEnd);
				aEditor->Colorize(operation.mStart.mLine - 1, operation.mEnd.mLine - operation.mStart.mLine + 2);
				break;
			}
			}
		}
	}

	aEditor->mState = mBefore;
	aEditor->EnsureCursorVisible();
}

void TextEditor::UndoRecord::Redo(TextEditor* aEditor)
{
	for (int i = 0; i < mOperations.size(); i++)
	{
		const UndoOperation& operation = mOperations[i];
		if (!operation.mText.empty())
		{
			switch (operation.mType)
			{
			case UndoOperationType::Delete:
			{
				aEditor->DeleteRange(operation.mStart, operation.mEnd);
				aEditor->Colorize(operation.mStart.mLine - 1, operation.mEnd.mLine - operation.mStart.mLine + 1);
				break;
			}
			case UndoOperationType::Add:
			{
				auto start = operation.mStart;
				aEditor->InsertTextAt(start, operation.mText.c_str());
				aEditor->Colorize(operation.mStart.mLine - 1, operation.mEnd.mLine - operation.mStart.mLine + 1);
				break;
			}
			}
		}
	}

	aEditor->mState = mAfter;
	aEditor->EnsureCursorVisible();
}

std::string TextEditor::GetText(const Coordinates& aStart, const Coordinates& aEnd) const
{
	IM_ASSERT(aStart < aEnd);

	std::string result;
	auto lstart = aStart.mLine;
	auto lend = aEnd.mLine;
	auto istart = GetCharacterIndexR(aStart);
	auto iend = GetCharacterIndexR(aEnd);
	size_t s = 0;

	for (auto i = lstart; i < lend; i++)
		s += mLines[i].size();

	result.reserve(s + s / 8);

	while (istart < iend || lstart < lend)
	{
		if (lstart >= int(mLines.size()))
			break;

		auto& line = mLines[lstart];
		if (istart < int(line.size()))
		{
			result += line[istart].mChar;
			istart++;
		}
		else
		{
			istart = 0;
			++lstart;
			result += '\n';
		}
	}

	return result;
}

std::string TextEditor::GetAllSelectedText() const
{
	std::string result;

	for (int c = 0; c <= mState.mCurrentCursor; c++)
		if (mState.mCursors[c].HasSelection())
		{
			if (result.length() != 0)
				result += '\n';

			result += GetSelectedText(c);
		}

	return result;
}

std::string TextEditor::GetSelectedText(int aCursor) const
{
	if (aCursor == -1)
		aCursor = mState.mCurrentCursor;

	return GetText(mState.mCursors[aCursor].GetSelectionStart(), mState.mCursors[aCursor].GetSelectionEnd());
}

void TextEditor::SetCursorPosition(const Coordinates& aPosition, int aCursor, bool aClearSelection)
{
	if (aCursor == -1)
		aCursor = mState.mCurrentCursor;

	mCursorPositionChanged = true;
	if (aClearSelection)
		mState.mCursors[aCursor].mInteractiveStart = aPosition;
	if (mState.mCursors[aCursor].mInteractiveEnd != aPosition)
	{
		mState.mCursors[aCursor].mInteractiveEnd = aPosition;
		EnsureCursorVisible();
	}
}

int TextEditor::InsertTextAt(Coordinates& /* inout */ aWhere, const char* aValue)
{
	int cindex = GetCharacterIndexR(aWhere);
	int totalLines = 0;
	while (*aValue != '\0')
	{
		if (*aValue == '\r')
		{
			// skip
			++aValue;
		}
		else if (*aValue == '\n')
		{
			if (cindex < int(mLines[aWhere.mLine].size()))
			{
				auto& newLine = InsertLine(aWhere.mLine + 1);
				auto& line = mLines[aWhere.mLine];
				AddGlyphsToLine(aWhere.mLine + 1, 0, line.begin() + cindex, line.end());
				RemoveGlyphsFromLine(aWhere.mLine, cindex);
			}
			else
			{
				InsertLine(aWhere.mLine + 1);
			}
			++aWhere.mLine;
			aWhere.mColumn = 0;
			cindex = 0;
			++totalLines;
			++aValue;
		}
		else
		{
			auto& line = mLines[aWhere.mLine];
			auto d = UTF8CharLength(*aValue);
			while (d-- > 0 && *aValue != '\0')
				AddGlyphToLine(aWhere.mLine, cindex++, Glyph(*aValue++, PaletteIndex::Default));
			aWhere.mColumn = GetCharacterColumn(aWhere.mLine, cindex);
		}
	}

	return totalLines;
}

void TextEditor::InsertTextAtCursor(const char* aValue, int aCursor)
{
	if (aValue == nullptr)
		return;
	if (aCursor == -1)
		aCursor = mState.mCurrentCursor;

	auto pos = GetActualCursorCoordinates(aCursor);
	auto start = std::min(pos, mState.mCursors[aCursor].GetSelectionStart());
	int totalLines = pos.mLine - start.mLine;

	totalLines += InsertTextAt(pos, aValue);

	SetCursorPosition(pos, aCursor);
	Colorize(start.mLine - 1, totalLines + 2);
}

bool TextEditor::Move(int& aLine, int& aCharIndex, bool aLeft, bool aLockLine) const
{
	// assumes given char index is not in the middle of utf8 sequence
	// char index can be line.length()

	// invalid line
	if (aLine >= mLines.size())
		return false;

	if (aLeft)
	{
		if (aCharIndex == 0)
		{
			if (aLockLine || aLine == 0)
				return false;
			aLine--;
			aCharIndex = int(mLines[aLine].size());
		}
		else
		{
			aCharIndex--;
			while (aCharIndex > 0 && IsUTFSequence(mLines[aLine][aCharIndex].mChar))
				aCharIndex--;
		}
	}
	else // right
	{
		if (aCharIndex == mLines[aLine].size())
		{
			if (aLockLine || aLine == mLines.size() - 1)
				return false;
			aLine++;
			aCharIndex = 0;
		}
		else
		{
			int seqLength = UTF8CharLength(mLines[aLine][aCharIndex].mChar);
			aCharIndex = std::min(aCharIndex + seqLength, int(mLines[aLine].size()));
		}
	}
	return true;
}

void TextEditor::MoveCharIndexAndColumn(int aLine, int& aCharIndex, int& aColumn) const
{
	IM_ASSERT(aLine < mLines.size());
	IM_ASSERT(aCharIndex < mLines[aLine].size());
	char c = mLines[aLine][aCharIndex].mChar;
	aCharIndex += UTF8CharLength(c);
	if (c == '\t')
		aColumn = (aColumn / mTabSize) * mTabSize + mTabSize;
	else
		aColumn++;
}

void TextEditor::MoveCoords(Coordinates& aCoords, MoveDirection aDirection, bool aWordMode, int aLineCount) const
{
	int charIndex = GetCharacterIndexR(aCoords);
	int lineIndex = aCoords.mLine;
	switch (aDirection)
	{
	case MoveDirection::Right:
		if (charIndex >= mLines[lineIndex].size())
		{
			if (lineIndex < mLines.size() - 1)
			{
				aCoords.mLine = std::max(0, std::min(int(mLines.size()) - 1, lineIndex + 1));
				aCoords.mColumn = 0;
			}
		}
		else
		{
			Move(lineIndex, charIndex);
			int oneStepRightColumn = GetCharacterColumn(lineIndex, charIndex);
			if (aWordMode)
			{
				aCoords = FindWordEnd(aCoords);
				aCoords.mColumn = std::max(aCoords.mColumn, oneStepRightColumn);
			}
			else
				aCoords.mColumn = oneStepRightColumn;
		}
		break;
	case MoveDirection::Left:
		if (charIndex == 0)
		{
			if (lineIndex > 0)
			{
				aCoords.mLine = lineIndex - 1;
				aCoords.mColumn = GetLineMaxColumn(aCoords.mLine);
			}
		}
		else
		{
			Move(lineIndex, charIndex, true);
			aCoords.mColumn = GetCharacterColumn(lineIndex, charIndex);
			if (aWordMode)
				aCoords = FindWordStart(aCoords);
		}
		break;
	case MoveDirection::Up:
		aCoords.mLine = std::max(0, lineIndex - aLineCount);
		break;
	case MoveDirection::Down:
		aCoords.mLine = std::max(0, std::min(int(mLines.size()) - 1, lineIndex + aLineCount));
		break;
	}
}

void TextEditor::MoveUp(int aAmount, bool aSelect)
{
	for (int c = 0; c <= mState.mCurrentCursor; c++)
	{
		Coordinates newCoords = mState.mCursors[c].mInteractiveEnd;
		MoveCoords(newCoords, MoveDirection::Up, false, aAmount);
		SetCursorPosition(newCoords, c, !aSelect);
	}
	EnsureCursorVisible();
}

void TextEditor::MoveDown(int aAmount, bool aSelect)
{
	for (int c = 0; c <= mState.mCurrentCursor; c++)
	{
		IM_ASSERT(mState.mCursors[c].mInteractiveEnd.mColumn >= 0);
		Coordinates newCoords = mState.mCursors[c].mInteractiveEnd;
		MoveCoords(newCoords, MoveDirection::Down, false, aAmount);
		SetCursorPosition(newCoords, c, !aSelect);
	}
	EnsureCursorVisible();
}

void TextEditor::MoveLeft(bool aSelect, bool aWordMode)
{
	if (AnyCursorHasSelection() && !aSelect && !aWordMode)
	{
		for (int c = 0; c <= mState.mCurrentCursor; c++)
			SetCursorPosition(mState.mCursors[c].GetSelectionStart(), c);
	}
	else
	{
		for (int c = 0; c <= mState.mCurrentCursor; c++)
		{
			Coordinates newCoords = mState.mCursors[c].mInteractiveEnd;
			MoveCoords(newCoords, MoveDirection::Left, aWordMode);
			SetCursorPosition(newCoords, c, !aSelect);
		}
	}
	EnsureCursorVisible();
}

void TextEditor::MoveRight(bool aSelect, bool aWordMode)
{
	if (AnyCursorHasSelection() && !aSelect && !aWordMode)
	{
		for (int c = 0; c <= mState.mCurrentCursor; c++)
			SetCursorPosition(mState.mCursors[c].GetSelectionEnd(), c);
	}
	else
	{
		for (int c = 0; c <= mState.mCurrentCursor; c++)
		{
			Coordinates newCoords = mState.mCursors[c].mInteractiveEnd;
			MoveCoords(newCoords, MoveDirection::Right, aWordMode);
			SetCursorPosition(newCoords, c, !aSelect);
		}
	}
	EnsureCursorVisible();
}

void TextEditor::MoveTop(bool aSelect)
{
	SetCursorPosition(Coordinates(0, 0), mState.mCurrentCursor, !aSelect);
}

void TextEditor::TextEditor::MoveBottom(bool aSelect)
{
	int maxLine = int(mLines.size()) - 1;
	Coordinates newPos = Coordinates(maxLine, GetLineMaxColumn(maxLine));
	SetCursorPosition(newPos, mState.mCurrentCursor, !aSelect);
}

void TextEditor::MoveHome(bool aSelect)
{
	for (int c = 0; c <= mState.mCurrentCursor; c++)
		SetCursorPosition(Coordinates(mState.mCursors[c].mInteractiveEnd.mLine, 0), c, !aSelect);
}

void TextEditor::MoveEnd(bool aSelect)
{
	for (int c = 0; c <= mState.mCurrentCursor; c++)
	{
		int lindex = mState.mCursors[c].mInteractiveEnd.mLine;
		SetCursorPosition(Coordinates(lindex, GetLineMaxColumn(lindex)), c, !aSelect);
	}
}

void TextEditor::EnterCharacter(ImWchar aChar)
{
	UndoRecord u;
	u.mBefore = mState;
	bool hasSelection = AnyCursorHasSelection();

	if (hasSelection && mCompletePairedGlyphs && (aChar == '{' || aChar == '[' || aChar == '(' || aChar == '"' || aChar == '\''))
	{
		auto closer = aChar == '{' ? '}' : (aChar == '[' ? ']' : (aChar == '(' ? ')' : aChar));

		for (int c = mState.mCurrentCursor; c > -1; c--)
		{
			auto coord = GetActualCursorCoordinates(c);
			auto cindex = GetCharacterIndexR(coord);
			AddGlyphToLine(coord.mLine, cindex++, Glyph(closer, PaletteIndex::Default));

			UndoOperation added;
			added.mType = UndoOperationType::Add;
			added.mStart = coord;
			added.mEnd = Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex));
			auto end = Coordinates(added.mEnd.mLine, added.mEnd.mColumn + 1);

			char buf[2];
			buf[0] = closer;
			buf[1] = 0;
			added.mText = buf;
			u.mOperations.push_back(added);

			coord = GetActualCursorCoordinates(c, true);
			cindex = GetCharacterIndexR(coord);
			AddGlyphToLine(coord.mLine, cindex++, Glyph(aChar, PaletteIndex::Default));

			added.mStart = coord;
			added.mEnd = Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex));

			buf[0] = aChar;
			added.mText = buf;
			u.mOperations.push_back(added);

			SetCursorPosition(end, c);
			Colorize(coord.mLine, end.mLine - coord.mLine + 1);
		}


		ClearSelections();
		u.mAfter = mState;
		AddUndo(u);
		return;
	}

	if (hasSelection)
	{
		for (int c = mState.mCurrentCursor; c > -1; c--)
		{
			u.mOperations.push_back({ GetSelectedText(c), mState.mCursors[c].GetSelectionStart(), mState.mCursors[c].GetSelectionEnd(), UndoOperationType::Delete });
			DeleteSelection(c);
		}
	}

	std::vector<Coordinates> coords;

	for (int c = mState.mCurrentCursor; c > -1; c--) // order important here for typing \n in the same line at the same time
	{
		auto coord = GetActualCursorCoordinates(c);
		coords.push_back(coord);
		UndoOperation added;
		added.mType = UndoOperationType::Add;
		added.mStart = coord;

		// special handling for newline
		if (aChar == '\n')
		{
			std::string insert = "\n";
			std::string whitespace;
			int column = 0;

			if (mAutoIndent)
			{
				auto& line = mLines[coord.mLine];
				auto cindex = GetCharacterIndexR(coord);
				char previousChar = cindex > 0 ? line[cindex - 1].mChar : 0;
				char nextChar = cindex <= int(line.size()) - 1 ? line[cindex].mChar : 0;

				// determine whitespace at start of old line and add it to the new line
				for (int i = 0; i < line.size() && std::isblank(line[i].mChar); ++i)
					whitespace += line[i].mChar;

				insert += whitespace;
				column = int(whitespace.size());

				// handle special cases
				if (previousChar == '[' || previousChar == '{')
				{
					// add to an existing block
					insert += "\t";
					column++;

					if ((previousChar == '[' && nextChar == ']') || (previousChar == '{' && nextChar == '}'))
						// open a new block
						insert += "\n" + whitespace;
				}

				if (std::isblank(nextChar))
				{
					// remove extra whitespaces
					std::string deleted;

					while (cindex <= int(line.size()) - 1 && std::isblank(line[cindex].mChar))
						deleted += line[cindex++].mChar;

					auto end = Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex));
					u.mOperations.push_back({deleted, coord, end, UndoOperationType::Delete});
					DeleteRange(coord, end);
				}
			}

			// insert new text
			Coordinates insertEnd = coord;
			InsertTextAt(insertEnd, insert.c_str());
			added.mText = insert;
			added.mEnd = insertEnd;
			u.mOperations.push_back(added);

			// set new cursor position
			SetCursorPosition(Coordinates(coord.mLine + 1, GetCharacterColumn(coord.mLine + 1, column)), c);
		}
		else
		{
			char buf[7];
			int e = ImTextCharToUtf8(buf, 7, aChar);
			if (e > 0)
			{
				buf[e] = '\0';
				auto& line = mLines[coord.mLine];
				auto cindex = GetCharacterIndexR(coord);

				if (mOverwrite && cindex < int(line.size()))
				{
					auto d = UTF8CharLength(line[cindex].mChar);

					UndoOperation removed;
					removed.mType = UndoOperationType::Delete;
					removed.mStart = mState.mCursors[c].mInteractiveEnd;
					removed.mEnd = Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex + d));

					while (d-- > 0 && cindex < int(line.size()))
					{
						removed.mText += line[cindex].mChar;
						RemoveGlyphsFromLine(coord.mLine, cindex, cindex + 1);
					}
					u.mOperations.push_back(removed);
				}

				// automatically complete paired glyphs when feature is turned on
				if (mCompletePairedGlyphs && (aChar == '{' || aChar == '[' || aChar == '('))
				{
					auto closer = aChar == '{' ? '}' : (aChar == '[' ? ']' : ')');

					AddGlyphToLine(coord.mLine, cindex++, Glyph(aChar, PaletteIndex::Default));
					AddGlyphToLine(coord.mLine, cindex++, Glyph(closer, PaletteIndex::Default));

					buf[0] = aChar;
					buf[1] = closer;
					buf[2] = 0;

					added.mText = buf;
					added.mEnd = Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex));
					SetCursorPosition(Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex - 1)), c);
				}
				// if the glyph is closing and the following character is the same, just skip it in complete pair mode
				else if (mCompletePairedGlyphs && (aChar == '}' || aChar == ']' || aChar == ')'))
				{
					if (line.size() > cindex && line[cindex].mChar == aChar)
						SetCursorPosition(Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, ++cindex)), c);
					else
					{
						AddGlyphToLine(coord.mLine, cindex++, Glyph(aChar, PaletteIndex::Default));
						buf[0] = aChar;
						buf[1] = 0;
						added.mText = buf;
						added.mEnd = Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex));
						SetCursorPosition(Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex)), c);
					}
				}
				// special handling for "quoted" glyph pairs
				else if (mCompletePairedGlyphs && (aChar == '"' || aChar == '\''))
				{
					if (line.size() > cindex && line[cindex].mChar == aChar)
						SetCursorPosition(Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, ++cindex)), c);
					else
					{
						AddGlyphToLine(coord.mLine, cindex++, Glyph(aChar, PaletteIndex::Default));
						AddGlyphToLine(coord.mLine, cindex++, Glyph(aChar, PaletteIndex::Default));

						buf[0] = aChar;
						buf[1] = aChar;
						buf[2] = 0;

						added.mText = buf;
						added.mEnd = Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex));
						SetCursorPosition(Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex - 1)), c);
					}
				}
				else
				{
					for (auto p = buf; *p != '\0'; p++, ++cindex)
						AddGlyphToLine(coord.mLine, cindex, Glyph(*p, PaletteIndex::Default));
					added.mText = buf;

					SetCursorPosition(Coordinates(coord.mLine, GetCharacterColumn(coord.mLine, cindex)), c);
					added.mEnd = GetActualCursorCoordinates(c);
				}

				u.mOperations.push_back(added);
			}
		}
	}

	u.mAfter = mState;
	AddUndo(u);

	for (const auto& coord : coords)
		Colorize(coord.mLine - 1, 3);
	EnsureCursorVisible();
}

void TextEditor::Backspace(bool aWordMode)
{
	if (AnyCursorHasSelection())
		Delete(aWordMode);
	else
	{
		EditorState stateBeforeDeleting = mState;
		MoveLeft(true, aWordMode);
		if (!AllCursorsHaveSelection()) // can't do backspace if any cursor at {0,0}
		{
			if (AnyCursorHasSelection())
				MoveRight();
			return;
		}

		OnCursorPositionChanged(); // might combine cursors
		Delete(aWordMode, &stateBeforeDeleting);
	}
}

void TextEditor::Delete(bool aWordMode, const EditorState* aEditorState)
{
	if (AnyCursorHasSelection())
	{
		UndoRecord u;
		u.mBefore = aEditorState == nullptr ? mState : *aEditorState;
		for (int c = mState.mCurrentCursor; c > -1; c--)
		{
			if (mState.mCursors[c].HasSelection())
			{
				u.mOperations.push_back({ GetSelectedText(c), mState.mCursors[c].GetSelectionStart(), mState.mCursors[c].GetSelectionEnd(), UndoOperationType::Delete });
				DeleteSelection(c);
			}
		}
		u.mAfter = mState;
		AddUndo(u);
	}
	else
	{
		EditorState stateBeforeDeleting = mState;
		MoveRight(true, aWordMode);
		if (!AllCursorsHaveSelection()) // can't do delete if any cursor at end of last line
		{
			if (AnyCursorHasSelection())
				MoveLeft();
			return;
		}

		OnCursorPositionChanged(); // might combine cursors
		Delete(aWordMode, &stateBeforeDeleting);
	}
}

void TextEditor::SetSelection(Coordinates aStart, Coordinates aEnd, int aCursor)
{
	if (aCursor == -1)
		aCursor = mState.mCurrentCursor;

	Coordinates minCoords = Coordinates(0, 0);
	int maxLine = int(mLines.size()) - 1;
	Coordinates maxCoords = Coordinates(maxLine, GetLineMaxColumn(maxLine));
	if (aStart < minCoords)
		aStart = minCoords;
	else if (aStart > maxCoords)
		aStart = maxCoords;
	if (aEnd < minCoords)
		aEnd = minCoords;
	else if (aEnd > maxCoords)
		aEnd = maxCoords;

	mState.mCursors[aCursor].mInteractiveStart = aStart;
	SetCursorPosition(aEnd, aCursor, false);
}

void TextEditor::SetSelection(int aStartLine, int aStartChar, int aEndLine, int aEndChar, int aCursor)
{
	Coordinates startCoords = { aStartLine, GetCharacterColumn(aStartLine, aStartChar) };
	Coordinates endCoords = { aEndLine, GetCharacterColumn(aEndLine, aEndChar) };
	SetSelection(startCoords, endCoords, aCursor);
}

void TextEditor::SelectNextOccurrenceOf(const char* aText, int aTextSize, int aCursor, bool aCaseSensitive, bool aWholeWord)
{
	if (aCursor == -1)
		aCursor = mState.mCurrentCursor;
	Coordinates nextStart, nextEnd;
	FindNextOccurrence(aText, aTextSize, mState.mCursors[aCursor].mInteractiveEnd, nextStart, nextEnd, aCaseSensitive, aWholeWord);
	SetSelection(nextStart, nextEnd, aCursor);
	EnsureCursorVisible(aCursor, true);
}

void TextEditor::AddCursorForNextOccurrence(bool aCaseSensitive, bool aWholeWord)
{
	const Cursor& currentCursor = mState.mCursors[mState.GetLastAddedCursorIndex()];
	if (currentCursor.GetSelectionStart() == currentCursor.GetSelectionEnd())
		return;

	std::string selectionText = GetText(currentCursor.GetSelectionStart(), currentCursor.GetSelectionEnd());
	Coordinates nextStart, nextEnd;
	if (!FindNextOccurrence(selectionText.c_str(), int(selectionText.length()), currentCursor.GetSelectionEnd(), nextStart, nextEnd, aCaseSensitive, aWholeWord))
		return;

	mState.AddCursor();
	SetSelection(nextStart, nextEnd, mState.mCurrentCursor);
	mState.SortCursorsFromTopToBottom();
	MergeCursorsIfPossible();
	EnsureCursorVisible(-1, true);
}

bool TextEditor::FindNextOccurrence(const char* aText, int aTextSize, const Coordinates& aFrom, Coordinates& outStart, Coordinates& outEnd, bool aCaseSensitive, bool aWholeWord)
{
	IM_ASSERT(aTextSize > 0);
	bool fmatches = false;
	int fline, ifline;
	int findex, ifindex;

	ifline = fline = aFrom.mLine;
	ifindex = findex = GetCharacterIndexR(aFrom);

	while (true)
	{
		{ // match function
			int lineOffset = 0;
			int currentCharIndex = findex;
			int i = 0;
			for (; i < aTextSize; i++)
			{
				if (currentCharIndex == mLines[fline + lineOffset].size())
				{
					if (aText[i] == '\n' && fline + lineOffset + 1 < mLines.size())
					{
						currentCharIndex = 0;
						lineOffset++;
					}
					else
						break;
				}
				else
				{
					char toCompareA = mLines[fline + lineOffset][currentCharIndex].mChar;
					char toCompareB = aText[i];
					toCompareA = (!aCaseSensitive && toCompareA >= 'A' && toCompareA <= 'Z') ? toCompareA - 'A' + 'a' : toCompareA;
					toCompareB = (!aCaseSensitive && toCompareB >= 'A' && toCompareB <= 'Z') ? toCompareB - 'A' + 'a' : toCompareB;
					if (toCompareA != toCompareB)
						break;
					else
						currentCharIndex++;
				}
			}
			if (i == aTextSize)
			{
				Coordinates start = { fline, GetCharacterColumn(fline, findex) };
				Coordinates end = { fline + lineOffset, GetCharacterColumn(fline + lineOffset, currentCharIndex) };

				if (aTextSize == 4) {
					int w = 4;
				}
				if (!aWholeWord || IsWholeWord(start, end))
				{
					outStart = start;
					outEnd = end;
					return true;
				}
			}
		}

		// move forward
		if (findex == mLines[fline].size()) // need to consider line breaks
		{
			if (fline == mLines.size() - 1)
			{
				fline = 0;
				findex = 0;
			}
			else
			{
				fline++;
				findex = 0;
			}
		}
		else
			findex++;

		// detect complete scan
		if (findex == ifindex && fline == ifline)
			return false;
	}

	return false;
}

bool TextEditor::FindMatchingBracket(int aLine, int aCharIndex, Coordinates& out)
{
	static const std::unordered_map<char, char> openToCloseChar = {
		{'{', '}'},
		{'(' , ')'},
		{'[' , ']'}
	};

	static const std::unordered_map<char, char> closeToOpenChar = {
		{'}', '{'},
		{')' , '('},
		{']' , '['}
	};

	if (aLine > mLines.size() - 1)
		return false;
	int maxCharIndex = int(mLines[aLine].size()) - 1;
	if (aCharIndex > maxCharIndex)
		return false;

	int currentLine = aLine;
	int currentCharIndex = aCharIndex;
	int counter = 1;
	if (closeToOpenChar.find(mLines[aLine][aCharIndex].mChar) != closeToOpenChar.end())
	{
		char closeChar = mLines[aLine][aCharIndex].mChar;
		char openChar = closeToOpenChar.at(closeChar);
		while (Move(currentLine, currentCharIndex, true))
		{
			if (currentCharIndex < mLines[currentLine].size())
			{
				char currentChar = mLines[currentLine][currentCharIndex].mChar;
				if (currentChar == openChar)
				{
					counter--;
					if (counter == 0)
					{
						out = { currentLine, GetCharacterColumn(currentLine, currentCharIndex) };
						return true;
					}
				}
				else if (currentChar == closeChar)
					counter++;
			}
		}
	}
	else if (openToCloseChar.find(mLines[aLine][aCharIndex].mChar) != openToCloseChar.end())
	{
		char openChar = mLines[aLine][aCharIndex].mChar;
		char closeChar = openToCloseChar.at(openChar);
		while (Move(currentLine, currentCharIndex))
		{
			if (currentCharIndex < mLines[currentLine].size())
			{
				char currentChar = mLines[currentLine][currentCharIndex].mChar;
				if (currentChar == closeChar)
				{
					counter--;
					if (counter == 0)
					{
						out = { currentLine, GetCharacterColumn(currentLine, currentCharIndex) };
						return true;
					}
				}
				else if (currentChar == openChar)
					counter++;
			}
		}
	}
	return false;
}

void TextEditor::ChangeCurrentLinesIndentation(bool aIncrease)
{
	UndoRecord u;
	u.mBefore = mState;

	// process all the cursors
	for (int c = mState.mCurrentCursor; c > -1; c--)
	{
		// get cursor selection
		auto cursorStart = mState.mCursors[c].GetSelectionStart();
		auto cursorEnd = mState.mCursors[c].GetSelectionEnd();
		Coordinates newCursorStart = cursorStart;
		Coordinates newCursorEnd = cursorEnd;

		// process all lines in a single cursor
		for (int currentLine = cursorEnd.mLine; currentLine >= cursorStart.mLine; currentLine--)
		{
			if (Coordinates{currentLine, 0} != cursorEnd || cursorEnd == cursorStart)
			{
				if (aIncrease)
				{
					// indent the current selection
					if (mLines[currentLine].size() > 0)
					{
						Coordinates lineStart = { currentLine, 0 };
						Coordinates insertionEnd = lineStart;
						InsertTextAt(insertionEnd, "\t"); // updates insertionEnd
						u.mOperations.push_back({ "\t", lineStart, insertionEnd, UndoOperationType::Add });
						Colorize(lineStart.mLine, 1);

						// determime have many columns the cursor start/end gets indented
						if (currentLine == cursorStart.mLine)
							newCursorStart.mColumn += cursorStart.mColumn ? mTabSize : 0;

						if (currentLine == cursorEnd.mLine)
							newCursorEnd.mColumn += mTabSize;
					}
				}

				else
				{
					// de-indent the current selection
					int column = 0;
					int charIndex = 0;

					// determine how many whitespace are available at the start with a max of 4 columns
					while (column < 4 && std::isblank(mLines[currentLine][charIndex].mChar))
					{
						column += mLines[currentLine][charIndex].mChar == '\t' ? TabSizeAtColumn(column) : 1;
						charIndex++;
					}

					Coordinates start = { currentLine, 0 };
					Coordinates end = { currentLine, GetCharacterColumn(currentLine, charIndex) };

					// see if anything should be deleted
					if (start != end)
					{
						u.mOperations.push_back({ GetText(start, end), start, end, UndoOperationType::Delete });
						DeleteRange(start, end);
						Colorize(currentLine, 1);

						// determime have many columns the cursor start/end gets de-indented
						if (currentLine == cursorStart.mLine)
							newCursorStart.mColumn -= end.mColumn;
							newCursorStart.mColumn = std::max(newCursorStart.mColumn, 0);

						if (currentLine == cursorEnd.mLine)
							newCursorEnd.mColumn -= end.mColumn;
					}
				}
			}
		}

		// update selection
		SetSelection(newCursorStart, newCursorEnd, c);
	}

	if (u.mOperations.size() > 0)
		AddUndo(u);
}

void TextEditor::MoveUpCurrentLines()
{
	UndoRecord u;
	u.mBefore = mState;

	std::set<int> affectedLines;
	int minLine = -1;
	int maxLine = -1;
	for (int c = mState.mCurrentCursor; c > -1; c--) // cursors are expected to be sorted from top to bottom
	{
		for (int currentLine = mState.mCursors[c].GetSelectionEnd().mLine; currentLine >= mState.mCursors[c].GetSelectionStart().mLine; currentLine--)
		{
			if (Coordinates{ currentLine, 0 } == mState.mCursors[c].GetSelectionEnd() && mState.mCursors[c].GetSelectionEnd() != mState.mCursors[c].GetSelectionStart()) // when selection ends at line start
				continue;
			affectedLines.insert(currentLine);
			minLine = minLine == -1 ? currentLine : (currentLine < minLine ? currentLine : minLine);
			maxLine = maxLine == -1 ? currentLine : (currentLine > maxLine ? currentLine : maxLine);
		}
	}
	if (minLine == 0) // can't move up anymore
		return;

	Coordinates start = { minLine - 1, 0 };
	Coordinates end = { maxLine, GetLineMaxColumn(maxLine) };
	u.mOperations.push_back({ GetText(start, end), start, end, UndoOperationType::Delete });

	for (int line : affectedLines) // lines should be sorted here
		std::swap(mLines[line - 1], mLines[line]);
	for (int c = mState.mCurrentCursor; c > -1; c--)
	{
		mState.mCursors[c].mInteractiveStart.mLine -= 1;
		mState.mCursors[c].mInteractiveEnd.mLine -= 1;
		// no need to set mCursorPositionChanged as cursors will remain sorted
	}

	end = { maxLine, GetLineMaxColumn(maxLine) }; // this line is swapped with line above, need to find new max column
	u.mOperations.push_back({ GetText(start, end), start, end, UndoOperationType::Add });
	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::MoveDownCurrentLines()
{
	UndoRecord u;
	u.mBefore = mState;

	std::set<int> affectedLines;
	int minLine = -1;
	int maxLine = -1;
	for (int c = 0; c <= mState.mCurrentCursor; c++) // cursors are expected to be sorted from top to bottom
	{
		for (int currentLine = mState.mCursors[c].GetSelectionEnd().mLine; currentLine >= mState.mCursors[c].GetSelectionStart().mLine; currentLine--)
		{
			if (Coordinates{ currentLine, 0 } == mState.mCursors[c].GetSelectionEnd() && mState.mCursors[c].GetSelectionEnd() != mState.mCursors[c].GetSelectionStart()) // when selection ends at line start
				continue;
			affectedLines.insert(currentLine);
			minLine = minLine == -1 ? currentLine : (currentLine < minLine ? currentLine : minLine);
			maxLine = maxLine == -1 ? currentLine : (currentLine > maxLine ? currentLine : maxLine);
		}
	}
	if (maxLine == mLines.size() - 1) // can't move down anymore
		return;

	Coordinates start = { minLine, 0 };
	Coordinates end = { maxLine + 1, GetLineMaxColumn(maxLine + 1)};
	u.mOperations.push_back({ GetText(start, end), start, end, UndoOperationType::Delete });

	std::set<int>::reverse_iterator rit;
	for (rit = affectedLines.rbegin(); rit != affectedLines.rend(); rit++) // lines should be sorted here
		std::swap(mLines[*rit + 1], mLines[*rit]);
	for (int c = mState.mCurrentCursor; c > -1; c--)
	{
		mState.mCursors[c].mInteractiveStart.mLine += 1;
		mState.mCursors[c].mInteractiveEnd.mLine += 1;
		// no need to set mCursorPositionChanged as cursors will remain sorted
	}

	end = { maxLine + 1, GetLineMaxColumn(maxLine + 1) }; // this line is swapped with line below, need to find new max column
	u.mOperations.push_back({ GetText(start, end), start, end, UndoOperationType::Add });
	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::ToggleLineComment()
{
	if (mLanguageDefinition == nullptr)
		return;
	const std::string& commentString = mLanguageDefinition->mSingleLineComment;

	UndoRecord u;
	u.mBefore = mState;

	bool shouldAddComment = false;
	std::unordered_set<int> affectedLines;
	for (int c = mState.mCurrentCursor; c > -1; c--)
	{
		for (int currentLine = mState.mCursors[c].GetSelectionEnd().mLine; currentLine >= mState.mCursors[c].GetSelectionStart().mLine; currentLine--)
		{
			if (Coordinates{ currentLine, 0 } == mState.mCursors[c].GetSelectionEnd() && mState.mCursors[c].GetSelectionEnd() != mState.mCursors[c].GetSelectionStart()) // when selection ends at line start
				continue;
			affectedLines.insert(currentLine);
			int currentIndex = 0;
			while (currentIndex < mLines[currentLine].size() && std::isblank(mLines[currentLine][currentIndex].mChar == ' ')) currentIndex++;
			if (currentIndex == mLines[currentLine].size())
				continue;
			int i = 0;
			while (i < commentString.length() && currentIndex + i < mLines[currentLine].size() && mLines[currentLine][currentIndex + i].mChar == commentString[i]) i++;
			bool matched = i == commentString.length();
			shouldAddComment |= !matched;
		}
	}

	if (shouldAddComment)
	{
		for (int currentLine : affectedLines) // order doesn't matter as changes are not multiline
		{
			Coordinates lineStart = { currentLine, 0 };
			Coordinates insertionEnd = lineStart;
			InsertTextAt(insertionEnd, (commentString + ' ').c_str()); // sets insertion end
			u.mOperations.push_back({ (commentString + ' ') , lineStart, insertionEnd, UndoOperationType::Add });
			Colorize(lineStart.mLine, 1);
		}
	}
	else
	{
		for (int currentLine : affectedLines) // order doesn't matter as changes are not multiline
		{
			int currentIndex = 0;
			while (currentIndex < mLines[currentLine].size() && std::isblank(mLines[currentLine][currentIndex].mChar)) currentIndex++;
			if (currentIndex == mLines[currentLine].size())
				continue;
			int i = 0;
			while (i < commentString.length() && currentIndex + i < mLines[currentLine].size() && mLines[currentLine][currentIndex + i].mChar == commentString[i]) i++;
			bool matched = i == commentString.length();
			IM_ASSERT(matched);
			if (currentIndex + i < mLines[currentLine].size() && mLines[currentLine][currentIndex + i].mChar == ' ')
				i++;

			Coordinates start = { currentLine, GetCharacterColumn(currentLine, currentIndex) };
			Coordinates end = { currentLine, GetCharacterColumn(currentLine, currentIndex + i) };
			u.mOperations.push_back({ GetText(start, end) , start, end, UndoOperationType::Delete});
			DeleteRange(start, end);
			Colorize(currentLine, 1);
		}
	}

	u.mAfter = mState;
	AddUndo(u);
}

void TextEditor::RemoveCurrentLines()
{
	if (IsEmpty())
		return;

	UndoRecord u;
	u.mBefore = mState;

	if (AnyCursorHasSelection())
	{
		for (int c = mState.mCurrentCursor; c > -1; c--)
		{
			if (mState.mCursors[c].HasSelection())
			{
				u.mOperations.push_back({ GetSelectedText(c), mState.mCursors[c].GetSelectionStart(), mState.mCursors[c].GetSelectionEnd(), UndoOperationType::Delete });
				DeleteSelection(c);
			}
		}
	}
	MoveHome();
	OnCursorPositionChanged(); // might combine cursors

	for (int c = mState.mCurrentCursor; c > -1; c--)
	{
		int currentLine = mState.mCursors[c].mInteractiveEnd.mLine;
		int nextLine = currentLine + 1;
		int prevLine = currentLine - 1;

		Coordinates toDeleteStart, toDeleteEnd;
		if (mLines.size() > nextLine) // next line exists
		{
			toDeleteStart = Coordinates(currentLine, 0);
			toDeleteEnd = Coordinates(nextLine, 0);
			SetCursorPosition({ mState.mCursors[c].mInteractiveEnd.mLine, 0 }, c);
		}
		else if (prevLine > -1) // previous line exists
		{
			toDeleteStart = Coordinates(prevLine, GetLineMaxColumn(prevLine));
			toDeleteEnd = Coordinates(currentLine, GetLineMaxColumn(currentLine));
			SetCursorPosition({ prevLine, 0 }, c);
		}
		else
		{
			toDeleteStart = Coordinates(currentLine, 0);
			toDeleteEnd = Coordinates(currentLine, GetLineMaxColumn(currentLine));
			SetCursorPosition({ currentLine, 0 }, c);
		}

		u.mOperations.push_back({ GetText(toDeleteStart, toDeleteEnd), toDeleteStart, toDeleteEnd, UndoOperationType::Delete });

		std::unordered_set<int> handledCursors = { c };
		if (toDeleteStart.mLine != toDeleteEnd.mLine)
			RemoveLine(currentLine, &handledCursors);
		else
			DeleteRange(toDeleteStart, toDeleteEnd);
	}

	u.mAfter = mState;
	AddUndo(u);
}

float TextEditor::TextDistanceToLineStart(const Coordinates& aFrom, bool aSanitizeCoords) const
{
	if (aSanitizeCoords)
		return SanitizeCoordinates(aFrom).mColumn * mCharAdvance.x;
	else
		return aFrom.mColumn * mCharAdvance.x;
}

void TextEditor::EnsureCursorVisible(int aCursor, bool aStartToo)
{
	if (aCursor == -1)
		aCursor = mState.GetLastAddedCursorIndex();

	mEnsureCursorVisible = aCursor;
	mEnsureCursorVisibleStartToo = aStartToo;
	return;
}

TextEditor::Coordinates TextEditor::SanitizeCoordinates(const Coordinates& aValue) const
{
	auto line = (aValue.mLine >= int(mLines.size())) ? int(mLines.size()) - 1 : aValue.mLine;
	auto column = std::min(aValue.mColumn, GetLineMaxColumn(line));
	return Coordinates(line, column);
}

TextEditor::Coordinates TextEditor::GetActualCursorCoordinates(int aCursor, bool aStart) const
{
	if (aCursor == -1)
		return SanitizeCoordinates(aStart ? mState.mCursors[mState.mCurrentCursor].mInteractiveStart : mState.mCursors[mState.mCurrentCursor].mInteractiveEnd);
	else
		return SanitizeCoordinates(aStart ? mState.mCursors[aCursor].mInteractiveStart : mState.mCursors[aCursor].mInteractiveEnd);
}

TextEditor::Coordinates TextEditor::ScreenPosToCoordinates(const ImVec2& aPosition, bool aInsertionMode, bool* isOverLineNumber) const
{
	ImVec2 origin = ImGui::GetCursorScreenPos();
	ImVec2 local(aPosition.x - origin.x + 3.0f, aPosition.y - origin.y);

	if (isOverLineNumber != nullptr)
		*isOverLineNumber = local.x < mTextStart;

	Coordinates out = {
		std::max(0, int(floor(local.y / mCharAdvance.y))),
		std::max(0, int(floor((local.x - mTextStart) / mCharAdvance.x)))
	};
	int charIndex = GetCharacterIndexL(out);
	if (charIndex > -1 && charIndex < mLines[out.mLine].size() && mLines[out.mLine][charIndex].mChar == '\t')
	{
		int columnToLeft = GetCharacterColumn(out.mLine, charIndex);
		int columnToRight = GetCharacterColumn(out.mLine, GetCharacterIndexR(out));
		if (out.mColumn - columnToLeft < columnToRight - out.mColumn)
			out.mColumn = columnToLeft;
		else
			out.mColumn = columnToRight;
	}
	else
		out.mColumn = std::max(0, int(floor((local.x - mTextStart + 0.33f * mCharAdvance.x) / mCharAdvance.x)));
	return SanitizeCoordinates(out);
}

TextEditor::Coordinates TextEditor::FindWordStart(const Coordinates& aFrom) const
{
	if (aFrom.mLine >= int(mLines.size()))
		return aFrom;

	int lineIndex = aFrom.mLine;
	auto& line = mLines[lineIndex];
	int charIndex = GetCharacterIndexL(aFrom);

	if (charIndex > int(line.size()) || line.size() == 0)
		return aFrom;
	if (charIndex == int(line.size()))
		charIndex--;

	bool initialIsWordChar = CharIsWordChar(line[charIndex].mChar);
	bool initialIsSpace = isspace(line[charIndex].mChar);
	char initialChar = line[charIndex].mChar;
	while (Move(lineIndex, charIndex, true, true))
	{
		bool isWordChar = CharIsWordChar(line[charIndex].mChar);
		bool isSpace = isspace(line[charIndex].mChar);
		if ((initialIsSpace && !isSpace) ||
			(initialIsWordChar && !isWordChar) ||
			(!initialIsWordChar && !initialIsSpace && initialChar != line[charIndex].mChar))
		{
			Move(lineIndex, charIndex, false, true); // one step to the right
			break;
		}
	}
	return { aFrom.mLine, GetCharacterColumn(aFrom.mLine, charIndex) };
}

TextEditor::Coordinates TextEditor::FindWordEnd(const Coordinates& aFrom) const
{
	if (aFrom.mLine >= int(mLines.size()))
		return aFrom;

	int lineIndex = aFrom.mLine;
	auto& line = mLines[lineIndex];
	auto charIndex = GetCharacterIndexL(aFrom);

	if (charIndex >= int(line.size()))
		return aFrom;

	bool initialIsWordChar = CharIsWordChar(line[charIndex].mChar);
	bool initialIsSpace = isspace(line[charIndex].mChar);
	char initialChar = line[charIndex].mChar;
	while (Move(lineIndex, charIndex, false, true))
	{
		if (charIndex == line.size())
			break;
		bool isWordChar = CharIsWordChar(line[charIndex].mChar);
		bool isSpace = isspace(line[charIndex].mChar);
		if ((initialIsSpace && !isSpace) ||
			(initialIsWordChar && !isWordChar) ||
			(!initialIsWordChar && !initialIsSpace && initialChar != line[charIndex].mChar))
			break;
	}
	return { lineIndex, GetCharacterColumn(aFrom.mLine, charIndex) };
}

bool TextEditor::IsWholeWord(const Coordinates& aFrom, const Coordinates& aTo) const
{
	if (aFrom.mLine >= mLines.size() || aTo.mLine >= mLines.size())
		return false;

	auto& startLine = mLines[aFrom.mLine];
	auto charIndex = GetCharacterIndexL(aFrom);

	if (charIndex != 0 && CharIsWordChar(startLine[charIndex - 1].mChar)) {
		return false;
	}

	auto& endLine = mLines[aTo.mLine];
	charIndex = GetCharacterIndexL(aTo);

	if (charIndex != endLine.size() && CharIsWordChar(endLine[charIndex].mChar)) {
		return false;
	}

	return true;
}

int TextEditor::GetCharacterIndexL(const Coordinates& aCoords) const
{
	if (aCoords.mLine >= mLines.size())
		return -1;

	auto& line = mLines[aCoords.mLine];
	int c = 0;
	int i = 0;
	int tabCoordsLeft = 0;

	for (; i < line.size() && c < aCoords.mColumn;)
	{
		if (line[i].mChar == '\t')
		{
			if (tabCoordsLeft == 0)
				tabCoordsLeft = TabSizeAtColumn(c);
			if (tabCoordsLeft > 0)
				tabCoordsLeft--;
			c++;
		}
		else
			++c;
		if (tabCoordsLeft == 0)
			i += UTF8CharLength(line[i].mChar);
	}
	return i;
}

int TextEditor::GetCharacterIndexR(const Coordinates& aCoords) const
{
	if (aCoords.mLine >= mLines.size())
		return -1;
	int c = 0;
	int i = 0;
	for (; i < mLines[aCoords.mLine].size() && c < aCoords.mColumn;)
		MoveCharIndexAndColumn(aCoords.mLine, i, c);
	return i;
}

int TextEditor::GetCharacterColumn(int aLine, int aIndex) const
{
	if (aLine >= mLines.size())
		return 0;
	int c = 0;
	int i = 0;
	while (i < aIndex && i < mLines[aLine].size())
		MoveCharIndexAndColumn(aLine, i, c);
	return c;
}

int TextEditor::GetFirstVisibleCharacterIndex(int aLine) const
{
	if (aLine >= mLines.size())
		return 0;
	int c = 0;
	int i = 0;
	while (c < mFirstVisibleColumn && i < mLines[aLine].size())
		MoveCharIndexAndColumn(aLine, i, c);
	if (c > mFirstVisibleColumn)
		i--;
	return i;
}

int TextEditor::GetLineMaxColumn(int aLine, int aLimit) const
{
	if (aLine >= mLines.size())
		return 0;
	int c = 0;
	if (aLimit == -1)
	{
		for (int i = 0; i < mLines[aLine].size(); )
			MoveCharIndexAndColumn(aLine, i, c);
	}
	else
	{
		for (int i = 0; i < mLines[aLine].size(); )
		{
			MoveCharIndexAndColumn(aLine, i, c);
			if (c > aLimit)
				return aLimit;
		}
	}
	return c;
}

TextEditor::Line& TextEditor::InsertLine(int aIndex)
{
	IM_ASSERT(!mReadOnly);
	auto& result = *mLines.insert(mLines.begin() + aIndex, Line());

	for (int c = 0; c <= mState.mCurrentCursor; c++) // handle multiple cursors
	{
		if (mState.mCursors[c].mInteractiveEnd.mLine >= aIndex)
			SetCursorPosition({ mState.mCursors[c].mInteractiveEnd.mLine + 1, mState.mCursors[c].mInteractiveEnd.mColumn }, c);
	}

	// handle error markers
	std::map<int, std::string> etmp;
	for (auto& i : mErrorMarkers)
		etmp.insert({i.first >= aIndex ? i.first + 1 : i.first, i.second});
	mErrorMarkers = std::move(etmp);

	return result;
}

void TextEditor::RemoveLine(int aIndex, const std::unordered_set<int>* aHandledCursors)
{
	IM_ASSERT(!mReadOnly);
	IM_ASSERT(mLines.size() > 1);

	mLines.erase(mLines.begin() + aIndex);

	// handle multiple cursors
	for (int c = 0; c <= mState.mCurrentCursor; c++)
	{
		if (mState.mCursors[c].mInteractiveEnd.mLine >= aIndex)
		{
			if (aHandledCursors == nullptr || aHandledCursors->find(c) == aHandledCursors->end()) // move up if has not been handled already
				SetCursorPosition({ mState.mCursors[c].mInteractiveEnd.mLine - 1, mState.mCursors[c].mInteractiveEnd.mColumn }, c);
		}
	}

	// handle error markers
	std::map<int, std::string> etmp;
	for (auto& i : mErrorMarkers)
		if (i.first != aIndex)
			etmp.insert({i.first < aIndex ? i.first : i.first - 1, i.second});
	mErrorMarkers = std::move(etmp);
}

void TextEditor::RemoveLines(int aStart, int aEnd)
{
	IM_ASSERT(!mReadOnly);
	IM_ASSERT(aEnd >= aStart);
	IM_ASSERT(mLines.size() > (size_t)(aEnd - aStart));

	mLines.erase(mLines.begin() + aStart, mLines.begin() + aEnd);

	if (mLines.empty())
		mLines.emplace_back(Line());

	// handle multiple cursors
	for (int c = 0; c <= mState.mCurrentCursor; c++)
	{
		if (mState.mCursors[c].mInteractiveEnd.mLine >= aStart)
		{
			int targetLine = mState.mCursors[c].mInteractiveEnd.mLine - (aEnd - aStart);
			targetLine = targetLine < 0 ? 0 : targetLine;
			mState.mCursors[c].mInteractiveEnd.mLine = targetLine;
		}
		if (mState.mCursors[c].mInteractiveStart.mLine >= aStart)
		{
			int targetLine = mState.mCursors[c].mInteractiveStart.mLine - (aEnd - aStart);
			targetLine = targetLine < 0 ? 0 : targetLine;
			mState.mCursors[c].mInteractiveStart.mLine = targetLine;
		}
	}

	// handle error markers
	std::map<int, std::string> etmp;
	for (auto& i : mErrorMarkers)
		if (i.first < aStart || i.first >= aEnd)
			etmp.insert({i.first < aStart ? i.first : i.first - (aEnd - aStart), i.second});
	mErrorMarkers = std::move(etmp);
}

void TextEditor::DeleteRange(const Coordinates& aStart, const Coordinates& aEnd)
{
	IM_ASSERT(aEnd >= aStart);
	IM_ASSERT(!mReadOnly);

	if (aEnd == aStart)
		return;

	auto start = GetCharacterIndexL(aStart);
	auto end = GetCharacterIndexR(aEnd);

	if (aStart.mLine == aEnd.mLine)
	{
		auto n = GetLineMaxColumn(aStart.mLine);
		if (aEnd.mColumn >= n)
			RemoveGlyphsFromLine(aStart.mLine, start); // from start to end of line
		else
			RemoveGlyphsFromLine(aStart.mLine, start, end);
	}
	else
	{
		RemoveGlyphsFromLine(aStart.mLine, start); // from start to end of line
		RemoveGlyphsFromLine(aEnd.mLine, 0, end);
		auto& firstLine = mLines[aStart.mLine];
		auto& lastLine = mLines[aEnd.mLine];

		if (aStart.mLine < aEnd.mLine)
		{
			AddGlyphsToLine(aStart.mLine, int(firstLine.size()), lastLine.begin(), lastLine.end());
			for (int c = 0; c <= mState.mCurrentCursor; c++) // move up cursors in line that is being moved up
			{
				// if cursor is selecting the same range we are deleting, it's because this is being called from
				// DeleteSelection which already sets the cursor position after the range is deleted
				if (mState.mCursors[c].GetSelectionStart() == aStart && mState.mCursors[c].GetSelectionEnd() == aEnd)
					continue;
				if (mState.mCursors[c].mInteractiveEnd.mLine > aEnd.mLine)
					break;
				else if (mState.mCursors[c].mInteractiveEnd.mLine != aEnd.mLine)
					continue;
				int otherCursorEndCharIndex = GetCharacterIndexR(mState.mCursors[c].mInteractiveEnd);
				int otherCursorStartCharIndex = GetCharacterIndexR(mState.mCursors[c].mInteractiveStart);
				int otherCursorNewEndCharIndex = GetCharacterIndexR(aStart) + otherCursorEndCharIndex;
				int otherCursorNewStartCharIndex = GetCharacterIndexR(aStart) + otherCursorStartCharIndex;
				auto targetEndCoords = Coordinates(aStart.mLine, GetCharacterColumn(aStart.mLine, otherCursorNewEndCharIndex));
				auto targetStartCoords = Coordinates(aStart.mLine, GetCharacterColumn(aStart.mLine, otherCursorNewStartCharIndex));
				SetCursorPosition(targetStartCoords, c, true);
				SetCursorPosition(targetEndCoords, c, false);
			}
			RemoveLines(aStart.mLine + 1, aEnd.mLine + 1);
		}
	}
}

void TextEditor::DeleteSelection(int aCursor)
{
	if (aCursor == -1)
		aCursor = mState.mCurrentCursor;

	if (mState.mCursors[aCursor].GetSelectionEnd() == mState.mCursors[aCursor].GetSelectionStart())
		return;

	Coordinates newCursorPos = mState.mCursors[aCursor].GetSelectionStart();
	DeleteRange(newCursorPos, mState.mCursors[aCursor].GetSelectionEnd());
	SetCursorPosition(newCursorPos, aCursor);
	Colorize(newCursorPos.mLine, 1);
}

void TextEditor::RemoveGlyphsFromLine(int aLine, int aStartChar, int aEndChar)
{
	int column = GetCharacterColumn(aLine, aStartChar);
	auto& line = mLines[aLine];
	OnBeforeLineChange(aLine, column, aEndChar - aStartChar, true);
	line.erase(line.begin() + aStartChar, aEndChar == -1 ? line.end() : line.begin() + aEndChar);
	OnAfterLineChange(aLine, column, aEndChar - aStartChar, true);
}

void TextEditor::AddGlyphsToLine(int aLine, int aTargetIndex, Line::iterator aSourceStart, Line::iterator aSourceEnd)
{
	int targetColumn = GetCharacterColumn(aLine, aTargetIndex);
	int charsInserted = int(std::distance(aSourceStart, aSourceEnd));
	auto& line = mLines[aLine];
	OnBeforeLineChange(aLine, targetColumn, charsInserted, false);
	line.insert(line.begin() + aTargetIndex, aSourceStart, aSourceEnd);
	OnAfterLineChange(aLine, targetColumn, charsInserted, false);
}

void TextEditor::AddGlyphToLine(int aLine, int aTargetIndex, Glyph aGlyph)
{
	int targetColumn = GetCharacterColumn(aLine, aTargetIndex);
	auto& line = mLines[aLine];
	OnBeforeLineChange(aLine, targetColumn, 1, false);
	line.insert(line.begin() + aTargetIndex, aGlyph);
	OnAfterLineChange(aLine, targetColumn, 1, false);
}

ImU32 TextEditor::GetGlyphColor(const Glyph& aGlyph) const
{
	if (mLanguageDefinition == nullptr)
		return mPalette[int(PaletteIndex::Default)];
	if (aGlyph.mComment)
		return mPalette[int(PaletteIndex::Comment)];
	if (aGlyph.mMultiLineComment)
		return mPalette[int(PaletteIndex::MultiLineComment)];
	auto const color = mPalette[int(aGlyph.mColorIndex)];
	if (aGlyph.mPreprocessor)
	{
		const auto ppcolor = mPalette[int(PaletteIndex::Preprocessor)];
		const int c0 = ((ppcolor & 0xff) + (color & 0xff)) / 2;
		const int c1 = (((ppcolor >> 8) & 0xff) + ((color >> 8) & 0xff)) / 2;
		const int c2 = (((ppcolor >> 16) & 0xff) + ((color >> 16) & 0xff)) / 2;
		const int c3 = (((ppcolor >> 24) & 0xff) + ((color >> 24) & 0xff)) / 2;
		return ImU32(c0 | (c1 << 8) | (c2 << 16) | (c3 << 24));
	}
	return color;
}

void TextEditor::HandleKeyboardInputs(bool aParentIsFocused)
{
	if (ImGui::IsWindowFocused() || ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow))
	{
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

		ImGuiIO& io = ImGui::GetIO();
		io.WantCaptureKeyboard = true;
		io.WantTextInput = true;

		// cursor movements
		if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
			MoveUp(1, shift);
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
			MoveDown(1, shift);
		else if (isOptionalAltShift && ImGui::IsKeyPressed(ImGuiKey_LeftArrow))
			MoveLeft(shift, alt);
		else if (isOptionalAltShift && ImGui::IsKeyPressed(ImGuiKey_RightArrow))
			MoveRight(shift, alt);
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_PageUp))
			MoveUp(mVisibleLineCount - 2, shift);
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_PageDown))
			MoveDown(mVisibleLineCount - 2, shift);
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
			MoveTop(shift);
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Home))
			MoveTop(shift);
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
			MoveBottom(shift);
		else if (isOptionalShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_End))
			MoveBottom(shift);
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_Home))
			MoveHome(shift);
		else if (isOptionalShift && ImGui::IsKeyPressed(ImGuiKey_End))
			MoveEnd(shift);

		// text selection
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_A))
			SelectAll();
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_D))
			AddCursorForNextOccurrence();

		// clipboard operations
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_X))
			Cut();
		else if (isShiftOnly && ImGui::IsKeyPressed(ImGuiKey_Delete))
			Cut();
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_Insert))
			Copy();
		else if (isShortcut && ImGui::IsKeyPressed(ImGuiKey_C))
			Copy();
		else if (!mReadOnly && isShiftOnly && ImGui::IsKeyPressed(ImGuiKey_Insert))
			Paste();
		else if (!mReadOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_V))
			Paste();
		else if (!mReadOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Z))
			Undo();
		else if (!mReadOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Y))
			Redo();
		else if (!mReadOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_Z))
			Redo();

		// remove text
		else if (!mReadOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Delete))
			Delete(alt);
		else if (!mReadOnly && isOptionalAlt && ImGui::IsKeyPressed(ImGuiKey_Backspace))
			Backspace(alt);
		else if (!mReadOnly && isShiftShortcut && ImGui::IsKeyPressed(ImGuiKey_K))
			RemoveCurrentLines();

			// text manipulation
		else if (!mReadOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_LeftBracket))
			ChangeCurrentLinesIndentation(false);
		else if (!mReadOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_RightBracket))
			ChangeCurrentLinesIndentation(true);
		else if (!mReadOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_UpArrow))
			MoveUpCurrentLines();
		else if (!mReadOnly && isAltOnly && ImGui::IsKeyPressed(ImGuiKey_DownArrow))
			MoveDownCurrentLines();
		else if (!mReadOnly && isShortcut && ImGui::IsKeyPressed(ImGuiKey_Slash))
			ToggleLineComment();

		// change mode
		else if (isNoModifiers && ImGui::IsKeyPressed(ImGuiKey_Insert))
			mOverwrite ^= true;

		// handle new line
		else if (!mReadOnly && isNoModifiers && (ImGui::IsKeyPressed(ImGuiKey_Enter) || ImGui::IsKeyPressed(ImGuiKey_KeypadEnter)))
			EnterCharacter('\n');

		// handle tabs
		else if (!mReadOnly && ImGui::IsKeyPressed(ImGuiKey_Tab))
		{
			if (AnyCursorHasSelection())
				ChangeCurrentLinesIndentation(!shift);
			else
				EnterCharacter('\t');
		}

		// regular text
		if (!mReadOnly && !io.InputQueueCharacters.empty())
		{
			for (int i = 0; i < io.InputQueueCharacters.Size; i++)
			{
				auto c = io.InputQueueCharacters[i];

				if (c == '\n' || c >= 32)
					EnterCharacter(c);
			}

			io.InputQueueCharacters.resize(0);
		}
	}
}

void TextEditor::HandleMouseInputs()
{
	ImGuiIO& io = ImGui::GetIO();

	// show text cursor if required
	if (ImGui::IsWindowFocused())
		ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);

	// Pan with dragging middle mouse button
	if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
	{
		ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
		ImGui::SetScrollX(ImGui::GetScrollX() - mouseDelta.x);
		ImGui::SetScrollY(ImGui::GetScrollY() - mouseDelta.y);
		ImGui::ResetMouseDragDelta();
	}

	// Re-sort and possibly merge cursors on left mouse button release
	else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		mState.SortCursorsFromTopToBottom();
		MergeCursorsIfPossible();
	}

	// Ignore other interactions when the editor is not hovered
	else if (ImGui::IsWindowHovered())
	{
		// Update selection with dragging left mouse button
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			io.WantCaptureMouse = true;
			Coordinates cursorCoords = ScreenPosToCoordinates(ImGui::GetMousePos(), !mOverwrite);
			SetCursorPosition(cursorCoords, mState.GetLastAddedCursorIndex(), false);
		}

		// Extend selection with shift left mouse button click
		else if (ImGui::IsKeyDown(ImGuiMod_Shift) && ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			Coordinates newSelection = ScreenPosToCoordinates(ImGui::GetMousePos(), !mOverwrite);
			SetCursorPosition(newSelection, mState.mCurrentCursor, false);
		}

		// handle left mouse button actions
		else
		{
			auto click = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
			auto doubleClick = ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
			auto t = ImGui::GetTime();
			auto tripleClick = click && !doubleClick && (mLastClickTime != -1.0f && (t - mLastClickTime) < io.MouseDoubleClickTime);

#if __APPLE__
			auto addCursor = ImGui::IsKeyDown(ImGuiMod_Alt);
#else
			auto addCursor = ImGui::IsKeyDown(ImGuiMod_Ctrl);
#endif

			// Left mouse button triple click
			if (tripleClick)
			{
				if (addCursor)
					mState.AddCursor();
				else
					mState.mCurrentCursor = 0;

				Coordinates cursorCoords = ScreenPosToCoordinates(ImGui::GetMousePos());
				Coordinates targetCursorPos = cursorCoords.mLine < mLines.size() - 1 ?
					Coordinates{ cursorCoords.mLine + 1, 0 } :
					Coordinates{ cursorCoords.mLine, GetLineMaxColumn(cursorCoords.mLine) };
				SetSelection({ cursorCoords.mLine, 0 }, targetCursorPos, mState.mCurrentCursor);

				mLastClickTime = -1.0f;
			}

			// Left mouse button double click
			else if (doubleClick)
			{
				if (addCursor)
					mState.AddCursor();
				else
					mState.mCurrentCursor = 0;

				Coordinates cursorCoords = ScreenPosToCoordinates(ImGui::GetMousePos());
				SetSelection(FindWordStart(cursorCoords), FindWordEnd(cursorCoords), mState.mCurrentCursor);

				mLastClickTime = float(ImGui::GetTime());
			}

			// Left mouse button single click
			else if (click)
			{
				if (addCursor)
					mState.AddCursor();
				else
					mState.mCurrentCursor = 0;

				bool isOverLineNumber;
				Coordinates cursorCoords = ScreenPosToCoordinates(ImGui::GetMousePos(), !mOverwrite, &isOverLineNumber);
				if (isOverLineNumber)
				{
					Coordinates targetCursorPos = cursorCoords.mLine < mLines.size() - 1 ?
						Coordinates{ cursorCoords.mLine + 1, 0 } :
						Coordinates{ cursorCoords.mLine, GetLineMaxColumn(cursorCoords.mLine) };
					SetSelection({ cursorCoords.mLine, 0 }, targetCursorPos, mState.mCurrentCursor);
				}
				else
					SetCursorPosition(cursorCoords, mState.GetLastAddedCursorIndex());

				mLastClickTime = float(ImGui::GetTime());
			}
		}
	}
}

void TextEditor::UpdateViewVariables(float aScrollX, float aScrollY)
{
	mContentHeight = ImGui::GetWindowHeight() - (IsHorizontalScrollbarVisible() ? ImGui::GetStyle().ScrollbarSize : 0.0f);
	mContentWidth = ImGui::GetWindowWidth() - (IsVerticalScrollbarVisible() ? ImGui::GetStyle().ScrollbarSize : 0.0f);

	mVisibleLineCount = std::max(int(ceil(mContentHeight / mCharAdvance.y)), 0);
	mFirstVisibleLine = std::max(int(aScrollY / mCharAdvance.y), 0);
	mLastVisibleLine = std::max(int((mContentHeight + aScrollY) / mCharAdvance.y), 0);

	mVisibleColumnCount = std::max(int(ceil((mContentWidth - std::max(mTextStart - aScrollX, 0.0f)) / mCharAdvance.x)), 0);
	mFirstVisibleColumn = std::max(int(std::max(aScrollX - mTextStart, 0.0f) / mCharAdvance.x), 0);
	mLastVisibleColumn = std::max(int((mContentWidth + aScrollX - mTextStart) / mCharAdvance.x), 0);
}

void TextEditor::Render()
{
	// Compute mCharAdvance regarding to scaled font size (Ctrl + mouse wheel)
	const float fontWidth = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;
	const float fontHeight = ImGui::GetTextLineHeightWithSpacing();
	mCharAdvance = ImVec2(fontWidth, fontHeight * mLineSpacing);
	auto drawList = ImGui::GetWindowDrawList();

	// Deduce mTextStart by evaluating mLines size (global lineMax) plus two spaces as text width
	mTextStart = mLeftMargin;
	if (mShowLineNumbers)
	{
		int maxDigits = int(std::log10(mLines.size()) + 1);
		mTextStart += (maxDigits + 2) * mCharAdvance.x;
	}

	ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
	mScrollX = ImGui::GetScrollX();
	mScrollY = ImGui::GetScrollY();
	UpdateViewVariables(mScrollX, mScrollY);

	auto io = ImGui::GetIO();
	mCursorAnimationTimer = std::fmod(mCursorAnimationTimer + io.DeltaTime, 1.0f);

	int maxColumnLimited = 0;

	float spaceSize = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, " ", nullptr, nullptr).x;

	for (int lineNo = mFirstVisibleLine; lineNo <= mLastVisibleLine && lineNo < mLines.size(); lineNo++)
	{
		ImVec2 lineStartScreenPos = ImVec2(cursorScreenPos.x, cursorScreenPos.y + lineNo * mCharAdvance.y);
		ImVec2 textScreenPos = ImVec2(lineStartScreenPos.x + mTextStart, lineStartScreenPos.y);

		auto& line = mLines[lineNo];
		maxColumnLimited = std::max(GetLineMaxColumn(lineNo, mLastVisibleColumn), maxColumnLimited);

		Coordinates lineStartCoord(lineNo, 0);
		Coordinates lineEndCoord(lineNo, maxColumnLimited);

		// Draw selection for the current line
		for (int c = 0; c <= mState.mCurrentCursor; c++)
		{
			float rectStart = -1.0f;
			float rectEnd = -1.0f;
			Coordinates cursorSelectionStart = mState.mCursors[c].GetSelectionStart();
			Coordinates cursorSelectionEnd = mState.mCursors[c].GetSelectionEnd();
			IM_ASSERT(cursorSelectionStart <= cursorSelectionEnd);

			if (cursorSelectionStart <= lineEndCoord)
				rectStart = cursorSelectionStart > lineStartCoord ? TextDistanceToLineStart(cursorSelectionStart) : 0.0f;
			if (cursorSelectionEnd > lineStartCoord)
				rectEnd = TextDistanceToLineStart(cursorSelectionEnd < lineEndCoord ? cursorSelectionEnd : lineEndCoord);
			if (cursorSelectionEnd.mLine > lineNo ||
				(cursorSelectionEnd.mLine == lineNo && cursorSelectionEnd > lineEndCoord))
				rectEnd += mCharAdvance.x;

			if (rectStart != -1 && rectEnd != -1 && rectStart < rectEnd)
				drawList->AddRectFilled(
					ImVec2{ lineStartScreenPos.x + mTextStart + rectStart, lineStartScreenPos.y },
					ImVec2{ lineStartScreenPos.x + mTextStart + rectEnd, lineStartScreenPos.y + mCharAdvance.y },
					mPalette[int(PaletteIndex::Selection)]);
		}

		auto errorIt = mErrorMarkers.find(lineNo + 1);

		// Draw line number (right aligned)
		if (mShowLineNumbers)
		{
			int digits = int(std::log10(lineNo + 1) + 1.0f);
			float lineNoWidth = (digits + 2) * mCharAdvance.x;

			// Draw error marker (if required)
			if (errorIt != mErrorMarkers.end())
			{
				auto start = ImVec2(lineStartScreenPos.x + mTextStart - lineNoWidth - 2, lineStartScreenPos.y);
				auto end = ImVec2(start.x + digits * mCharAdvance.x + 4, lineStartScreenPos.y + mCharAdvance.y);
				drawList->AddRectFilled(start, end, mPalette[int(PaletteIndex::ErrorMarker)]);

				// Draw tooltip (if mouse is hovering over line number)
				if (ImGui::IsMouseHoveringRect(start, end))
				{
					ImGui::BeginTooltip();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
					ImGui::Text("Error at line %d:", errorIt->first);
					ImGui::PopStyleColor();
					ImGui::Separator();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.2f, 1.0f));
					ImGui::Text("%s", errorIt->second.c_str());
					ImGui::PopStyleColor();
					ImGui::EndTooltip();
				}
			}

			PaletteIndex foreground = mState.mCursors[0].mInteractiveEnd.mLine == lineNo ? PaletteIndex::CurrentLineNumber : PaletteIndex::LineNumber;
			drawList->AddText(ImVec2(lineStartScreenPos.x + mTextStart - lineNoWidth, lineStartScreenPos.y), mPalette[int(foreground)], std::to_string(lineNo + 1).c_str());
		}

		std::vector<Coordinates> cursorCoordsInThisLine;
		for (int c = 0; c <= mState.mCurrentCursor; c++)
		{
			if (mState.mCursors[c].mInteractiveEnd.mLine == lineNo)
				cursorCoordsInThisLine.push_back(mState.mCursors[c].mInteractiveEnd);
		}
		if (cursorCoordsInThisLine.size() > 0)
		{
			// Render the cursors
			if (ImGui::IsWindowFocused())
			{
				for (const auto& cursorCoords : cursorCoordsInThisLine)
				{
					float width = 1.0f;
					auto cindex = GetCharacterIndexR(cursorCoords);
					float cx = TextDistanceToLineStart(cursorCoords);

					if (mOverwrite && cindex < int(line.size()))
					{
						if (line[cindex].mChar == '\t')
						{
							auto x = (1.0f + std::floor((1.0f + cx) / (float(mTabSize) * spaceSize))) * (float(mTabSize) * spaceSize);
							width = x - cx;
						}
						else
							width = mCharAdvance.x;
					}
					ImVec2 cstart(textScreenPos.x + cx, lineStartScreenPos.y);
					ImVec2 cend(textScreenPos.x + cx + width, lineStartScreenPos.y + mCharAdvance.y);

					if (!io.ConfigInputTextCursorBlink || mCursorAnimationTimer < 0.5f)
						drawList->AddRectFilled(cstart, cend, mPalette[int(PaletteIndex::Cursor)]);

					if (mShowMatchingBrackets && mCursorOnBracket)
					{
						ImVec2 topLeft = { cstart.x, lineStartScreenPos.y + fontHeight + 1.0f };
						ImVec2 bottomRight = { topLeft.x + mCharAdvance.x, topLeft.y + 1.0f };
						drawList->AddRectFilled(topLeft, bottomRight, mPalette[int(PaletteIndex::Cursor)]);
					}
				}
			}
		}

		// Render colorized text
		int charIndex = GetFirstVisibleCharacterIndex(lineNo);
		int column = mFirstVisibleColumn; // can be in the middle of tab character
		while (charIndex < mLines[lineNo].size() && column <= mLastVisibleColumn)
		{
			auto& glyph = line[charIndex];
			auto color = GetGlyphColor(glyph);
			ImVec2 targetGlyphPos = { lineStartScreenPos.x + mTextStart + TextDistanceToLineStart({lineNo, column}, false), lineStartScreenPos.y };

			if (glyph.mChar == '\t')
			{
				if (mShowWhitespaces)
				{
					ImVec2 p1, p2, p3, p4;

					const auto s = ImGui::GetFontSize();
					const auto x1 = targetGlyphPos.x + mCharAdvance.x * 0.3f;
					const auto y = targetGlyphPos.y + fontHeight * 0.5f;

					if (mShortTabs)
					{
						const auto x2 = targetGlyphPos.x + mCharAdvance.x;
						p1 = ImVec2(x1, y);
						p2 = ImVec2(x2, y);
						p3 = ImVec2(x2 - s * 0.16f, y - s * 0.16f);
						p4 = ImVec2(x2 - s * 0.16f, y + s * 0.16f);
					}
					else
					{
						const auto x2 = targetGlyphPos.x + TabSizeAtColumn(column) * mCharAdvance.x - mCharAdvance.x * 0.3f;
						p1 = ImVec2(x1, y);
						p2 = ImVec2(x2, y);
						p3 = ImVec2(x2 - s * 0.2f, y - s * 0.2f);
						p4 = ImVec2(x2 - s * 0.2f, y + s * 0.2f);
					}

					drawList->AddLine(p1, p2, mPalette[int(PaletteIndex::ControlCharacter)]);
					drawList->AddLine(p2, p3, mPalette[int(PaletteIndex::ControlCharacter)]);
					drawList->AddLine(p2, p4, mPalette[int(PaletteIndex::ControlCharacter)]);
				}
			}
			else if (glyph.mChar == ' ')
			{
				if (mShowWhitespaces)
				{
					const auto s = ImGui::GetFontSize();
					const auto x = targetGlyphPos.x + spaceSize * 0.5f;
					const auto y = targetGlyphPos.y + s * 0.5f;
					drawList->AddCircleFilled(ImVec2(x, y), 1.5f, mPalette[int(PaletteIndex::ControlCharacter)], 4);
				}
			}
			else
			{
				int seqLength = UTF8CharLength(glyph.mChar);
				if (mCursorOnBracket && seqLength == 1 && mMatchingBracketCoords == Coordinates{ lineNo, column })
				{
					ImVec2 topLeft = { targetGlyphPos.x, targetGlyphPos.y + fontHeight + 1.0f };
					ImVec2 bottomRight = { topLeft.x + mCharAdvance.x, topLeft.y + 1.0f };
					drawList->AddRectFilled(topLeft, bottomRight, mPalette[int(PaletteIndex::Cursor)]);
				}

				std::string glyphBuffer;

				for (int i = 0; i < seqLength; i++)
					glyphBuffer.push_back(line[charIndex + i].mChar);

				drawList->AddText(targetGlyphPos, color, glyphBuffer.c_str());
			}

			MoveCharIndexAndColumn(lineNo, charIndex, column);
		}

		if (errorIt != mErrorMarkers.end())
		{
			// underline error line
			auto start = ImVec2(lineStartScreenPos.x, lineStartScreenPos.y + mCharAdvance.y - 4);
			auto end = ImVec2(lineStartScreenPos.x + mContentWidth, lineStartScreenPos.y + mCharAdvance.y - 1);
			drawList->AddRectFilled(start, end, mPalette[int(PaletteIndex::ErrorMarker)]);
		}
	}

	mCurrentSpaceHeight = (mLines.size() + std::min(mVisibleLineCount - 1, int(mLines.size()))) * mCharAdvance.y;
	mCurrentSpaceWidth = std::max((maxColumnLimited + std::min(mVisibleColumnCount - 1, maxColumnLimited)) * mCharAdvance.x, mCurrentSpaceWidth);

	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::Dummy(ImVec2(mCurrentSpaceWidth, mCurrentSpaceHeight));

	if (mEnsureCursorVisible > -1)
	{
		for (int i = 0; i < (mEnsureCursorVisibleStartToo ? 2 : 1); i++) // first pass for interactive end and second pass for interactive start
		{
			if (i) UpdateViewVariables(mScrollX, mScrollY); // second pass depends on changes made in first pass
			Coordinates targetCoords = GetActualCursorCoordinates(mEnsureCursorVisible, i); // cursor selection end or start
			if (targetCoords.mLine <= mFirstVisibleLine)
			{
				float targetScroll = std::max(0.0f, (targetCoords.mLine - 0.5f) * mCharAdvance.y);
				if (targetScroll < mScrollY)
					ImGui::SetScrollY(targetScroll);
			}
			if (targetCoords.mLine >= mLastVisibleLine)
			{
				float targetScroll = std::max(0.0f, (targetCoords.mLine + 1.5f) * mCharAdvance.y - mContentHeight);
				if (targetScroll > mScrollY)
					ImGui::SetScrollY(targetScroll);
			}
			if (targetCoords.mColumn <= mFirstVisibleColumn)
			{
				float targetScroll = std::max(0.0f, mTextStart + (targetCoords.mColumn - 0.5f) * mCharAdvance.x);
				if (targetScroll < mScrollX)
					ImGui::SetScrollX(mScrollX = targetScroll);
			}
			if (targetCoords.mColumn >= mLastVisibleColumn)
			{
				float targetScroll = std::max(0.0f, mTextStart + (targetCoords.mColumn + 0.5f) * mCharAdvance.x - mContentWidth);
				if (targetScroll > mScrollX)
					ImGui::SetScrollX(mScrollX = targetScroll);
			}
		}
		mEnsureCursorVisible = -1;
	}
	if (mScrollToTop)
	{
		ImGui::SetScrollY(0.0f);
		mScrollToTop = false;
	}
	if (mSetViewAtLine > -1)
	{
		float targetScroll;
		switch (mSetViewAtLineMode)
		{
		default:
		case SetViewAtLineMode::FirstVisibleLine:
			targetScroll = std::max(0.0f, float(mSetViewAtLine) * mCharAdvance.y);
			break;
		case SetViewAtLineMode::LastVisibleLine:
			targetScroll = std::max(0.0f, float(mSetViewAtLine - (mLastVisibleLine - mFirstVisibleLine)) * mCharAdvance.y);
			break;
		case SetViewAtLineMode::Centered:
			targetScroll = std::max(0.0f, (float(mSetViewAtLine) - float(mLastVisibleLine - mFirstVisibleLine) * 0.5f) * mCharAdvance.y);
			break;
		}
		ImGui::SetScrollY(targetScroll);
		mSetViewAtLine = -1;
	}
}

void TextEditor::OnCursorPositionChanged()
{
	if (mState.mCurrentCursor == 0 && !mState.mCursors[0].HasSelection()) // only one cursor without selection
		mCursorOnBracket = FindMatchingBracket(mState.mCursors[0].mInteractiveEnd.mLine,
			GetCharacterIndexR(mState.mCursors[0].mInteractiveEnd), mMatchingBracketCoords);
	else
		mCursorOnBracket = false;

	if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left))
	{
		mState.SortCursorsFromTopToBottom();
		MergeCursorsIfPossible();
	}
}

void TextEditor::OnBeforeLineChange(int aLine, int aColumn, int aCharCount, bool aDeleted)
{
	cursorCharIndices.clear();

	for (int c = 0; c <= mState.mCurrentCursor; c++)
	{
		if (mState.mCursors[c].mInteractiveEnd.mLine == aLine && // cursor is at the line
			mState.mCursors[c].mInteractiveEnd.mColumn > aColumn && // cursor is to the right of changing part
			mState.mCursors[c].GetSelectionEnd() == mState.mCursors[c].GetSelectionStart()) // cursor does not have a selection
		{
			cursorCharIndices[c] = GetCharacterIndexR({ aLine, mState.mCursors[c].mInteractiveEnd.mColumn });
			cursorCharIndices[c] += aDeleted ? -aCharCount : aCharCount;
		}
	}
}

void TextEditor::OnAfterLineChange(int aLine, int aColumn, int aCharCount, bool aDeleted)
{
	for (auto& item : cursorCharIndices)
		SetCursorPosition({ aLine, GetCharacterColumn(aLine, item.second) }, item.first);
}

void TextEditor::MergeCursorsIfPossible()
{
	// requires the cursors to be sorted from top to bottom
	std::unordered_set<int> cursorsToDelete;
	if (AnyCursorHasSelection())
	{
		// merge cursors if they overlap
		for (int c = mState.mCurrentCursor; c > 0; c--)// iterate backwards through pairs
		{
			int pc = c - 1; // pc for previous cursor

			bool pcContainsC = mState.mCursors[pc].GetSelectionEnd() >= mState.mCursors[c].GetSelectionEnd();
			bool pcContainsStartOfC = mState.mCursors[pc].GetSelectionEnd() > mState.mCursors[c].GetSelectionStart();

			if (pcContainsC)
			{
				cursorsToDelete.insert(c);
			}
			else if (pcContainsStartOfC)
			{
				Coordinates pcStart = mState.mCursors[pc].GetSelectionStart();
				Coordinates cEnd = mState.mCursors[c].GetSelectionEnd();
				mState.mCursors[pc].mInteractiveEnd = cEnd;
				mState.mCursors[pc].mInteractiveStart = pcStart;
				cursorsToDelete.insert(c);
			}
		}
	}
	else
	{
		// merge cursors if they are at the same position
		for (int c = mState.mCurrentCursor; c > 0; c--)// iterate backwards through pairs
		{
			int pc = c - 1;
			if (mState.mCursors[pc].mInteractiveEnd == mState.mCursors[c].mInteractiveEnd)
				cursorsToDelete.insert(c);
		}
	}
	for (int c = mState.mCurrentCursor; c > -1; c--)// iterate backwards through each of them
	{
		if (cursorsToDelete.find(c) != cursorsToDelete.end())
			mState.mCursors.erase(mState.mCursors.begin() + c);
	}
	mState.mCurrentCursor -= cursorsToDelete.size();
}

void TextEditor::AddUndo(UndoRecord& aValue)
{
	IM_ASSERT(!mReadOnly);
	mUndoBuffer.resize((size_t)(mUndoIndex + 1));
	mUndoBuffer.back() = aValue;
	++mUndoIndex;
}

// TODO
// - multiline comments vs single-line: latter is blocking start of a ML
void TextEditor::Colorize(int aFromLine, int aLines)
{
	int toLine = aLines == -1 ? int(mLines.size()) : std::min(int(mLines.size()), aFromLine + aLines);
	mColorRangeMin = std::min(mColorRangeMin, aFromLine);
	mColorRangeMax = std::max(mColorRangeMax, toLine);
	mColorRangeMin = std::max(0, mColorRangeMin);
	mColorRangeMax = std::max(mColorRangeMin, mColorRangeMax);
	mCheckComments = true;
}

void TextEditor::ColorizeRange(int aFromLine, int aToLine)
{
	if (aFromLine >= aToLine || mLanguageDefinition == nullptr)
		return;

	std::string buffer;
	boost::cmatch results;
	std::string id;

	int endLine = std::max(0, std::min(int(mLines.size()), aToLine));
	for (int i = aFromLine; i < endLine; ++i)
	{
		auto& line = mLines[i];

		if (line.empty())
			continue;

		buffer.resize(line.size());
		for (size_t j = 0; j < line.size(); ++j)
		{
			auto& col = line[j];
			buffer[j] = col.mChar;
			col.mColorIndex = PaletteIndex::Default;
		}

		const char* bufferBegin = &buffer.front();
		const char* bufferEnd = bufferBegin + buffer.size();

		auto last = bufferEnd;

		for (auto first = bufferBegin; first != last; )
		{
			const char* token_begin = nullptr;
			const char* token_end = nullptr;
			PaletteIndex token_color = PaletteIndex::Default;

			bool hasTokenizeResult = false;

			if (mLanguageDefinition->mTokenize != nullptr)
			{
				if (mLanguageDefinition->mTokenize(first, last, token_begin, token_end, token_color))
					hasTokenizeResult = true;
			}

			if (hasTokenizeResult == false)
			{
				// todo : remove
				//printf("using regex for %.*s\n", first + 10 < last ? 10 : int(last - first), first);

				for (const auto& p : mRegexList)
				{
					bool regexSearchResult = false;
					try { regexSearchResult = boost::regex_search(first, last, results, p.first, boost::regex_constants::match_continuous); }
					catch (...) {}
					if (regexSearchResult)
					{
						hasTokenizeResult = true;

						auto& v = *results.begin();
						token_begin = v.first;
						token_end = v.second;
						token_color = p.second;
						break;
					}
				}
			}

			if (hasTokenizeResult == false)
			{
				first++;
			}
			else
			{
				const size_t token_length = token_end - token_begin;

				if (token_color == PaletteIndex::Identifier)
				{
					id.assign(token_begin, token_end);

					// todo : allmost all language definitions use lower case to specify keywords, so shouldn't this use ::tolower ?
					if (!mLanguageDefinition->mCaseSensitive)
						std::transform(id.begin(), id.end(), id.begin(), ::toupper);

					if (!line[first - bufferBegin].mPreprocessor)
					{
						if (mLanguageDefinition->mKeywords.count(id) != 0)
							token_color = PaletteIndex::Keyword;
						else if (mLanguageDefinition->mIdentifiers.count(id) != 0)
							token_color = PaletteIndex::KnownIdentifier;
						else if (mLanguageDefinition->mPreprocIdentifiers.count(id) != 0)
							token_color = PaletteIndex::PreprocIdentifier;
					}
					else
					{
						if (mLanguageDefinition->mPreprocIdentifiers.count(id) != 0)
							token_color = PaletteIndex::PreprocIdentifier;
					}
				}

				for (size_t j = 0; j < token_length; ++j)
					line[(token_begin - bufferBegin) + j].mColorIndex = token_color;

				first = token_end;
			}
		}
	}
}

template<class InputIt1, class InputIt2, class BinaryPredicate>
bool ColorizerEquals(InputIt1 first1, InputIt1 last1,
	InputIt2 first2, InputIt2 last2, BinaryPredicate p)
{
	for (; first1 != last1 && first2 != last2; ++first1, ++first2)
	{
		if (!p(*first1, *first2))
			return false;
	}
	return first1 == last1 && first2 == last2;
}
void TextEditor::ColorizeInternal()
{
	if (mLanguageDefinition == nullptr)
		return;

	if (mCheckComments)
	{
		auto endLine = mLines.size();
		auto endIndex = 0;
		auto commentStartLine = endLine;
		auto commentStartIndex = endIndex;
		auto withinString = false;
		auto withinSingleLineComment = false;
		auto withinPreproc = false;
		auto firstChar = true;			// there is no other non-whitespace characters in the line before
		auto concatenate = false;		// '\' on the very end of the line
		auto currentLine = 0;
		auto currentIndex = 0;
		while (currentLine < endLine || currentIndex < endIndex)
		{
			auto& line = mLines[currentLine];

			if (currentIndex == 0 && !concatenate)
			{
				withinSingleLineComment = false;
				withinPreproc = false;
				firstChar = true;
			}

			concatenate = false;

			if (!line.empty())
			{
				auto& g = line[currentIndex];
				auto c = g.mChar;

				if (c != mLanguageDefinition->mPreprocChar && !isspace(c))
					firstChar = false;

				if (currentIndex == int(line.size()) - 1 && line[line.size() - 1].mChar == '\\')
					concatenate = true;

				bool inComment = (commentStartLine < currentLine || (commentStartLine == currentLine && commentStartIndex <= currentIndex));

				if (withinString)
				{
					line[currentIndex].mMultiLineComment = inComment;

					if (c == '\"')
					{
						if (currentIndex + 1 < int(line.size()) && line[currentIndex + 1].mChar == '\"')
						{
							currentIndex += 1;
							if (currentIndex < int(line.size()))
								line[currentIndex].mMultiLineComment = inComment;
						}
						else
							withinString = false;
					}
					else if (c == '\\')
					{
						currentIndex += 1;
						if (currentIndex < int(line.size()))
							line[currentIndex].mMultiLineComment = inComment;
					}
				}
				else
				{
					if (firstChar && c == mLanguageDefinition->mPreprocChar)
						withinPreproc = true;

					if (c == '\"')
					{
						withinString = true;
						line[currentIndex].mMultiLineComment = inComment;
					}
					else
					{
						auto pred = [](const char& a, const Glyph& b) { return a == b.mChar; };
						auto from = line.begin() + currentIndex;
						auto& startStr = mLanguageDefinition->mCommentStart;
						auto& singleStartStr = mLanguageDefinition->mSingleLineComment;

						if (!withinSingleLineComment && currentIndex + startStr.size() <= line.size() &&
							ColorizerEquals(startStr.begin(), startStr.end(), from, from + startStr.size(), pred))
						{
							commentStartLine = currentLine;
							commentStartIndex = currentIndex;
						}
						else if (singleStartStr.size() > 0 &&
							currentIndex + singleStartStr.size() <= line.size() &&
							ColorizerEquals(singleStartStr.begin(), singleStartStr.end(), from, from + singleStartStr.size(), pred))
						{
							withinSingleLineComment = true;
						}

						inComment = (commentStartLine < currentLine || (commentStartLine == currentLine && commentStartIndex <= currentIndex));

						line[currentIndex].mMultiLineComment = inComment;
						line[currentIndex].mComment = withinSingleLineComment;

						auto& endStr = mLanguageDefinition->mCommentEnd;
						if (currentIndex + 1 >= int(endStr.size()) &&
							ColorizerEquals(endStr.begin(), endStr.end(), from + 1 - endStr.size(), from + 1, pred))
						{
							commentStartIndex = endIndex;
							commentStartLine = endLine;
						}
					}
				}
				if (currentIndex < int(line.size()))
					line[currentIndex].mPreprocessor = withinPreproc;
				currentIndex += UTF8CharLength(c);
				if (currentIndex >= int(line.size()))
				{
					currentIndex = 0;
					++currentLine;
				}
			}
			else
			{
				currentIndex = 0;
				++currentLine;
			}
		}
		mCheckComments = false;
	}

	if (mColorRangeMin < mColorRangeMax)
	{
		const int increment = (mLanguageDefinition->mTokenize == nullptr) ? 10 : 10000;
		const int to = std::min(mColorRangeMin + increment, mColorRangeMax);
		ColorizeRange(mColorRangeMin, to);
		mColorRangeMin = to;

		if (mColorRangeMax == mColorRangeMin)
		{
			mColorRangeMin = std::numeric_limits<int>::max();
			mColorRangeMax = 0;
		}
		return;
	}
}

void TextEditor::UpdatePalette()
{
	// Update palette with the current alpha from style
	mPaletteAlpha = ImGui::GetStyle().Alpha;

	for (int i = 0; i < int(PaletteIndex::Max); ++i)
	{
		auto color = ImGui::ColorConvertU32ToFloat4(mPaletteBase[i]);
		color.w *= mPaletteAlpha;
		mPalette[i] = ImGui::ColorConvertFloat4ToU32(color);
	}
}

const TextEditor::Palette& TextEditor::GetDarkPalette()
{
	const static Palette p = { {
		IM_COL32(220, 223, 228, 255),	// Default
		IM_COL32(224, 108, 117, 255),	// Keyword
		IM_COL32(229, 192, 123, 255),	// Number
		IM_COL32(152, 195, 121, 255),	// String
		IM_COL32(224, 160, 112, 255),	// Char literal
		IM_COL32(106, 115, 132, 255),	// Punctuation
		IM_COL32(128, 128,  64, 255),	// Preprocessor
		IM_COL32(220, 223, 228, 255),	// Identifier
		IM_COL32( 97, 175, 239, 255),	// Known identifier
		IM_COL32(198, 120, 221, 255),	// Preproc identifier
		IM_COL32( 54, 150, 162, 255),	// Comment (single line)
		IM_COL32( 54, 150, 162, 255),	// Comment (multi line)
		IM_COL32( 40,  44,  52, 255),	// Background
		IM_COL32(224, 224, 224, 255),	// Cursor
		IM_COL32( 32,  96, 160, 128),	// Selection
		IM_COL32(255,  32,   0, 128),	// ErrorMarker
		IM_COL32(255, 255, 255,  21),	// ControlCharacter
		IM_COL32(  0, 128, 240,  64),	// Breakpoint
		IM_COL32(122, 131, 148, 255),	// Line number
		IM_COL32(255, 255, 255, 255),	// Current line number
	} };

	return p;
}

const TextEditor::Palette& TextEditor::GetMarianaPalette()
{
	const static Palette p = { {
		IM_COL32(255, 255, 255, 255),	// Default
		IM_COL32(198, 149, 198, 255),	// Keyword
		IM_COL32(249, 174,  88, 255),	// Number
		IM_COL32(153, 199, 148, 255),	// String
		IM_COL32(224, 160, 112, 255),	// Char literal
		IM_COL32( 95, 180, 180, 255),	// Punctuation
		IM_COL32(128, 128,  64, 255),	// Preprocessor
		IM_COL32(255, 255, 255, 255),	// Identifier
		IM_COL32( 77, 198, 155, 255),	// Known identifier
		IM_COL32(224, 160, 255, 255),	// Preproc identifier
		IM_COL32(166, 172, 185, 255),	// Comment (single line)
		IM_COL32(166, 172, 185, 255),	// Comment (multi line)
		IM_COL32( 48,  56,  65, 255),	// Background
		IM_COL32(224, 224, 224, 255),	// Cursor
		IM_COL32(110, 122, 133, 128),	// Selection
		IM_COL32(236,  95, 102, 128),	// ErrorMarker
		IM_COL32(255, 255, 255,  48),	// ControlCharacter
		IM_COL32(  0, 128, 240,  64),	// Breakpoint
		IM_COL32(255, 255, 255, 176),	// Line number
		IM_COL32(255, 255, 255, 255),	// Current line number
	} };

	return p;
}

const TextEditor::Palette& TextEditor::GetLightPalette()
{
	const static Palette p = { {
		IM_COL32( 64,  64,  64, 255),	// Default
		IM_COL32(  6,  12, 255, 255),	// Keyword
		IM_COL32(  0, 128,   0, 255),	// Number
		IM_COL32(160,  32,  32, 255),	// String
		IM_COL32(112,  64,  48, 255),	// Char literal
		IM_COL32(  0,   0,   0, 255),	// Punctuation
		IM_COL32( 96,  96,  64, 255),	// Preprocessor
		IM_COL32( 64,  64,  64, 255),	// Identifier
		IM_COL32( 16,  96,  96, 255),	// Known identifier
		IM_COL32(160,  64, 192, 255),	// Preproc identifier
		IM_COL32( 32,  80,  32, 255),	// Comment (single line)
		IM_COL32( 32,  80,  64, 255),	// Comment (multi line)
		IM_COL32(255, 255, 255, 255),	// Background
		IM_COL32(  0,   0,   0, 255),	// Cursor
		IM_COL32(  0,   0,  96,  64),	// Selection
		IM_COL32(255,  16,   0, 160),	// ErrorMarker
		IM_COL32(144, 144, 144, 144),	// ControlCharacter
		IM_COL32(  0, 128, 240, 128),	// Breakpoint
		IM_COL32(  0,  80,  80, 255),	// Line number
		IM_COL32(  0,   0,   0, 255),	// Current line number
	} };

	return p;
}

const TextEditor::Palette& TextEditor::GetRetroBluePalette()
{
	const static Palette p = { {
		IM_COL32(255, 255,   0, 255),	// Default
		IM_COL32(  0, 255, 255, 255),	// Keyword
		IM_COL32(  0, 255,   0, 255),	// Number
		IM_COL32(  0, 128, 128, 255),	// String
		IM_COL32(  0, 128, 128, 255),	// Char literal
		IM_COL32(255, 255, 255, 255),	// Punctuation
		IM_COL32(  0, 128,   0, 255),	// Preprocessor
		IM_COL32(255, 255,   0, 255),	// Identifier
		IM_COL32(255, 255, 255, 255),	// Known identifier
		IM_COL32(255,   0, 255, 255),	// Preproc identifier
		IM_COL32(128, 128, 128, 255),	// Comment (single line)
		IM_COL32( 64,  64,  64, 255),	// Comment (multi line)
		IM_COL32(  0,   0, 128, 255),	// Background
		IM_COL32(255, 128,   0, 255),	// Cursor
		IM_COL32(  0, 255, 255, 128),	// Selection
		IM_COL32(255,   0,   0, 160),	// ErrorMarker
		IM_COL32(255, 255, 255,  80),	// ControlCharacter
		IM_COL32(  0, 128, 255, 128),	// Breakpoint
		IM_COL32(  0, 128, 128, 255),	// Line number
		IM_COL32(255, 255, 255, 255),	// Current line number
	} };

	return p;
}

TextEditor::Palette TextEditor::defaultPalette = TextEditor::GetDarkPalette();
