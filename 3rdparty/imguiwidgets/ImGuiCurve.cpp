// [src] https://github.com/ocornut/imgui/issues/55
// [src] https://github.com/ocornut/imgui/issues/123
// [src] https://github.com/ocornut/imgui/issues/786

// v1.23 - selection index track, context menu, improve manipulation controls (D.Click to add/delete, drag to add)
// v1.22 - flip button; cosmetic fixes
// v1.21 - oops :)
// v1.20 - add iq's interpolation code
// v1.10 - easing and colors
// v1.00 - jari komppa's original

#include <algorithm>
#include <cmath>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_internal.h"

#include "ImGuiCurve.h"


namespace tween {

enum TYPE {
	LINEAR,

	QUADIN, // t^2
	QUADOUT,
	QUADINOUT,
	CUBICIN, // t^3
	CUBICOUT,
	CUBICINOUT,
	QUARTIN, // t^4
	QUARTOUT,
	QUARTINOUT,
	QUINTIN, // t^5
	QUINTOUT,
	QUINTINOUT,
	SINEIN, // sin(t)
	SINEOUT,
	SINEINOUT,
	EXPOIN, // 2^t
	EXPOOUT,
	EXPOINOUT,
	CIRCIN, // sqrt(1-t^2)
	CIRCOUT,
	CIRCINOUT,
	ELASTICIN, // exponentially decaying sine wave
	ELASTICOUT,
	ELASTICINOUT,
	BACKIN, // overshooting cubic easing: (s+1)*t^3 - s*t^2
	BACKOUT,
	BACKINOUT
};

static inline double ease(int easetype, double t) {
	const double pi = 3.1415926535897932384626433832795;
	const double pi2 = 3.1415926535897932384626433832795 / 2;

	switch (easetype) {
		// Modeled after the line y = x
		default:

		case TYPE::LINEAR: {
			return t;
		}

		// Modeled after the parabola y = x^2
		case TYPE::QUADIN: {
			return t * t;
		}

		// Modeled after the parabola y = -x^2 + 2x
		case TYPE::QUADOUT: {
			return -(t * (t - 2));
		}

		// Modeled after the piecewise quadratic
		// y = (1/2)((2x)^2)			 ; [0, 0.5)
		// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
		case TYPE::QUADINOUT: {
			if (t < 0.5)
			{
				return 2 * t * t;
			}
			else
			{
				return (-2 * t * t) + (4 * t) - 1;
			}
		}

		// Modeled after the cubic y = x^3
		case TYPE::CUBICIN: {
			return t * t * t;
		}

		// Modeled after the cubic y = (x - 1)^3 + 1
		case TYPE::CUBICOUT: {
			double f = (t - 1);
			return f * f * f + 1;
		}

		// Modeled after the piecewise cubic
		// y = (1/2)((2x)^3)	   ; [0, 0.5)
		// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
		case TYPE::CUBICINOUT: {
			if (t < 0.5) {
				return 4 * t * t * t;

			} else {
				double f = ((2 * t) - 2);
				return 0.5 * f * f * f + 1;
			}
		}

		// Modeled after the quartic x^4
		case TYPE::QUARTIN: {
			return t * t * t * t;
		}

		// Modeled after the quartic y = 1 - (x - 1)^4
		case TYPE::QUARTOUT: {
			double f = (t - 1);
			return f * f * f * (1 - t) + 1;
		}

		// Modeled after the piecewise quartic
		// y = (1/2)((2x)^4)		; [0, 0.5)
		// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
		case TYPE::QUARTINOUT: {
			if (t < 0.5) {
				return 8 * t * t * t * t;

			} else {
				double f = (t - 1);
				return -8 * f * f * f * f + 1;
			}
		}

		// Modeled after the quintic y = x^5
		case TYPE::QUINTIN: {
			return t * t * t * t * t;
		}

		// Modeled after the quintic y = (x - 1)^5 + 1
		case TYPE::QUINTOUT: {
			double f = (t - 1);
			return f * f * f * f * f + 1;
		}

		// Modeled after the piecewise quintic
		// y = (1/2)((2x)^5)	   ; [0, 0.5)
		// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
		case TYPE::QUINTINOUT: {
			if (t < 0.5) {
				return 16 * t * t * t * t * t;

			} else {
				double f = ((2 * t) - 2);
				return 0.5 * f * f * f * f * f + 1;
			}
		}

		// Modeled after quarter-cycle of sine wave
		case TYPE::SINEIN: {
			return std::sin((t - 1) * pi2) + 1;
		}

		// Modeled after quarter-cycle of sine wave (different phase)
		case TYPE::SINEOUT: {
			return std::sin(t * pi2);
		}

		// Modeled after half sine wave
		case TYPE::SINEINOUT: {
			return 0.5 * (1 - std::cos(t * pi));
		}

		// Modeled after shifted quadrant IV of unit circle
		case TYPE::CIRCIN: {
			return 1 - std::sqrt(1 - (t * t));
		}

		// Modeled after shifted quadrant II of unit circle
		case TYPE::CIRCOUT: {
			return std::sqrt((2 - t) * t);
		}

		// Modeled after the piecewise circular function
		// y = (1/2)(1 - sqrt(1 - 4x^2))		   ; [0, 0.5)
		// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
		case TYPE::CIRCINOUT: {
			if (t < 0.5) {
				return 0.5 * (1 - std::sqrt(1 - 4 * (t * t)));

			} else {
				return 0.5 * (std::sqrt(-((2 * t) - 3) * ((2 * t) - 1)) + 1);
			}
		}

		// Modeled after the exponential function y = 2^(10(x - 1))
		case TYPE::EXPOIN: {
			return (t == 0.0) ? t : pow(2, 10 * (t - 1));
		}

		// Modeled after the exponential function y = -2^(-10x) + 1
		case TYPE::EXPOOUT: {
			return (t == 1.0) ? t : 1 - pow(2, -10 * t);
		}

		// Modeled after the piecewise exponential
		// y = (1/2)2^(10(2x - 1))		 ; [0,0.5)
		// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
		case TYPE::EXPOINOUT: {
			if (t == 0.0 || t == 1.0)
				return t;

			if (t < 0.5) {
				return 0.5 * pow(2, (20 * t) - 10);

			} else {
				return -0.5 * pow(2, (-20 * t) + 10) + 1;
			}
		}

		// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
		case TYPE::ELASTICIN: {
			return std::sin(13 * pi2 * t) * pow(2, 10 * (t - 1));
		}

		// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
		case TYPE::ELASTICOUT: {
			return std::sin(-13 * pi2 * (t + 1)) * pow(2, -10 * t) + 1;
		}

		// Modeled after the piecewise exponentially-damped sine wave:
		// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))	  ; [0,0.5)
		// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
		case TYPE::ELASTICINOUT: {
			if (t < 0.5) {
				return 0.5 * std::sin(13 * pi2 * (2 * t)) * pow(2, 10 * ((2 * t) - 1));

			} else {
				return 0.5 * (std::sin(-13 * pi2 * ((2 * t - 1) + 1)) * pow(2, -10 * (2 * t - 1)) + 2);
			}
		}

		// Modeled (originally) after the overshooting cubic y = x^3-x*sin(x*pi)
		case TYPE::BACKIN: {
			double s = 1.70158f;
			return t * t * ((s + 1) * t - s);
		}

		// Modeled (originally) after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
		case TYPE::BACKOUT: {
			double s = 1.70158f;
			return --t, 1.f * (t * t * ((s + 1) * t + s) + 1);
		}

		// Modeled (originally) after the piecewise overshooting cubic function:
		// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))		   ; [0, 0.5)
		// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
		case TYPE::BACKINOUT: {
			double s = 1.70158f * 1.525f;

			if (t < 0.5) {
				return t *= 2, 0.5 * t * t * (t * s + t - s);

			} else {
				return t = t * 2 - 2, 0.5 * (2 + t * t * (t * s + t + s));
			}
		}
	}
}

}

namespace ImGui {

void spline(const float* key, int num, float t, float* v) {
	// [src] http://iquilezles.org/www/articles/minispline/minispline.htm
	// key format (for dim == 1) is (t0,x0,t1,x1 ...)
	// key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
	// key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)

	static signed char coefs[16] = {
		-1,  2, -1, 0,
		 3, -5,  0, 2,
		-3,  4,  1, 0,
		 1, -1,  0, 0
	};

	// find key
	int k = 0;

	while (key[k * 2] < t) {
		k++;
	}

	// interpolant
	const float h = (t - key[(k - 1) * 2]) / (key[k * 2] - key[(k - 1) * 2]);

	// init result
	v[0] = 0.0f;
	v[1] = 0.0f;

	// add basis functions
	for (int i = 0; i < 4; i++) {
		int kn = k + i - 2;
		kn = std::clamp(kn, 0, num - 1);

		signed char *co = coefs + 4 * i;
		float b  = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);
		v[0] += b * key[kn * 2 + 1];
		v[1] += b * key[kn * 2 + 2];
	}
}

float CurveValueSmooth(float p, int maxpoints, const ImVec2* points) {
	// determine number of actual points
	int actualPoints = 0;

	while (points[actualPoints].x >= 0.0f && actualPoints < maxpoints) {
		actualPoints++;
	}

	// sanity check
    if (actualPoints < 2 || points == 0) {
		return 0.0f;
	}

	// handle min/max cases
    if (p <= 0.0f) {
        return points[0].y;

	} else if (p >= 1.0f) {
        return points[actualPoints - 1].y;
	}

	// if we only have two points, it's a straight line
	if (actualPoints == 2) {
		return points[0].y + p * (points[1].y - points[0].y);

	} else {
		// calculate Catmull-Rom spline
		float output[2];
		spline(reinterpret_cast<const float*>(points), actualPoints, p, output);

		// ensure value is in proper range
		return std::clamp(output[0], 0.0f, 1.0f);
	}
}

static inline float ImRemap(float v, float a, float b, float c, float d) {
	return (c + (d - c) * (v - a) / (b - a));
}

static inline ImVec2 ImRemap(const ImVec2& v, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d) {
	return ImVec2(ImRemap(v.x, a.x, b.x, c.x, d.x), ImRemap(v.y, a.y, b.y, c.y, d.y));
}

bool Curve(const char* label, const ImVec2& size, const int maxpoints, ImVec2* points, int* selection) {
	bool modified = false;
	int i;

	if (maxpoints < 2 || points == nullptr) {
		return false;
	}

	if (points[0].x < 0.0f) {
		points[0] = ImVec2(0.0f, 0.0f);
		points[1] = ImVec2(1.0f, 1.0f);
		points[2].x = CurveTerminator;
	}

	ImGuiWindow* window = GetCurrentWindow();
	ImGuiContext& g = *GImGui;

	const ImGuiID id = window->GetID(label);

	if (window->SkipItems) {
		return false;
	}

	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	ItemSize(bb);

	if (!ItemAdd(bb, id, &bb)) {
		return false;
	}

	PushID(label);

	int currentSelection = selection ? *selection : -1;
	const bool hovered = ImGui::ItemHoverable(bb, id, 0);

	int pointCount = 0;

	while (pointCount < maxpoints && points[pointCount].x >= 0.0f) {
		pointCount++;
	}

	const ImGuiStyle& style = g.Style;
	RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);

	const float ht = bb.Max.y - bb.Min.y;
	const float wd = bb.Max.x - bb.Min.x;

	int hoveredPoint = -1;

	const float pointRadiusInPixels = 5.0f;

	// Handle point selection
	if (hovered) {
		ImVec2 hoverPos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
		hoverPos.y = 1.0f - hoverPos.y;
		ImVec2 pos = hoverPos;

		int left = 0;

		while (left < pointCount && points[left].x < pos.x) {
			left++;
		}

		if (left) {
			left--;
		}

		const ImVec2 hoverPosScreen = ImRemap(hoverPos, ImVec2(0, 0), ImVec2(1, 1), bb.Min, bb.Max);
		const ImVec2 p1s = ImRemap(points[left], ImVec2(0, 0), ImVec2(1, 1), bb.Min, bb.Max);
		const ImVec2 p2s = ImRemap(points[left + 1], ImVec2(0, 0), ImVec2(1, 1), bb.Min, bb.Max);

		const float p1d = ImSqrt(ImLengthSqr(p1s - hoverPosScreen));
		const float p2d = ImSqrt(ImLengthSqr(p2s - hoverPosScreen));

		if (p1d < pointRadiusInPixels) {
			hoveredPoint = left;
		}

		if (p2d < pointRadiusInPixels) {
			hoveredPoint = left + 1;
		}

		if (g.IO.MouseDown[0]) {
			if (currentSelection == -1) {
				currentSelection = hoveredPoint;
			}

		} else {
			currentSelection = -1;
		}

		enum {
			action_none,
			action_add_point,
			action_delete_point
		};

		int action = action_none;

		if (currentSelection == -1) {
			if (g.IO.MouseDoubleClicked[0] || IsMouseDragging(0))
				action = action_add_point;

		} else if(g.IO.MouseDoubleClicked[0]) {
			action = action_delete_point;
		}

		if (action == action_add_point) {
			if (pointCount < maxpoints) {
				for (i = pointCount; i > left; i--) {
					points[i] = points[i - 1];
				}

				points[left + 1] = pos;
				currentSelection = left + 1;
				pointCount++;

				if (pointCount < maxpoints) {
					points[pointCount].x = CurveTerminator;
				}

				modified = true;
			}

		} else if (action == action_delete_point) {
			// delete point
			if (currentSelection > 0 && currentSelection < maxpoints - 1) {
				for (i = currentSelection; i < maxpoints - 1; i++) {
					points[i] = points[i + 1];
				}

				points[--pointCount].x = CurveTerminator;
				currentSelection = -1;
				modified = true;
			}
		}
	}

	// handle point dragging
	const bool draggingPoint = IsMouseDragging(0) && currentSelection != -1;

	if (draggingPoint) {
		if (selection) {
			SetActiveID(id, window);
		}

		SetFocusID(id, window);
		FocusWindow(window);

		modified = true;

		ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);

		// constrain Y to min/max
		pos.y = 1.0f - pos.y;

		// constrain X to the min left/ max right
		const float pointXRangeMin = (currentSelection > 0) ? points[currentSelection - 1].x : 0.0f;
		const float pointXRangeMax = (currentSelection + 1 < pointCount) ? points[currentSelection + 1].x : 1.0f;

		pos = ImClamp(pos, ImVec2(pointXRangeMin, 0.0f), ImVec2(pointXRangeMax, 1.0f));

		points[currentSelection] = pos;

		// snap X first/last to min/max
		if (points[0].x < points[pointCount - 1].x) {
			points[0].x = 0.0f;
			points[pointCount - 1].x = 1.0f;

		} else {
			points[0].x = 1.0f;
			points[pointCount - 1].x = 0.0f;
		}
	}

	if (!IsMouseDragging(0) && GetActiveID() == id && selection && *selection != -1 && currentSelection == -1) {
		ClearActiveID();
	}

	const ImU32 gridColor1 = GetColorU32(ImGuiCol_TextDisabled, 0.5f);
	const ImU32 gridColor2 = GetColorU32(ImGuiCol_TextDisabled, 0.25f);

	ImDrawList* drawList = window->DrawList;

	// bg grid
	drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 4), ImVec2(bb.Max.x, bb.Min.y + ht / 4), gridColor1);
	drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 2), ImVec2(bb.Max.x, bb.Min.y + ht / 2), gridColor1);
	drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 4 * 3), ImVec2(bb.Max.x, bb.Min.y + ht / 4 * 3), gridColor1);

	for (i = 0; i < 9; i++) {
		drawList->AddLine(ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Min.y), ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Max.y), gridColor2);
	}

	// smooth curve (the higher the smoother)
	static constexpr int smoothness = 128;

	for (i = 0; i < smoothness; i++) {
		float px = (i + 0) / float(smoothness);
		float qx = (i + 1) / float(smoothness);

		const float py = CurveValueSmooth(px, maxpoints, points);
		const float qy = CurveValueSmooth(qx, maxpoints, points);

		ImVec2 p = ImVec2(px, py);
		ImVec2 q = ImVec2(qx, qy);
		p.y = 1.0f - p.y;
		q.y = 1.0f - q.y;

		p = ImRemap(p, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);
		q = ImRemap(q, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);

		drawList->AddLine(p, q, GetColorU32(ImGuiCol_PlotHistogram));
	}

	if (hovered || draggingPoint) {
		// control points
		for (i = 0; i < pointCount; i++) {
			ImVec2 p = points[i];
			p.y = 1.0f - p.y;
			p = ImRemap(p, ImVec2(0, 0), ImVec2(1, 1), bb.Min, bb.Max);

			ImVec2 a = p - ImVec2(5, 5);
			ImVec2 b = p + ImVec2(5, 5);

			if(hoveredPoint == i)
				drawList->AddRect(a, b, GetColorU32(ImGuiCol_PlotHistogramHovered));

			else
				drawList->AddCircle(p, pointRadiusInPixels, GetColorU32(ImGuiCol_PlotLinesHovered));
		}
	}

	// draw the text at mouse position
	char buf[128];
	const char* str = label;

	if (hovered || draggingPoint) {
		ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
		pos.y = 1.0f - pos.y;

		pos = ImLerp(ImVec2(0, 0), ImVec2(1, 1), pos);

		snprintf(buf, sizeof(buf), "%s (%.2f,%.2f)", label, pos.x, pos.y);
		str = buf;
	}

	RenderTextClipped(ImVec2(bb.Min.x, bb.Min.y + style.FramePadding.y), bb.Max, str, NULL, NULL, ImVec2(0.5f, 0.5f));

	// curve selector
	static const char* items[] = {
		"Quad in",		"Quad out", 	"Quad in/out",
		"Cubic in",		"Cubic out",	"Cubic in/out",
		"Quart in",		"Quart out",	"Quart in/out",
		"Quint in",		"Quint out",	"Quint in/out",
		"Sine in",		"Sine out", 	"Sine in/out",
		"Expo in",		"Expo out",		"Expo in/out",
		"Circ in",		"Circ out",   	"Circ in/out",
		"Elastic in",	"Elastic out",	"Elastic in/out",
		"Back in",		"Back out",   	"Back in/out"
	};

	// buttons; @todo: mirror, smooth, tessellate
	if (ImGui::BeginPopupContextItem(label)) {
		if (ImGui::Selectable("Reset")) {
			points[0] = ImVec2(0, 0);
			points[1] = ImVec2(1, 1);
			points[2].x = CurveTerminator;
			modified = true;
		}

		if (ImGui::Selectable("Flip")) {
			for (i = 0; i < pointCount; i++) {
				points[i].y = 1.0f - points[i].y;
			}

			modified = true;
		}

		if (ImGui::Selectable("Mirror")) {
			for (int i = 0, j = pointCount - 1; i < j; i++, j--) {
				ImSwap(points[i], points[j]);
			}

			for (i = 0; i < pointCount; i++) {
				points[i].x = 1.0f - points[i].x;
			}

			modified = true;
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Presets")) {
			for (int item = 0; item < IM_ARRAYSIZE(items); item++) {
				if (ImGui::MenuItem(items[item])) {
					for (i = 0; i < maxpoints; i++) {
						const float px = i / float(maxpoints - 1);
						const float py = float(tween::ease(item + 1, px));
						points[i] = ImVec2(px, py);
					}

					modified = true;
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	PopID();

	if (selection) {
		*selection = currentSelection;
	}

	return modified;
}

};
