#pragma once

#include "imgui.h"

namespace ImGui {
	bool Curve(const char* label, const ImVec2& size, const int maxpoints, ImVec2* points, int* selection, const ImVec2& rangeMin = ImVec2(0.0f, 0.0f), const ImVec2& rangeMax = ImVec2(1.0f, 1.0f));
	float CurveValue(float p, int maxpoints, const ImVec2* points);
	float CurveValueSmooth(float p, int maxpoints, const ImVec2* points);
	static constexpr float CurveTerminator = -10000.0f;
};
