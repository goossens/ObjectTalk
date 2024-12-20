#include "TextEditor.h"

void TextEditor::UnitTests()
{
	SetText(" \t  \t   \t \t\n");
	// --- GetCharacterColumn --- //
	{
		// Returns column given line and character index in that line.
		// Column is on the left side of character
		IM_ASSERT(GetCharacterColumn(0, 0) == 0);
		IM_ASSERT(GetCharacterColumn(0, 1) == 1);
		IM_ASSERT(GetCharacterColumn(0, 2) == 4);
		IM_ASSERT(GetCharacterColumn(0, 3) == 5);
		IM_ASSERT(GetCharacterColumn(0, 4) == 6);
		IM_ASSERT(GetCharacterColumn(0, 5) == 8);
		IM_ASSERT(GetCharacterColumn(0, 6) == 9);
		IM_ASSERT(GetCharacterColumn(0, 7) == 10);
		IM_ASSERT(GetCharacterColumn(0, 8) == 11);
		IM_ASSERT(GetCharacterColumn(0, 9) == 12);
		IM_ASSERT(GetCharacterColumn(0, 10) == 13);
		IM_ASSERT(GetCharacterColumn(0, 11) == 16);
		IM_ASSERT(GetCharacterColumn(0, 12) == 16); // out of range
		// empty line
		IM_ASSERT(GetCharacterColumn(1, 0) == 0);
		IM_ASSERT(GetCharacterColumn(1, 1) == 0); // out of range
		IM_ASSERT(GetCharacterColumn(1, 2) == 0); // out of range
		// nonexistent line
		IM_ASSERT(GetCharacterColumn(2, 0) == 0);
		IM_ASSERT(GetCharacterColumn(2, 1) == 0);
		IM_ASSERT(GetCharacterColumn(2, 2) == 0);
	}

	// --- GetCharacterIndexL --- //
	{
		// Returns character index from coordinates, if coordinates are in the middle of a tab character it returns character index of that tab character
		IM_ASSERT(GetCharacterIndexL({ 0, 0 }) == 0);
		IM_ASSERT(GetCharacterIndexL({ 0, 1 }) == 1);
		IM_ASSERT(GetCharacterIndexL({ 0, 2 }) == 1);
		IM_ASSERT(GetCharacterIndexL({ 0, 3 }) == 1);
		IM_ASSERT(GetCharacterIndexL({ 0, 4 }) == 2);
		IM_ASSERT(GetCharacterIndexL({ 0, 5 }) == 3);
		IM_ASSERT(GetCharacterIndexL({ 0, 6 }) == 4);
		IM_ASSERT(GetCharacterIndexL({ 0, 7 }) == 4);
		IM_ASSERT(GetCharacterIndexL({ 0, 8 }) == 5);
		IM_ASSERT(GetCharacterIndexL({ 0, 9 }) == 6);
		IM_ASSERT(GetCharacterIndexL({ 0, 10 }) == 7);
		IM_ASSERT(GetCharacterIndexL({ 0, 11 }) == 8);
		IM_ASSERT(GetCharacterIndexL({ 0, 12 }) == 9);
		IM_ASSERT(GetCharacterIndexL({ 0, 13 }) == 10);
		IM_ASSERT(GetCharacterIndexL({ 0, 14 }) == 10);
		IM_ASSERT(GetCharacterIndexL({ 0, 15 }) == 10);
		IM_ASSERT(GetCharacterIndexL({ 0, 16 }) == 11);
		IM_ASSERT(GetCharacterIndexL({ 0, 17 }) == 11); // out of range
		IM_ASSERT(GetCharacterIndexL({ 0, 18 }) == 11); // out of range
		// empty line
		IM_ASSERT(GetCharacterIndexL({ 1, 0 }) == 0);
		IM_ASSERT(GetCharacterIndexL({ 1, 1 }) == 0); // out of range
		IM_ASSERT(GetCharacterIndexL({ 1, 2 }) == 0); // out of range
		// nonexistent line
		IM_ASSERT(GetCharacterIndexL({ 2, 0 }) == -1);
		IM_ASSERT(GetCharacterIndexL({ 2, 1 }) == -1);
		IM_ASSERT(GetCharacterIndexL({ 2, 2 }) == -1);
	}

	// --- GetCharacterIndexR --- //
	{
		// Returns character index from coordinates, if coordinates are in the middle of a tab character it returns character index of next character
		IM_ASSERT(GetCharacterIndexR({ 0, 0 }) == 0);
		IM_ASSERT(GetCharacterIndexR({ 0, 1 }) == 1);
		IM_ASSERT(GetCharacterIndexR({ 0, 2 }) == 2);
		IM_ASSERT(GetCharacterIndexR({ 0, 3 }) == 2);
		IM_ASSERT(GetCharacterIndexR({ 0, 4 }) == 2);
		IM_ASSERT(GetCharacterIndexR({ 0, 5 }) == 3);
		IM_ASSERT(GetCharacterIndexR({ 0, 6 }) == 4);
		IM_ASSERT(GetCharacterIndexR({ 0, 7 }) == 5);
		IM_ASSERT(GetCharacterIndexR({ 0, 8 }) == 5);
		IM_ASSERT(GetCharacterIndexR({ 0, 9 }) == 6);
		IM_ASSERT(GetCharacterIndexR({ 0, 10 }) == 7);
		IM_ASSERT(GetCharacterIndexR({ 0, 11 }) == 8);
		IM_ASSERT(GetCharacterIndexR({ 0, 12 }) == 9);
		IM_ASSERT(GetCharacterIndexR({ 0, 13 }) == 10);
		IM_ASSERT(GetCharacterIndexR({ 0, 14 }) == 11);
		IM_ASSERT(GetCharacterIndexR({ 0, 15 }) == 11);
		IM_ASSERT(GetCharacterIndexR({ 0, 16 }) == 11);
		IM_ASSERT(GetCharacterIndexR({ 0, 17 }) == 11); // out of range
		IM_ASSERT(GetCharacterIndexR({ 0, 18 }) == 11); // out of range
		// empty line
		IM_ASSERT(GetCharacterIndexR({ 1, 0 }) == 0);
		IM_ASSERT(GetCharacterIndexR({ 1, 1 }) == 0); // out of range
		IM_ASSERT(GetCharacterIndexR({ 1, 2 }) == 0); // out of range
		// nonexistent line
		IM_ASSERT(GetCharacterIndexR({ 2, 0 }) == -1);
		IM_ASSERT(GetCharacterIndexR({ 2, 1 }) == -1);
		IM_ASSERT(GetCharacterIndexR({ 2, 2 }) == -1);
	}

	// --- GetText --- //
	{
		// Gets text from aStart to aEnd, tabs are counted on the start position
		std::string text = GetText({ 0, 0 }, { 0, 1 });
		IM_ASSERT(text.compare(" ") == 0);
		text = GetText({ 0, 1 }, { 0, 2 });
		IM_ASSERT(text.compare("\t") == 0);
		text = GetText({ 0, 2 }, { 0, 3 });
		IM_ASSERT(text.compare("") == 0);
		text = GetText({ 0, 3 }, { 0, 4 });
		IM_ASSERT(text.compare("") == 0);
		text = GetText({ 0, 4 }, { 0, 5 });
		IM_ASSERT(text.compare(" ") == 0);
		text = GetText({ 0, 5 }, { 0, 6 });
		IM_ASSERT(text.compare(" ") == 0);
		text = GetText({ 0, 6 }, { 0, 7 });
		IM_ASSERT(text.compare("\t") == 0);
		text = GetText({ 0, 7 }, { 0, 8 });
		IM_ASSERT(text.compare("") == 0);

		text = GetText({ 0, 0 }, { 0, 8 });
		IM_ASSERT(text.compare(" \t  \t") == 0);
		text = GetText({ 0, 0 }, { 0, 7 });
		IM_ASSERT(text.compare(" \t  \t") == 0);
		text = GetText({ 0, 0 }, { 0, 6 });
		IM_ASSERT(text.compare(" \t  ") == 0);
		text = GetText({ 0, 4 }, { 0, 12 });
		IM_ASSERT(text.compare("  \t   \t") == 0);
		text = GetText({ 0, 4 }, { 0, 13 });
		IM_ASSERT(text.compare("  \t   \t ") == 0);
		text = GetText({ 0, 4 }, { 0, 14 });
		IM_ASSERT(text.compare("  \t   \t \t") == 0);
		text = GetText({ 0, 4 }, { 0, 15 });
		IM_ASSERT(text.compare("  \t   \t \t") == 0);
		text = GetText({ 0, 4 }, { 0, 16 });
		IM_ASSERT(text.compare("  \t   \t \t") == 0);

		text = GetText({ 0, 0 }, { 1, 0 });
		IM_ASSERT(text.compare(" \t  \t   \t \t\n") == 0);
	}

	// --- DeleteRange --- //
	{
		// Deletes from start to end coordinates, any overlapping tabs will be deleted, doesn't allow out of range lines
		DeleteRange({ 0, 0 }, { 0, 0 });
		IM_ASSERT(GetText() == " \t  \t   \t \t\n");
		DeleteRange({ 0, 0 }, { 0, 1 });
		IM_ASSERT(GetText() == "\t  \t   \t \t\n");
		DeleteRange({ 0, 0 }, { 0, 2 });
		IM_ASSERT(GetText() == "  \t   \t \t\n");
		DeleteRange({ 0, 12 }, { 0, 12 });
		IM_ASSERT(GetText() == "  \t   \t \t\n");
		DeleteRange({ 1, 0 }, { 1, 0 });
		IM_ASSERT(GetText() == "  \t   \t \t\n");
		DeleteRange({ 0, 11 }, { 0, 12 });
		IM_ASSERT(GetText() == "  \t   \t \n");
		DeleteRange({ 0, 2 }, { 0, 3 });
		IM_ASSERT(GetText() == "     \t \n");
		DeleteRange({ 0, 6 }, { 0, 7 });
		IM_ASSERT(GetText() == "      \n");
		SetText("a\nb\nc\nd\ne");
		DeleteRange({ 0, 0 }, { 2, 1 });
		IM_ASSERT(GetText() == "\nd\ne");
		DeleteRange({ 1, 1 }, { 2, 0 });
		IM_ASSERT(GetText() == "\nde");
		DeleteRange({ 1, 1 }, { 1, 15 }); // out of range column
		IM_ASSERT(GetText() == "\nd");
		SetText("asdf\nzxcv\nqwer\npo");
		DeleteRange({ 1, 2 }, { 1, 200 }); // out of range column
		IM_ASSERT(GetText() == "asdf\nzx\nqwer\npo");
		DeleteRange({ 0, 500 }, { 2, 500 }); // out of range column
		IM_ASSERT(GetText() == "asdf\npo");
	}

	// --- RemoveGlyphsFromLine --- //
	{
		//
	}


	SetText("asdf asdf\nasdf\nasdf\tasdf\n zxcv zxcv");
	// --- FindNextOccurrence --- //
	{
		Coordinates outStart, outEnd;
		IM_ASSERT(FindNextOccurrence("asdf", 4, { 0, 0 }, outStart, outEnd) && outStart == Coordinates(0, 0) && outEnd == Coordinates(0, 4));
		IM_ASSERT(FindNextOccurrence("asdf", 4, { 0, 1 }, outStart, outEnd) && outStart == Coordinates(0, 5) && outEnd == Coordinates(0, 9));
		IM_ASSERT(FindNextOccurrence("asdf", 4, { 0, 5 }, outStart, outEnd) && outStart == Coordinates(0, 5) && outEnd == Coordinates(0, 9));
		IM_ASSERT(FindNextOccurrence("asdf", 4, { 0, 6 }, outStart, outEnd) && outStart == Coordinates(1, 0) && outEnd == Coordinates(1, 4));
		IM_ASSERT(FindNextOccurrence("asdf", 4, { 3, 3 }, outStart, outEnd) && outStart == Coordinates(0, 0) && outEnd == Coordinates(0, 4)); // go to line 0 if reach end of file
		IM_ASSERT(FindNextOccurrence("zxcv", 4, { 3, 10 }, outStart, outEnd) && outStart == Coordinates(3, 1) && outEnd == Coordinates(3, 5)); // from behind in same line
		IM_ASSERT(!FindNextOccurrence("lalal", 4, { 3, 5 }, outStart, outEnd)); // not found
	}
}
