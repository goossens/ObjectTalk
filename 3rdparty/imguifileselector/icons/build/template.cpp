//	FileSelector - A file select dialog for Dear ImGui.
//	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <unordered_map>

#include "FileSelector.h"


//
//	Icon codepoints
//

GLYPHS


//
//	Icons as embedded data
//

static constexpr size_t iconSize = SIZE;

DATA


//
//	FileSelector::renderFile
//

void FileSelector::renderFile(ImDrawList* drawList, ImVec2 pos, float scale, const std::string& extension) {
	static std::unordered_map<std::string, CodePoint> icons = {
		{".c", fileCode},
		{".cpp", fileCode},
		{".h", fileCode},
		{".hpp", fileCode},
		{".lua", fileCode},
		{".py", fileCode},
		{".txt", fileDocument},
		{".md", fileDocument},
		{".json", fileCog},
		{".csv", fileDelimited},
		{".jpg", fileImage},
		{".jpeg", fileImage},
		{".png", fileImage},
		{".gif", fileImage},
		{".tiff", fileImage},
		{".svg", fileChart},
		{".mp4", fileVideo},
		{".mov", fileVideo},
		{".mp3", fileMusic},
		{".wav", fileMusic},
		{".aiff", fileMusic},
		{".doc", fileWord},
		{".docx", fileWord},
		{".xls", fileExcel},
		{".xlsx", fileExcel},
		{".ppt", filePowerpoint},
		{".pptx", filePowerpoint}
	};

	CodePoint codepoint = file;

	if (icons.find(extension) != icons.end()) {
		codepoint = icons[extension];
	}

	renderIcon(drawList, pos, scale, codepoint);
}


//
//	FileSelector::renderFolder
//

void FileSelector::renderFolder(ImDrawList* drawList, ImVec2 pos, float scale) {
	renderIcon(drawList, pos, scale, folder);
}


//
//	FileSelector::renderIcon
//

void FileSelector::renderIcon(ImDrawList* drawList, ImVec2 pos, float scale, CodePoint codepoint) {
	static ImFont* font = nullptr;

	if (!font) {
		ImFontConfig config;
		std::copy_n("Icons", 6, config.Name);
		config.FontDataOwnedByAtlas = false;

		font = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
			static_cast<void*>(iconData),
			static_cast<int>(iconSize),
			0.0f,
			&config);
	}

	drawList->AddText(
		font,
		ImGui::GetStyle().FontSizeBase * scale,
		pos,
		ImGui::GetColorU32(ImGuiCol_TextDisabled),
		reinterpret_cast<const char*>(codepoint));
}
