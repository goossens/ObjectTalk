//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>
#include <vector>

#include "imgui.h"
#include "md4c.h"


//
//	OtMarkdown
//

class OtMarkdown {
public:
	// constructor/destructor
	OtMarkdown();
	~OtMarkdown();

	// render the specified markdown as an ImGui widget
	void render(const std::string& markdown);

private:
	// parser and its callbacks
	MD_PARSER md;
	int block(MD_BLOCKTYPE type, void* data, bool enter);
	int span(MD_SPANTYPE type, void* data, bool enter);
	int text(MD_TEXTTYPE type, const char* text, const char* end);

	// block callbacks
	void docBlock(bool enter);
	void quoteBlock(bool enter);
	void ulBlock(const MD_BLOCK_UL_DETAIL* detail, bool enter);
	void olBlock(const MD_BLOCK_OL_DETAIL* detail, bool enter);
	void liBlock(const MD_BLOCK_LI_DETAIL* detail, bool enter);
	void hrBlock(bool enter);
	void hBlock(const MD_BLOCK_H_DETAIL*  detail, bool  enter);
	void codeBlock(const MD_BLOCK_CODE_DETAIL* detail, bool enter);
	void htmlBlock(bool enter);
	void pBlock(bool enter);
	void tableBlock(const MD_BLOCK_TABLE_DETAIL* detail, bool enter);
	void theadBlock(bool enter);
	void tbodyBlock(bool enter);
	void trBlock(bool enter);
	void thBlock(const MD_BLOCK_TD_DETAIL* detail, bool enter);
	void tdBlock(const MD_BLOCK_TD_DETAIL* detail, bool enter);

	// span callbacks
	void emSpan(bool enter);
	void strongSpan(bool enter);
	void delSpan(bool enter);
	void aSpan(const MD_SPAN_A_DETAIL* detail, bool enter);
	void imgSpan(const MD_SPAN_IMG_DETAIL* detail, bool enter);
	void codeSpan(bool enter);
	void latexMathSpan(bool enter);
	void latexMathDisplaySpan(bool enter);
	void wikiLinkSpan(const MD_SPAN_WIKILINK_DETAIL* detail, bool enter);
	void uSpan(bool enter);

	// current state
	bool isUnderlined = false;
	bool isStrikeThrough = false;
	bool isEm = false;
	bool isStrong = false;
	bool isTableHeader = false;
	bool isTableBody = false;
	bool isImage = false;
	int hLevel = 0;

	bool tableBorder = true;
	bool tableHeaderHighlight = true;
	size_t tableNextColumn = 0;
	ImVec2 tableLastPos;
	std::vector<float> tableColPos;
	std::vector<float> tableRowPos;

	struct imageInfo {
		ImTextureID	id;
		ImVec2	size;
	};

	struct listInfo {
		char delimiter;
		bool isOL;
		unsigned currentOL;
	};

	std::vector<listInfo> listStack;

	std::string href;

	// utility functions
	void renderText(const char* text, const char* end);
	void renderHtml(const char* text, const char* end);
	void renderEntity(const char* text, const char* end);
	void line(ImColor color, bool under);

	ImFont* getFont();
	void setFont(bool enter);
	ImVec4 getColor();
	void setColor(bool enter);
	void setHref(bool enter, const MD_ATTRIBUTE& src);
	bool getImage(imageInfo& nfo);
	void openUrl();
};
