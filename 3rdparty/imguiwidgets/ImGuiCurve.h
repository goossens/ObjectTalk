#pragma once

#include "imgui.h"

namespace ImGui {
	bool Curve(const char* label, const ImVec2& size, const int maxpoints, ImVec2* points, int* selection);
	float CurveValueSmooth(float p, int maxpoints, const ImVec2* points);
	static constexpr float CurveTerminator = -10000.0f;
};
