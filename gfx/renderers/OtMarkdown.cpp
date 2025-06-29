//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Inspired by https://github.com/mekhontsev/imgui_md.
//


//
//	Include files
//

#include "OtFramework.h"
#include "OtMarkdown.h"


//
//	OtMarkdown::OtMarkdown
//

OtMarkdown::OtMarkdown() {
	md.abi_version = 0;

	md.flags =
		MD_FLAG_TABLES |
		MD_FLAG_WIKILINKS |
		MD_FLAG_STRIKETHROUGH;

	md.enter_block = [](MD_BLOCKTYPE t, void* d, void* u) {
		return ((OtMarkdown*) u)->block(t, d, true);
	};

	md.leave_block = [](MD_BLOCKTYPE t, void* d, void* u) {
		return ((OtMarkdown*) u)->block(t, d, false);
	};

	md.enter_span = [](MD_SPANTYPE t, void* d, void* u) {
		return ((OtMarkdown*) u)->span(t, d, true);
	};

	md.leave_span = [](MD_SPANTYPE t, void* d, void* u) {
		return ((OtMarkdown*) u)->span(t, d, false);
	};

	md.text = [](MD_TEXTTYPE t, const MD_CHAR* text, MD_SIZE size, void* u) {
		return ((OtMarkdown*) u)->text(t, text, text + size);
	};

	md.debug_log = nullptr;
	md.syntax = nullptr;
}


//
//	OtMarkdown::~OtMarkdown
//

OtMarkdown::~OtMarkdown() {
}


//
//	OtMarkdown::render
//

void OtMarkdown::render(const std::string& markdown) {
	md_parse(markdown.c_str(), MD_SIZE(markdown.size()), &md, this);
}


//
//	OtMarkdown::docBlock
//

void OtMarkdown::docBlock(bool /* enter */) {
}


//
//	OtMarkdown::quoteBlock
//

void OtMarkdown::quoteBlock(bool /* enter */) {
}


//
//	OtMarkdown::ulBlock
//

void OtMarkdown::ulBlock(const MD_BLOCK_UL_DETAIL* detail, bool enter) {
	if (enter) {
		listStack.push_back(listInfo{detail->mark, false, 0});

	} else {
		listStack.pop_back();

		if (listStack.empty()) {
			ImGui::NewLine();
		}
	}
}


//
//	OtMarkdown::olBlock
//

void OtMarkdown::olBlock(const MD_BLOCK_OL_DETAIL* detail, bool enter) {
	if (enter) {
		listStack.push_back(listInfo{detail->mark_delimiter, true, detail->start});

	} else {
		listStack.pop_back();

		if (listStack.empty()) {
			ImGui::NewLine();
		}
	}}


//
//	OtMarkdown::liBlock
//

void OtMarkdown::liBlock(const MD_BLOCK_LI_DETAIL* /* detail */, bool enter) {
	if (enter) {
		ImGui::NewLine();

		listInfo& info = listStack.back();

		if (info.isOL) {
			ImGui::Text("%d%c", info.currentOL++, info.delimiter);
			ImGui::SameLine();

		} else {
			if (info.delimiter == '*') {
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::GetStyle().FramePadding.x * 2);
				ImGui::Bullet();

			} else {
				ImGui::Text("%c", info.delimiter);
				ImGui::SameLine();
			}
		}

		ImGui::Indent();

	} else {
		ImGui::Unindent();
	}
}


//
//	OtMarkdown::hrBlock
//

inline void OtMarkdown::hrBlock(bool enter) {
	if (!enter) {
		ImGui::NewLine();
		ImGui::Separator();
	}
}


//
//	OtMarkdown::hBlock
//

void OtMarkdown::hBlock(const MD_BLOCK_H_DETAIL* detail, bool enter) {
	if (enter) {
		hLevel = detail->level;
		ImGui::NewLine();

	} else {
		hLevel = 0;
	}

	setFont(enter);

	if (!enter) {
		if (detail->level <= 2) {
			ImGui::NewLine();
			ImGui::Separator();
		}
	}
}


//
//	OtMarkdown::codeBlock
//

void OtMarkdown::codeBlock(const MD_BLOCK_CODE_DETAIL* /* detail */, bool /* enter */) {
}


//
//	OtMarkdown::htmlBlock
//

void OtMarkdown::htmlBlock(bool /* enter */) {
}


//
//	OtMarkdown::pBlock
//

void OtMarkdown::pBlock(bool /* enter */) {
	if (listStack.empty()) {
		ImGui::NewLine();
	}
}


//
//	OtMarkdown::tableBlock
//

void OtMarkdown::tableBlock(const MD_BLOCK_TABLE_DETAIL* /* detail */, bool enter) {
	if (enter) {
		tableRowPos.clear();
		tableColPos.clear();
		tableLastPos = ImGui::GetCursorPos();

	} else {
		ImGui::NewLine();

		if (tableBorder) {
			tableLastPos.y = ImGui::GetCursorPos().y;

			tableColPos.push_back(tableLastPos.x);
			tableRowPos.push_back(tableLastPos.y);

			const ImVec2 wp = ImGui::GetWindowPos();
			const ImVec2 sp = ImGui::GetStyle().ItemSpacing;
			const float wx = wp.x + sp.x / 2;
			const float wy = wp.y - sp.y / 2 - ImGui::GetScrollY();

			for (size_t i = 0; i < tableColPos.size(); ++i) {
				tableColPos[i] += wx;
			}

			for (size_t i = 0; i < tableRowPos.size(); ++i) {
				tableRowPos[i] += wy;
			}

			const ImColor color = ImGui::GetStyle().Colors[ImGuiCol_TextDisabled];
			ImDrawList* dl = ImGui::GetWindowDrawList();
			const float xmin = tableColPos.front();
			const float xmax = tableColPos.back();

			for (size_t i = 0; i < tableRowPos.size(); ++i) {
				const float p = tableRowPos[i];
				dl->AddLine(ImVec2(xmin, p), ImVec2(xmax, p), color, i == 1 && tableHeaderHighlight ? 2.0f : 1.0f);
			}

			const float ymin = tableRowPos.front();
			const float ymax = tableRowPos.back();

			for (size_t i = 0; i < tableColPos.size(); i++) {
				const float p = tableColPos[i];
				dl->AddLine(ImVec2(p, ymin), ImVec2(p, ymax), color, 1.0f);
			}
		}
	}
}


//
//	OtMarkdown::theadBlock
//

void OtMarkdown::theadBlock(bool enter) {
	isTableHeader = enter;

	if (tableHeaderHighlight) {
		setFont(enter);
	}
}


//
//	OtMarkdown::tbodyBlock
//

void OtMarkdown::tbodyBlock(bool enter) {
	isTableBody = enter;
}


//
//	OtMarkdown::trBlock
//

void OtMarkdown::trBlock(bool enter) {
	ImGui::SetCursorPosY(tableLastPos.y);

	if (enter) {
		tableNextColumn = 0;
		ImGui::NewLine();
		tableRowPos.push_back(ImGui::GetCursorPosY());
	}
}


//
//	OtMarkdown::thBlock
//

void OtMarkdown::thBlock(const MD_BLOCK_TD_DETAIL* detail, bool enter) {
	tdBlock(detail, enter);
}


//
//	OtMarkdown::tdBlock
//

void OtMarkdown::tdBlock(const MD_BLOCK_TD_DETAIL* /* detail */, bool enter) {
	if (enter) {
		if (tableNextColumn < tableColPos.size()) {
			ImGui::SetCursorPosX(tableColPos[tableNextColumn]);

		} else {
			tableColPos.push_back(tableLastPos.x);
		}

		++tableNextColumn;
		ImGui::Indent(tableColPos[tableNextColumn - 1]);
		ImGui::SetCursorPos(ImVec2(tableColPos[tableNextColumn - 1], tableRowPos.back()));

	} else {
		const ImVec2 p = ImGui::GetCursorPos();
		ImGui::Unindent(tableColPos[tableNextColumn - 1]);
		ImGui::SetCursorPosX(p.x);

		if (p.y > tableLastPos.y) {
			tableLastPos.y = p.y;
		}
	}
	ImGui::TextUnformatted("");

	if (!tableBorder && enter && tableNextColumn == 1) {
		ImGui::SameLine(0.0f, 0.0f);

	} else {
		ImGui::SameLine();
	}
}


//
//	OtMarkdown::emSpan
//

void OtMarkdown::emSpan(bool enter) {
	isEm = enter;
	setFont(enter);
}


//
//	OtMarkdown::strongSpan
//

void OtMarkdown::strongSpan(bool enter) {
	isStrong = enter;
	setFont(enter);
}


//
//	OtMarkdown::delSpan
//

void OtMarkdown::delSpan(bool enter) {
	isStrikeThrough = enter;
}


//
//	OtMarkdown::aSpan
//

void OtMarkdown::aSpan(const MD_SPAN_A_DETAIL* detail, bool enter) {
	setHref(enter, detail->href);
	setColor(enter);
}


//
//	OtMarkdown::imgSpan
//

void OtMarkdown::imgSpan(const MD_SPAN_IMG_DETAIL* detail, bool enter) {
	isImage = enter;
	setHref(enter, detail->src);

	if (enter) {
		imageInfo info;

		if (getImage(info)) {
			const float scale = ImGui::GetStyle().FontScaleMain;
			info.size.x *= scale;
			info.size.y *= scale;
			ImVec2 const csz = ImGui::GetContentRegionAvail();

			if (info.size.x > csz.x) {
				const float r = info.size.y / info.size.x;
				info.size.x = csz.x;
				info.size.y = csz.x * r;
			}

			ImGui::Image(info.id, info.size);

			if (ImGui::IsItemHovered()) {
				if (ImGui::IsMouseReleased(0)) {
					openUrl();
				}
			}
		}
	}
}


//
//	OtMarkdown::codeSpan
//

void OtMarkdown::codeSpan(bool /* enter */) {
}


//
//	OtMarkdown::latexMathSpan
//

void OtMarkdown::latexMathSpan(bool /* enter */) {
}


//
//	OtMarkdown::latexMathDisplaySpan
//

void OtMarkdown::latexMathDisplaySpan(bool enter) {
	isUnderlined = enter;
}


//
//	OtMarkdown::wikiLinkSpan
//

void OtMarkdown::wikiLinkSpan(const MD_SPAN_WIKILINK_DETAIL* /* detail */, bool /* enter */) {
}


//
//	OtMarkdown::uSpan
//

void OtMarkdown::uSpan(bool /* enter */) {
}


//
//	OtMarkdown::block
//

int OtMarkdown::block(MD_BLOCKTYPE type, void* data, bool enter) {
	switch (type) {
		case MD_BLOCK_DOC: docBlock(enter); break;
		case MD_BLOCK_QUOTE: quoteBlock(enter); break;
		case MD_BLOCK_UL: ulBlock((MD_BLOCK_UL_DETAIL*) data, enter); break;
		case MD_BLOCK_OL: olBlock((MD_BLOCK_OL_DETAIL*) data, enter); break;
		case MD_BLOCK_LI: liBlock((MD_BLOCK_LI_DETAIL*) data, enter); break;
		case MD_BLOCK_HR: hrBlock(enter); break;
		case MD_BLOCK_H: hBlock((MD_BLOCK_H_DETAIL*) data, enter); break;
		case MD_BLOCK_CODE: codeBlock((MD_BLOCK_CODE_DETAIL*) data, enter); break;
		case MD_BLOCK_HTML: htmlBlock(enter); break;
		case MD_BLOCK_P: pBlock(enter); break;
		case MD_BLOCK_TABLE: tableBlock((MD_BLOCK_TABLE_DETAIL*) data, enter); break;
		case MD_BLOCK_THEAD: theadBlock(enter); break;
		case MD_BLOCK_TBODY: tbodyBlock(enter); break;
		case MD_BLOCK_TR: trBlock(enter); break;
		case MD_BLOCK_TH: thBlock((MD_BLOCK_TD_DETAIL*) data, enter); break;
		case MD_BLOCK_TD: tdBlock((MD_BLOCK_TD_DETAIL*) data, enter); break;
	}

	return 0;
}


//
//	OtMarkdown::span
//

int OtMarkdown::span(MD_SPANTYPE type, void* data, bool enter) {
	switch (type) {
		case MD_SPAN_EM: emSpan(enter); break;
		case MD_SPAN_STRONG: strongSpan(enter); break;
		case MD_SPAN_DEL: delSpan(enter); break;
		case MD_SPAN_A: aSpan((MD_SPAN_A_DETAIL*) data, enter); break;
		case MD_SPAN_IMG: imgSpan((MD_SPAN_IMG_DETAIL*) data, enter); break;
		case MD_SPAN_CODE: codeSpan(enter); break;
		case MD_SPAN_LATEXMATH: latexMathSpan(enter); break;
		case MD_SPAN_LATEXMATH_DISPLAY: latexMathDisplaySpan(enter); break;
		case MD_SPAN_WIKILINK: wikiLinkSpan((MD_SPAN_WIKILINK_DETAIL*) data, enter); break;
		case MD_SPAN_U: uSpan(enter); break;
	}

	return 0;
}


//
//	OtMarkdown::text
//

int OtMarkdown::text(MD_TEXTTYPE type, const char* text, const char* end) {
	switch (type) {
		case MD_TEXT_NORMAL: renderText(text, end); break;
		case MD_TEXT_CODE: renderText(text, end); break;
		case MD_TEXT_NULLCHAR:  break;
		case MD_TEXT_BR: ImGui::NewLine(); break;
		case MD_TEXT_SOFTBR: ImGui::NewLine();; break;
		case MD_TEXT_ENTITY: renderEntity(text, end); break;
		case MD_TEXT_HTML:  renderHtml(text, end); break;
		case MD_TEXT_LATEXMATH: renderText(text, end); break;
	}

	if (isTableHeader) {
		const float x = ImGui::GetCursorPosX();
		if (x > tableLastPos.x) {
			tableLastPos.x = x;
		}
	}

	return 0;
}


//
//	OtMarkdown::renderText
//

void OtMarkdown::renderText(const char* text, const char* end) {
	float scale = ImGui::GetStyle().FontScaleMain;
	ImGuiStyle& s = ImGui::GetStyle();
	bool isLF = false;

	while (!isImage && text < end) {
		const char* te = end;

		if (!isTableHeader) {
			float wl = ImGui::GetContentRegionAvail().x;

			if (isTableBody) {
				wl = (tableNextColumn < tableColPos.size() ? tableColPos[tableNextColumn] : tableLastPos.x);
				wl -= ImGui::GetCursorPosX();
			}

			auto font = ImGui::GetFont();
			te = font->CalcWordWrapPosition(font->LegacySize * scale, text, end, wl);

			if (te == text) {
				te++;
			}
		}

		ImGui::TextUnformatted(text, te);

		if (te > text && *(te - 1) == '\n') {
			isLF = true;
		}

		if (!href.empty()) {
			ImVec4 color;

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("%s", href.c_str());
				color = s.Colors[ImGuiCol_ButtonHovered];

				if (ImGui::IsMouseReleased(0)) {
					openUrl();
				}

			} else {
				color = s.Colors[ImGuiCol_Button];
			}

			line(color, true);
		}

		if (isUnderlined) {
			line(s.Colors[ImGuiCol_Text], true);
		}

		if (isStrikeThrough) {
			line(s.Colors[ImGuiCol_Text], false);
		}

		text = te;

		while (text < end && *text == ' ') {
			text++;
		}
	}

	if (!isLF) {
		ImGui::SameLine(0.0f, 0.0f);
	}
}


//
//	OtMarkdown::renderHtml
//

void OtMarkdown::renderHtml(const char* text, const char* end) {
	std::string html(text, end - text);

	if (html == "<br>") {
		ImGui::NewLine();

	} else if (html == "<hr>") {
		ImGui::Separator();

	} else if (html == "<u>") {
		isUnderlined = true;

	} else if (html == "</u>") {
		isUnderlined = false;

	} else {
		renderText(text, end);
	}
}


//
//	OtMarkdown::renderEntity
//

void OtMarkdown::renderEntity(const char* text, const char* end) {
	static struct {
		std::string entity;
		char character;
	} mappings[] = {
		{"&amp;", '&'},
		{"&apos;", '\''},
		{"&gt;", '>'},
		{"&lt;", '<'},
		{"&nbsp;", ' '},
		{"&quot;", '"'}
	};

	std::string entity(text, end - text);

	for (auto& mapping : mappings) {
		if (mapping.entity == entity) {
			char string[2];
			string[0] = mapping.character;
			string[1] = 0;
			renderText(string, string + 1);
			return;
		}
	}

	renderText(text, end);
}


//
//	OtMarkdown::line
//

void OtMarkdown::line(ImColor color, bool under) {
	ImVec2 mi = ImGui::GetItemRectMin();
	ImVec2 ma = ImGui::GetItemRectMax();

	if (!under) {
		ma.y -= ImGui::GetFontSize() / 2.0f;
	}

	mi.y = ma.y;
	ImGui::GetWindowDrawList()->AddLine(mi, ma, color, 1.0f);
}


//
//	OtMarkdown::getFont
//

ImFont* OtMarkdown::getFont() {
	return nullptr;

#if 0
	if (isTableHeader) {
		return g_font_bold;
	}

	switch (m_hlevel)
	{
	case 0:
		return m_is_strong ? g_font_bold : g_font_regular;
	case 1:
		return g_font_bold_large;
	default:
		return g_font_bold;
	}
#endif
}


//
//	OtMarkdown::setFont
//

void OtMarkdown::setFont(bool enter) {
	if (enter) {
		ImGui::PushFont(getFont(), 0.0f);

	} else {
		ImGui::PopFont();
	}
}


//
//	OtMarkdown::getColor
//

ImVec4 OtMarkdown::getColor() {
	if (href.empty()) {
		return  ImGui::GetStyle().Colors[ImGuiCol_Text];

	} else {
		return ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered];
	}
}


//
//	OtMarkdown::setColor
//

void OtMarkdown::setColor(bool enter) {
	if (enter) {
		ImGui::PushStyleColor(ImGuiCol_Text, getColor());

	} else {
		ImGui::PopStyleColor();
	}
}


//
//	OtMarkdown::setHref
//

void OtMarkdown::setHref(bool enter, const MD_ATTRIBUTE& src) {
	if (enter) {
		href.assign(src.text, src.size);

	} else {
		href.clear();
	}
}


//
//	OtMarkdown::getImage
//

bool OtMarkdown::getImage(imageInfo& info) {
//	info.id = ImGui::GetIO().Fonts->TexID;
	info.size = {100.0f, 50.0f};
	return true;
}


//
//	OtMarkdown::openUrl
//

void OtMarkdown::openUrl() {
	OtFramework::openURL(href);
}
