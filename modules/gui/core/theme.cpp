//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <filesystem>

#include "ot/libuv.h"

#include "theme.h"


//
//	globals
//

int OtTheme::width = 1280;
int OtTheme::height = 720;

ImU32 OtTheme::color100;
ImU32 OtTheme::color80;
ImU32 OtTheme::color60;
ImU32 OtTheme::color40;
ImU32 OtTheme::color20;
ImU32 OtTheme::background;

ImFont* OtTheme::robotoFont = nullptr;
ImFont* OtTheme::dsegFont = nullptr;
ImFont* OtTheme::dsegBoldFont = nullptr;


//
//	OtStyle::init
//

void OtTheme::init() {
	// set default screen size (720p)
	width = 1280;
	height = 720;

	// calculate colors
	background = IM_COL32(0, 12, 24, 255);
	color100 = IM_COL32(100, 180, 255, 255);
	color80 = IM_COL32(80, 144, 200, 255);
	color60 = IM_COL32(60, 108, 150, 255);
	color40 = IM_COL32(40, 72, 100, 255);
	color20 = IM_COL32(20, 36, 50, 255);

	// figure out where we live
	char buffer[PATH_MAX];
	size_t length = PATH_MAX;
	auto status = uv_exepath(buffer, &length);
	UV_CHECK_ERROR("uv_exepath", status);
	std::string home(buffer, length);

	// use share directory
	auto exec = std::filesystem::path(home);
	auto root = std::filesystem::canonical(exec).parent_path().parent_path();
	auto lib = root / "share" / "ot";
}


//
//	OtTheme::blend
//

ImU32 OtTheme::blend(ImU32 color1, ImU32 color2, double ratio) {
	auto c1 = ImGui::ColorConvertU32ToFloat4(color1);
	auto c2 = ImGui::ColorConvertU32ToFloat4(color2);
	return ImGui::ColorConvertFloat4ToU32(ImVec4(
		c1.x + (c2.x - c1.x) * ratio,
		c1.y + (c2.y - c1.y) * ratio,
		c1.z + (c2.z - c1.z) * ratio,
		c1.w + (c2.w - c1.w) * ratio));
}
