//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
#include "OtUi.h"


//
//	OtMarkdown::OtMarkdown
//

OtMarkdown::OtMarkdown() {
	md.abi_version = 0;

	md.flags =
		MD_FLAG_TABLES |
		MD_FLAG_UNDERLINE |
		MD_FLAG_WIKILINKS |
		MD_FLAG_TASKLISTS |
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
//	OtMarkdown::render
//

void OtMarkdown::render(const std::string& markdown) {
	fontSize = ImGui::GetStyle().FontSizeBase;
	md_parse(markdown.c_str(), MD_SIZE(markdown.size()), &md, this);
}


//
//	OtMarkdown::docBlock
//

void OtMarkdown::docBlock([[maybe_unused]] bool enter) {
}


//
//	OtMarkdown::quoteBlock
//

void OtMarkdown::quoteBlock(bool enter) {
	if (enter) {
		ImGui::Indent();

		quoteDepth++;
		quoteStart.push_back(ImGui::GetCursorScreenPos());
		admonitionScanPending = true;
		admonitionKind = AdmonitionKind::None;

	} else {
		// draw a vertical bar on the left side of the quote
		if (!quoteStart.empty()) {
			float startY = quoteStart.back().y;
			float indentedX = quoteStart.back().x;
			quoteStart.pop_back();

			float endY = ImGui::GetCursorScreenPos().y;
			float barX = indentedX - ImGui::GetStyle().IndentSpacing * 0.5f;
			float thickness = (admonitionKind != AdmonitionKind::None) ? 3.0f : 2.0f;

			ImColor barColor = (admonitionKind != AdmonitionKind::None)
				? ImColor(admonitionColor(admonitionKind))
				: ImColor(ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);

			ImGui::GetWindowDrawList()->AddLine(
				ImVec2(barX, startY), ImVec2(barX, endY),
				barColor,
				thickness);
		}

		ImGui::Unindent();
		quoteDepth--;
		admonitionKind = AdmonitionKind::None;
		admonitionScanPending = false;
		gap(afterQuoteGap);
	}
}


//
//	OtMarkdown::ulBlock
//

void OtMarkdown::ulBlock(const MD_BLOCK_UL_DETAIL* detail, bool enter) {
	if (enter) {
		listStack.push_back(ListInfo{detail->mark, false, 0});

	} else {
		listStack.pop_back();

		if (listStack.empty()) {
			ImGui::NewLine();
			gap(afterListGap);
		}
	}
}


//
//	OtMarkdown::olBlock
//

void OtMarkdown::olBlock(const MD_BLOCK_OL_DETAIL* detail, bool enter) {
	if (enter) {
		listStack.push_back(ListInfo{detail->mark_delimiter, true, detail->start});

	} else {
		listStack.pop_back();

		if (listStack.empty()) {
			ImGui::NewLine();
			gap(afterListGap);
		}
	}}


//
//	OtMarkdown::liBlock
//

void OtMarkdown::liBlock(const MD_BLOCK_LI_DETAIL* detail, bool enter) {
	if (enter) {
		gap();

		ListInfo& info = listStack.back();

		if (detail && detail->is_task) {
			renderTaskMarker(detail->task_mark == 'x' || detail->task_mark == 'X');
			ImGui::SameLine();

			if (info.isOL) {
				++info.currentOL;
		}

		} else if (info.isOL) {
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
		ImGui::Separator();
	}
}


//
//	OtMarkdown::hBlock
//

void OtMarkdown::hBlock(const MD_BLOCK_H_DETAIL* detail, bool enter) {
	if (enter) {
		hLevel = detail->level;
		setFont(enter);

	} else {
		ImGui::NewLine();
		hLevel = 0;
		setFont(enter);

		if (detail->level <= 2) {
			ImGui::Separator();
		}

		gap(afterHeaderGap);
	}
}


//
//	OtMarkdown::codeBlock
//

void OtMarkdown::codeBlock([[maybe_unused]] const MD_BLOCK_CODE_DETAIL* detail, bool enter) {
    if (enter) {
		auto font = getFont();
		ImGui::PushFont(font.font, font.size);

		auto color = ImGui::GetStyle().Colors[ImGuiCol_Text];
		color.x *= 0.5f;
		ImGui::PushStyleColor(ImGuiCol_Text, color);

	} else {
		ImGui::PopStyleColor();
		ImGui::PopFont();
		gap(afterCodeGap);
	}
}


//
//	OtMarkdown::htmlBlock
//

void OtMarkdown::htmlBlock([[maybe_unused]] bool  enter) {
}


//
//	OtMarkdown::pBlock
//

void OtMarkdown::pBlock(bool enter) {
	if (!enter && listStack.empty()) {
		ImGui::NewLine();
		gap(afterParagraphGap);
	}
}


//
//	OtMarkdown::tableBlock
//

void OtMarkdown::tableBlock([[maybe_unused]] const MD_BLOCK_TABLE_DETAIL* detail, bool enter) {
	if (enter) {
		ImGui::PushID(tableID++);

		ImGuiTableFlags flags =
			ImGuiTableFlags_BordersInnerV |
			ImGuiTableFlags_BordersOuterV |
			ImGuiTableFlags_BordersOuterH |
			ImGuiTableFlags_BordersInnerH;

		tableOpen = ImGui::BeginTable("##md", detail->col_count, flags);

	} else {
		if (tableOpen) {
			ImGui::EndTable();
		}

		tableOpen = false;
		ImGui::PopID();
	}
}


//
//	OtMarkdown::theadBlock
//

void OtMarkdown::theadBlock(bool enter) {
	isTableHeader = enter;
	setFont(enter);
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
	if (tableOpen && enter) {
		ImGui::TableNextRow();
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

void OtMarkdown::tdBlock([[maybe_unused]] const MD_BLOCK_TD_DETAIL* detail, bool enter) {
	if (tableOpen) {
		if (enter) {
			ImGui::TableNextColumn();
			cellAlign = detail->align;

			if (cellAlign != MD_ALIGN_DEFAULT && cellAlign != MD_ALIGN_LEFT) {
				cellWidth = ImGui::GetContentRegionAvail().x;
				vtxStart = ImGui::GetWindowDrawList()->VtxBuffer.Size;
				ImGui::BeginGroup();
			}


		} else {
			if (cellAlign != MD_ALIGN_DEFAULT && cellAlign != MD_ALIGN_LEFT) {
				ImGui::EndGroup();
				float contentWidth = ImGui::GetItemRectSize().x;

				if (contentWidth < cellWidth) {
					float offset = (cellAlign == MD_ALIGN_CENTER)
						? (cellWidth - contentWidth) * 0.5f
						: (cellWidth - contentWidth);

					if (offset > 0.0f) {
						ImDrawList* drawList = ImGui::GetWindowDrawList();

						for (int i = vtxStart; i < drawList->VtxBuffer.Size; i++) {
							drawList->VtxBuffer[i].pos.x += offset;
						}
					}
				}
			}

			cellAlign = MD_ALIGN_DEFAULT;
		}
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
		ImageInfo info;

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

void OtMarkdown::codeSpan([[maybe_unused]] bool enter) {
}


//
//	OtMarkdown::latexMathSpan
//

void OtMarkdown::latexMathSpan([[maybe_unused]] bool enter) {
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

void OtMarkdown::wikiLinkSpan([[maybe_unused]] const MD_SPAN_WIKILINK_DETAIL* detail, [[maybe_unused]] bool enter) {
}


//
//	OtMarkdown::uSpan
//

void OtMarkdown::uSpan([[maybe_unused]] bool enter) {
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
		case MD_TEXT_NULLCHAR: break;
		case MD_TEXT_BR: gap(); break;
		case MD_TEXT_SOFTBR: gap(); break;
		case MD_TEXT_ENTITY: renderEntity(text, end); break;
		case MD_TEXT_HTML: renderHtml(text, end); break;
		case MD_TEXT_LATEXMATH: renderText(text, end); break;
	}

	return 0;
}


//
//	OtMarkdown::renderText
//

void OtMarkdown::renderText(const char* text, const char* end) {
	if (admonitionScanPending) {
		admonitionScanPending = false;
		const char* markerEnd = nullptr;
		AdmonitionKind kind = matchAdmonitionMarker(text, end, markerEnd);

		if (kind != AdmonitionKind::None) {
			admonitionKind = kind;
			ImGui::PushStyleColor(ImGuiCol_Text, admonitionColor(kind));
			bool savedStrong = isStrong;
			isStrong = true;

			setFont(true);
			ImGui::TextUnformatted(admonitionLabel(kind));
			setFont(false);

			isStrong = savedStrong;
			ImGui::PopStyleColor();

			while (markerEnd < end && (*markerEnd == ' ' || *markerEnd == '\t')) {
				markerEnd++;
			}

			if (markerEnd < end) {
				renderText(markerEnd, end);
			}

			return;
		}
	}

	float scale = ImGui::GetStyle().FontScaleMain;
	ImGuiStyle& s = ImGui::GetStyle();
	bool isLF = false;

	while (!isImage && text < end) {
		const char* te = end;

		if (!isTableHeader) {
			float wl = ImGui::GetContentRegionAvail().x;

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
		gap();

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
//	OtMarkdown::renderTaskMarker
//

void OtMarkdown::renderTaskMarker(bool checked) {
	// draw a small bordered square, with a check mark if `checked`
	float h = ImGui::GetFontSize();
	float sz = h * 0.75f;
	ImVec2 cursor = ImGui::GetCursorScreenPos();
	float y_center = cursor.y + h * 0.5f;
	ImVec2 tl(cursor.x, y_center - sz * 0.5f);
	ImVec2 br(cursor.x + sz, y_center + sz * 0.5f);
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	ImU32 border = ImGui::GetColorU32(ImGuiCol_Text);
	float rounding = sz * 0.15f;
	float thickness = (sz / 12.0f > 1.0f) ? (sz / 12.0f) : 1.0f;
	drawList->AddRect(tl, br, border, rounding, 0, thickness);

	if (checked) {
		ImU32 mark = ImGui::GetColorU32(ImGuiCol_CheckMark);
		float pad = sz * 0.22f;
		ImVec2 a(tl.x + pad, tl.y + sz * 0.55f);
		ImVec2 b(tl.x + sz * 0.45f, br.y - pad);
		ImVec2 c(br.x - pad, tl.y + pad);
		float check_thick = (sz / 8.0f > 1.5f) ? (sz / 8.0f) : 1.5f;
		drawList->AddLine(a, b, mark, check_thick);
		drawList->AddLine(b, c, mark, check_thick);
	}

	// reserve horizontal space
	ImGui::Dummy(ImVec2(sz, h));
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
//	OtMarkdown::admonitionColor
//

ImVec4 OtMarkdown::admonitionColor(AdmonitionKind kind) {
	switch (kind) {
		case AdmonitionKind::Note:      return ImVec4(0.35f, 0.65f, 1.00f, 1.0f);
		case AdmonitionKind::Tip:       return ImVec4(0.25f, 0.73f, 0.32f, 1.0f);
		case AdmonitionKind::Important: return ImVec4(0.82f, 0.60f, 0.97f, 1.0f);
		case AdmonitionKind::Warning:   return ImVec4(0.95f, 0.75f, 0.22f, 1.0f);
		case AdmonitionKind::Caution:   return ImVec4(0.97f, 0.32f, 0.29f, 1.0f);
		default: return ImGui::GetStyle().Colors[ImGuiCol_TextDisabled];
	}
}


//
//	OtMarkdown::admonitionLabel
//

const char* OtMarkdown::admonitionLabel(AdmonitionKind kind) {
	switch (kind) {
		case AdmonitionKind::Note:      return "NOTE";
		case AdmonitionKind::Tip:       return "TIP";
		case AdmonitionKind::Important: return "IMPORTANT";
		case AdmonitionKind::Warning:   return "WARNING";
		case AdmonitionKind::Caution:   return "CAUTION";
		default: return "";
	}
}


//
//	OtMarkdown::matchAdmonitionMarker
//

OtMarkdown::AdmonitionKind OtMarkdown::matchAdmonitionMarker(const char* start, const char* end, const char*& markerEnd) {
	if (end - start < 4 || start[0] != '[' || start[1] != '!') {
		return AdmonitionKind::None;
	}

	const char* p = start + 2;
	const char* close = p;

	while (close < end && *close != ']') {
		close++;
	}

	if (close >= end) {
		return AdmonitionKind::None;
	}

	auto eq = [](const char* a, const char* aEnd, const char* b) {
		size_t n = (size_t)(aEnd - a);

		if (strlen(b) != n) {
			return false;
		}

		for (size_t i = 0; i < n; i++) {
			char ca = a[i];

			if (ca >= 'a' && ca <= 'z') {
				ca = (char)(ca - 'a' + 'A');
			}

			if (ca != b[i]) {
				return false;
			}
		}

		return true;
	};

	AdmonitionKind kind = AdmonitionKind::None;

	if      (eq(p, close, "NOTE"))      { kind = AdmonitionKind::Note; }
	else if (eq(p, close, "TIP"))       { kind = AdmonitionKind::Tip; }
	else if (eq(p, close, "IMPORTANT")) { kind = AdmonitionKind::Important; }
	else if (eq(p, close, "WARNING"))   { kind = AdmonitionKind::Warning; }
	else if (eq(p, close, "CAUTION"))   { kind = AdmonitionKind::Caution; }

	if (kind != AdmonitionKind::None) {
		markerEnd = close + 1;
	}

	return kind;
}


//
//	OtMarkdown::getFont
//

OtMarkdown::FontInfo OtMarkdown::getFont() {
	if (isTableHeader) {
		return FontInfo{OtUi::getBoldFont(), 0.0f};
	}

	switch (hLevel) {
		case 0: return isStrong ? FontInfo{OtUi::getBoldFont(), 0.0f} : FontInfo{OtUi::getTextFont(), 0.0f};
		case 1: return FontInfo{OtUi::getBoldFont(), fontSize * 1.75f};
		case 2: return FontInfo{OtUi::getBoldFont(), fontSize * 1.5f};
		case 3: return FontInfo{OtUi::getBoldFont(), fontSize * 1.25f};
		default: return FontInfo{OtUi::getBoldFont(), fontSize};
	}
}


//
//	OtMarkdown::setFont
//

void OtMarkdown::setFont(bool enter) {
	if (enter) {
		auto font = getFont();
		ImGui::PushFont(font.font, font.size);

	} else {
		ImGui::PopFont();
	}
}


//
//	OtMarkdown::getColor
//

ImVec4 OtMarkdown::getColor() {
	if (href.empty()) {
		return	ImGui::GetStyle().Colors[ImGuiCol_Text];

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

bool OtMarkdown::getImage(ImageInfo& info) {
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


//
//	OtMarkdown::gap
//

void OtMarkdown::gap(float size) {
	ImGui::Dummy(ImVec2(0.0f, fontSize * size));
}
