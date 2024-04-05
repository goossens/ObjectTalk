// [src] https://github.com/ocornut/imgui/issues/123
// [src] https://github.com/ocornut/imgui/issues/55

#include <cmath>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

namespace tween {
	enum TYPE
	{
		LINEAR,

		QUADIN,		 // t^2
		QUADOUT,
		QUADINOUT,
		CUBICIN,		 // t^3
		CUBICOUT,
		CUBICINOUT,
		QUARTIN,		 // t^4
		QUARTOUT,
		QUARTINOUT,
		QUINTIN,		 // t^5
		QUINTOUT,
		QUINTINOUT,
		SINEIN,		 // sin(t)
		SINEOUT,
		SINEINOUT,
		EXPOIN,		 // 2^t
		EXPOOUT,
		EXPOINOUT,
		CIRCIN,		 // sqrt(1-t^2)
		CIRCOUT,
		CIRCINOUT,
		ELASTICIN,	  // exponentially decaying sine wave
		ELASTICOUT,
		ELASTICINOUT,
		BACKIN,		 // overshooting cubic easing: (s+1)*t^3 - s*t^2
		BACKOUT,
		BACKINOUT,
		BOUNCEIN,		// exponentially decaying parabolic bounce
		BOUNCEOUT,
		BOUNCEINOUT,

		SINESQUARE,	 // gapjumper's
		EXPONENTIAL,	 // gapjumper's
		SCHUBRING1,	 // terry schubring's formula 1
		SCHUBRING2,	 // terry schubring's formula 2
		SCHUBRING3,	 // terry schubring's formula 3

		SINPI2,		 // tomas cepeda's
		SWING,		  // tomas cepeda's & lquery's
	};

	// }

	// implementation

	static inline double ease(int easetype, double t) {
		const double d = 1.f;
		const double pi = 3.1415926535897932384626433832795;
		const double pi2 = 3.1415926535897932384626433832795 / 2;

		double p = t/d;

		switch(easetype) {
			// Modeled after the line y = x
			default:
			case TYPE::LINEAR: {
				return p;
			}

			// Modeled after the parabola y = x^2
			case TYPE::QUADIN: {
				return p * p;
			}

			// Modeled after the parabola y = -x^2 + 2x
			case TYPE::QUADOUT: {
				return -(p * (p - 2));
			}

			// Modeled after the piecewise quadratic
			// y = (1/2)((2x)^2)			 ; [0, 0.5)
			// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
			case TYPE::QUADINOUT: {
				if (p < 0.5) {
					return 2 * p * p;
				}
				else {
					return (-2 * p * p) + (4 * p) - 1;
				}
			}

			// Modeled after the cubic y = x^3
			case TYPE::CUBICIN: {
				return p * p * p;
			}

			// Modeled after the cubic y = (x - 1)^3 + 1
			case TYPE::CUBICOUT: {
				double f = (p - 1);
				return f * f * f + 1;
			}

			// Modeled after the piecewise cubic
			// y = (1/2)((2x)^3)	  ; [0, 0.5)
			// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
			case TYPE::CUBICINOUT: {
				if (p < 0.5) {
					return 4 * p * p * p;
				}
				else {
					double f = ((2 * p) - 2);
					return 0.5 * f * f * f + 1;
				}
			}

			// Modeled after the quartic x^4
			case TYPE::QUARTIN: {
				return p * p * p * p;
			}

			// Modeled after the quartic y = 1 - (x - 1)^4
			case TYPE::QUARTOUT: {
				double f = (p - 1);
				return f * f * f * (1 - p) + 1;
			}

			// Modeled after the piecewise quartic
			// y = (1/2)((2x)^4)		; [0, 0.5)
			// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
			case TYPE::QUARTINOUT: {
				if (p < 0.5) {
					return 8 * p * p * p * p;
				}
				else {
					double f = (p - 1);
					return -8 * f * f * f * f + 1;
				}
			}

			// Modeled after the quintic y = x^5
			case TYPE::QUINTIN: {
				return p * p * p * p * p;
			}

			// Modeled after the quintic y = (x - 1)^5 + 1
			case TYPE::QUINTOUT: {
				double f = (p - 1);
				return f * f * f * f * f + 1;
			}

			// Modeled after the piecewise quintic
			// y = (1/2)((2x)^5)	  ; [0, 0.5)
			// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
			case TYPE::QUINTINOUT: {
				if (p < 0.5) {
					return 16 * p * p * p * p * p;
				}
				else {
					double f = ((2 * p) - 2);
					return 0.5 * f * f * f * f * f + 1;
				}
			}

			// Modeled after quarter-cycle of sine wave
			case TYPE::SINEIN: {
				return std::sin((p - 1) * pi2) + 1;
			}

			// Modeled after quarter-cycle of sine wave (different phase)
			case TYPE::SINEOUT: {
				return std::sin(p * pi2);
			}

			// Modeled after half sine wave
			case TYPE::SINEINOUT: {
				return 0.5 * (1 - std::cos(p * pi));
			}

			// Modeled after shifted quadrant IV of unit circle
			case TYPE::CIRCIN: {
				return 1 - std::sqrt(1 - (p * p));
			}

			// Modeled after shifted quadrant II of unit circle
			case TYPE::CIRCOUT: {
				return std::sqrt((2 - p) * p);
			}

			// Modeled after the piecewise circular function
			// y = (1/2)(1 - sqrt(1 - 4x^2))		  ; [0, 0.5)
			// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
			case TYPE::CIRCINOUT: {
				if (p < 0.5) {
					return 0.5 * (1 - std::sqrt(1 - 4 * (p * p)));
				}
				else {
					return 0.5 * (std::sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
				}
			}

			// Modeled after the exponential function y = 2^(10(x - 1))
			case TYPE::EXPOIN: {
				return (p == 0.0) ? p : std::pow(2, 10 * (p - 1));
			}

			// Modeled after the exponential function y = -2^(-10x) + 1
			case TYPE::EXPOOUT: {
				return (p == 1.0) ? p : 1 - std::pow(2, -10 * p);
			}

			// Modeled after the piecewise exponential
			// y = (1/2)2^(10(2x - 1))		 ; [0,0.5)
			// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
			case TYPE::EXPOINOUT: {
				if (p == 0.0 || p == 1.0) return p;

				if (p < 0.5) {
					return 0.5 * pow(2, (20 * p) - 10);
				}
				else {
					return -0.5 * pow(2, (-20 * p) + 10) + 1;
				}
			}

			// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
			case TYPE::ELASTICIN: {
				return std::sin(13 * pi2 * p) * pow(2, 10 * (p - 1));
			}

			// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
			case TYPE::ELASTICOUT: {
				return std::sin(-13 * pi2 * (p + 1)) * pow(2, -10 * p) + 1;
			}

			// Modeled after the piecewise exponentially-damped sine wave:
			// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))	 ; [0,0.5)
			// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
			case TYPE::ELASTICINOUT: {
				if (p < 0.5) {
					return 0.5 * std::sin(13 * pi2 * (2 * p)) * std::pow(2, 10 * ((2 * p) - 1));
				}
				else {
					return 0.5 * (std::sin(-13 * pi2 * ((2 * p - 1) + 1)) * std::pow(2, -10 * (2 * p - 1)) + 2);
				}
			}

			// Modeled (originally) after the overshooting cubic y = x^3-x*sin(x*pi)
			case TYPE::BACKIN: { /*
				return p * p * p - p * sin(p * pi); */
				double s = 1.70158f;
				return p * p * ((s + 1) * p - s);
			}

			// Modeled (originally) after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
			case TYPE::BACKOUT: { /*
				double f = (1 - p);
				return 1 - (f * f * f - f * sin(f * pi)); */
				double s = 1.70158f;
				return --p, 1.f * (p*p*((s+1)*p + s) + 1);
			}

			// Modeled (originally) after the piecewise overshooting cubic function:
			// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))		  ; [0, 0.5)
			// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
			case TYPE::BACKINOUT: { /*
				if (p < 0.5) {
					double f = 2 * p;
					return 0.5 * (f * f * f - f * sin(f * pi));
				}
				else {
					double f = (1 - (2*p - 1));
					return 0.5 * (1 - (f * f * f - f * sin(f * pi))) + 0.5;
				} */
				double s = 1.70158f * 1.525f;
				if (p < 0.5) {
					return p *= 2, 0.5 * p * p * (p*s+p-s);
				}
				else {
					return p = p * 2 - 2, 0.5 * (2 + p*p*(p*s+p+s));
				}
			}

#		  define tween$bounceout(p) (\
				(p) < 4/11.0 ? (121 * (p) * (p))/16.0 : \
				(p) < 8/11.0 ? (363/40.0 * (p) * (p)) - (99/10.0 * (p)) + 17/5.0 : \
				(p) < 9/10.0 ? (4356/361.0 * (p) * (p)) - (35442/1805.0 * (p)) + 16061/1805.0 \
						  : (54/5.0 * (p) * (p)) - (513/25.0 * (p)) + 268/25.0)

			case TYPE::BOUNCEIN: {
				return 1 - tween$bounceout(1 - p);
			}

			case TYPE::BOUNCEOUT: {
				return tween$bounceout(p);
			}

			case TYPE::BOUNCEINOUT: {
				if (p < 0.5) {
					return 0.5 * (1 - tween$bounceout(1 - p * 2));

				} else {
					return 0.5 * tween$bounceout((p * 2 - 1)) + 0.5;
				}
			}

#		  undef tween$bounceout

			case TYPE::SINESQUARE: {
				double A = std::sin((p)*pi2);
				return A*A;
			}

			case TYPE::EXPONENTIAL: {
				return 1/(1 + std::exp(6-12*(p)));
			}

			case TYPE::SCHUBRING1: {
				return 2*(p+(0.5f-p)* std::abs(0.5f-p))-0.5f;
			}

			case TYPE::SCHUBRING2: {
				double p1pass= 2*(p+(0.5f-p)* std::abs(0.5f-p))-0.5f;
				double p2pass= 2*(p1pass+(0.5f-p1pass)* std::abs(0.5f-p1pass))-0.5f;
				double pAvg=(p1pass+p2pass)/2;
				return pAvg;
			}

			case TYPE::SCHUBRING3: {
				double p1pass= 2*(p+(0.5f-p)* std::abs(0.5f-p))-0.5f;
				double p2pass= 2*(p1pass+(0.5f-p1pass)* std::abs(0.5f-p1pass))-0.5f;
				return p2pass;
			}

			case TYPE::SWING: {
				return ((-std::cos(pi * p) * 0.5) + 0.5);
			}

			case TYPE::SINPI2: {
				return std::sin(p * pi2);
			}
		}
	}
}

namespace ImGui
{
static const float CurveTerminator = -10000;

// [src] http://iquilezles.org/www/articles/minispline/minispline.htm
// key format (for dim == 1) is (t0,x0,t1,x1 ...)
// key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
// key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)
template<int DIM>
void spline(const float* key, int num, float t, float* v)
{
	static float coefs[16] = {
		-1.0f, 2.0f,-1.0f, 0.0f,
		 3.0f,-5.0f, 0.0f, 2.0f,
		-3.0f, 4.0f, 1.0f, 0.0f,
		 1.0f,-1.0f, 0.0f, 0.0f
	};

	const int size = DIM + 1;

	// find key
	int k = 0;
	while (key[k * size] < t)
		k++;

	const float key0 = key[(k - 1) * size];
	const float key1 = key[k * size];

	// interpolant
	const float h = (t - key0) / (key1 - key0);

	// init result
	for (int i = 0; i < DIM; i++)
		v[i] = 0.0f;

	// add basis functions
	for (int i = 0; i < 4; ++i)
	{
		const float* co = &coefs[4 * i];
		const float b = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);

		const int kn = ImClamp(k + i - 2, 0, num - 1);
		for (int j = 0; j < DIM; j++)
			v[j] += b * key[kn * size + j + 1];
	}
}

float CurveValueSmooth(float p, int maxpoints, const ImVec2* points)
{
	if (maxpoints < 2 || points == 0)
		return 0;
	if (p < 0)
		return points[0].y;

	float* input = new float[maxpoints * 2];
	float output[4];

	for (int i = 0; i < maxpoints; ++i) {
		input[i * 2 + 0] = points[i].x;
		input[i * 2 + 1] = points[i].y;
	}

	spline<1>(input, maxpoints, p, output);

	delete[] input;
	return output[0];
}

float CurveValue(float p, int maxpoints, const ImVec2* points)
{
	if (maxpoints < 2 || points == 0)
		return 0;
	if (p < 0)
		return points[0].y;

	int left = 0;
	while (left < maxpoints && points[left].x < p && points[left].x != -1)
		left++;
	if (left)
		left--;

	if (left == maxpoints - 1)
		return points[maxpoints - 1].y;

	float d = (p - points[left].x) / (points[left + 1].x - points[left].x);

	return points[left].y + (points[left + 1].y - points[left].y) * d;
}

static inline float ImRemap(float v, float a, float b, float c, float d) {
	return (c + (d - c) * (v - a) / (b - a));
}

static inline ImVec2 ImRemap(const ImVec2& v, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d) {
	return ImVec2(ImRemap(v.x, a.x, b.x, c.x, d.x), ImRemap(v.y, a.y, b.y, c.y, d.y));
}

bool Curve(const char* label, const ImVec2& size, const int maxpoints, ImVec2* points, int* selection, const ImVec2& rangeMin, const ImVec2& rangeMax) {
	bool changed = false;
	int i;

	if (points[0].x <= CurveTerminator) {
		points[0] = rangeMin;
		points[1] = rangeMax;
		points[2].x = CurveTerminator;
	}

	ImGuiWindow* window = GetCurrentWindow();
	ImGuiContext& g = *GImGui;
	const ImGuiID id = window->GetID(label);
	ImGuiStyle& style = ImGui::GetStyle();

	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	ItemSize(bb);
	ItemAdd(bb, id, &bb);
	PushID(label);

	int currentSelection = selection ? *selection : -1;
	const bool hovered = ImGui::ItemHoverable(bb, id, 0);
	int pointCount = 0;

	while (pointCount < maxpoints && points[pointCount].x >= rangeMin.x) {
		pointCount++;
	}

	RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);

	const float ht = bb.Max.y - bb.Min.y;
	const float wd = bb.Max.x - bb.Min.x;
	int hoveredPoint = -1;
	const float pointRadiusInPixels = 5.0f;

	// Handle point selection
	if (hovered) {
		ImVec2 hoverPos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
		hoverPos.y = 1.0f - hoverPos.y;
		ImVec2 pos = ImRemap(hoverPos, ImVec2(0, 0), ImVec2(1, 1), rangeMin, rangeMax);
		int left = 0;

		while (left < pointCount && points[left].x < pos.x) {
			left++;
		}

		if (left) {
			left--;
		}

		const ImVec2 hoverPosScreen = ImRemap(hoverPos, ImVec2(0, 0), ImVec2(1, 1), bb.Min, bb.Max);
		const ImVec2 p1s = ImRemap(points[left], rangeMin, rangeMax, bb.Min, bb.Max);
		const ImVec2 p2s = ImRemap(points[left + 1], rangeMin, rangeMax, bb.Min, bb.Max);
		const float p1d = ImSqrt(ImLengthSqr(p1s - hoverPosScreen));
		const float p2d = ImSqrt(ImLengthSqr(p2s - hoverPosScreen));

		if (p1d < pointRadiusInPixels) {
			hoveredPoint = left;
		}

		if (p2d < pointRadiusInPixels) {
			hoveredPoint = left;
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
			if (g.IO.MouseDoubleClicked[0] || IsMouseDragging(0)) {
				action = action_add_point;
			}

		} else if (g.IO.MouseDoubleClicked[0]) {
			action = action_delete_point;
		}

		if (action == action_add_point) {
			if (pointCount < maxpoints) {
				currentSelection = left + 1;
				++pointCount;

				for (i = pointCount; i > left; --i) {
					points[i] = points[i - 1];
				}

				points[left + 1] = pos;

				if (pointCount < maxpoints) {
					points[pointCount].x = CurveTerminator;
				}
			}

		} else if (action == action_delete_point) {
			// delete point
			if (currentSelection > 0 && currentSelection < maxpoints - 1) {
				for (i = currentSelection; i < maxpoints - 1; ++i) {
					points[i] = points[i + 1];
				}

				--pointCount;
				points[pointCount].x = CurveTerminator;
				currentSelection = -1;
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

		changed = true;

		ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);

		// constrain Y to min/max
		pos.y = 1.0f - pos.y;
		pos = ImRemap(pos, ImVec2(0, 0), ImVec2(1, 1), rangeMin, rangeMax);

		// constrain X to the min left/ max right
		const float pointXRangeMin = (currentSelection > 0) ? points[currentSelection - 1].x : rangeMin.x;
		const float pointXRangeMax = (currentSelection + 1 < pointCount) ? points[currentSelection + 1].x : rangeMax.x;

		pos = ImClamp(pos, ImVec2(pointXRangeMin, rangeMin.y), ImVec2(pointXRangeMax, rangeMax.y));
		points[currentSelection] = pos;

		// snap X first/last to min/max
		if (points[0].x < points[pointCount - 1].x) {
			points[0].x = rangeMin.y;
			points[pointCount - 1].x = rangeMax.x;

		} else {
			points[0].x = rangeMax.x;
			points[pointCount - 1].x = rangeMin.y;
		}
	}

	if (!IsMouseDragging(0) && GetActiveID() == id && selection && *selection != -1 && currentSelection == -1) {
		ClearActiveID();
	}

	const ImU32 gridColor1 = GetColorU32(ImGuiCol_TextDisabled, 0.5f);
	const ImU32 gridColor2 = GetColorU32(ImGuiCol_TextDisabled, 0.25f);

	ImDrawList* drawList = window->DrawList;

	// bg grid
	drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 2), ImVec2(bb.Max.x, bb.Min.y + ht / 2), gridColor1, 3);
	drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 4), ImVec2(bb.Max.x, bb.Min.y + ht / 4), gridColor1);
	drawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + ht / 4 * 3), ImVec2(bb.Max.x, bb.Min.y + ht / 4 * 3), gridColor1);

	for (i = 0; i < 9; i++) {
		drawList->AddLine(ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Min.y), ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Max.y), gridColor2);
	}

	drawList->PushClipRect(bb.Min, bb.Max);

	// smooth curve
	enum
	{
		smoothness = 256
	}; // the higher the smoother

	for (i = 0; i <= (smoothness - 1); ++i) {
		float px = (i + 0) / float(smoothness);
		float qx = (i + 1) / float(smoothness);

		px = ImRemap(px, 0, 1, rangeMin.x, rangeMax.x);
		qx = ImRemap(qx, 0, 1, rangeMin.x, rangeMax.x);

		const float py = CurveValueSmooth(px, maxpoints, points);
		const float qy = CurveValueSmooth(qx, maxpoints, points);

		ImVec2 p = ImRemap(ImVec2(px, py), rangeMin, rangeMax, ImVec2(0,0), ImVec2(1,1));
		ImVec2 q = ImRemap(ImVec2(qx, qy), rangeMin, rangeMax, ImVec2(0,0), ImVec2(1,1));
		p.y = 1.0f - p.y;
		q.y = 1.0f - q.y;

		p = ImRemap(p, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);
		q = ImRemap(q, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);

		drawList->AddLine(p, q, GetColorU32(ImGuiCol_PlotHistogram));
	}

	// lines
	for (i = 1; i < pointCount; i++) {
		ImVec2 a = ImRemap(points[i - 1], rangeMin, rangeMax, ImVec2(0, 0), ImVec2(1, 1));
		ImVec2 b = ImRemap(points[i], rangeMin, rangeMax, ImVec2(0, 0), ImVec2(1, 1));

		a.y = 1.0f - a.y;
		b.y = 1.0f - b.y;

		a = ImRemap(a, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);
		b = ImRemap(b, ImVec2(0,0), ImVec2(1,1), bb.Min, bb.Max);

		drawList->AddLine(a, b, GetColorU32(ImGuiCol_PlotLines, 0.5f));
	}

	if (hovered || draggingPoint) {
		// control points
		for (i = 0; i < pointCount; i++) {
			ImVec2 p = ImRemap(points[i], rangeMin, rangeMax, ImVec2(0, 0), ImVec2(1, 1));
			p.y = 1.0f - p.y;
			p = ImRemap(p, ImVec2(0, 0), ImVec2(1, 1), bb.Min, bb.Max);

			ImVec2 a = p - ImVec2(4, 4);
			ImVec2 b = p + ImVec2(4, 4);
			if(hoveredPoint == i) {
				drawList->AddRect(a, b, GetColorU32(ImGuiCol_PlotLinesHovered));

			} else {
				drawList->AddCircle(p, pointRadiusInPixels, GetColorU32(ImGuiCol_PlotLinesHovered));
			}
		}
	}

	drawList->PopClipRect();

	// draw the text at mouse position
	char buf[128];
	const char* str = label;

	if (hovered || draggingPoint) {
		ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
		pos.y = 1.0f - pos.y;

		pos = ImLerp(rangeMin, rangeMax, pos);

		snprintf(buf, sizeof(buf), "%s (%.2f,%.2f)", label, pos.x, pos.y);
		str = buf;
	}

	RenderTextClipped(ImVec2(bb.Min.x, bb.Min.y + style.FramePadding.y), bb.Max, str, NULL, NULL, ImVec2(0.5f, 0.5f));

	// curve selector
	static const char* items[] = {
		"Custom",

		"Linear",		  "Quad in",	 "Quad out",   "Quad in  out",  "Cubic in",   "Cubic out",
		"Cubic in  out",   "Quart in",	"Quart out",  "Quart in  out", "Quint in",   "Quint out",
		"Quint in  out",   "Sine in",	 "Sine out",   "Sine in  out",  "Expo in",	"Expo out",
		"Expo in  out",	"Circ in",	 "Circ out",   "Circ in  out",  "Elastic in", "Elastic out",
		"Elastic in  out", "Back in",	 "Back out",   "Back in  out",  "Bounce in",  "Bounce out",
		"Bounce in out",

		"Sine square",	 "Exponential",

		"Schubring1",	  "Schubring2",  "Schubring3",

		"SinPi2",		  "Swing"
	};

	// buttons; @todo: mirror, smooth, tessellate
	if (ImGui::BeginPopupContextItem(label)) {
		if (ImGui::Selectable("Reset")) {
			points[0] = rangeMin;
			points[1] = rangeMax;
			points[2].x = CurveTerminator;
		}

		if (ImGui::Selectable("Flip")) {
			for (i = 0; i < pointCount; ++i) {
				const float yVal = 1.0f - ImRemap(points[i].y, rangeMin.y, rangeMax.y, 0, 1);
				points[i].y = ImRemap(yVal, 0, 1, rangeMin.y, rangeMax.y);
			}
		}

		if (ImGui::Selectable("Mirror")) {
			for (int i = 0, j = pointCount - 1; i < j; i++, j--) {
				ImSwap(points[i], points[j]);
			}

			for (i = 0; i < pointCount; ++i) {
				const float xVal = 1.0f - ImRemap(points[i].x, rangeMin.x, rangeMax.x, 0, 1);
				points[i].x = ImRemap(xVal, 0, 1, rangeMin.x, rangeMax.x);
			}
		}

		ImGui::Separator();

		if (ImGui::BeginMenu("Presets")) {
			ImGui::PushID("curve_items");

			for (int row = 0; row < IM_ARRAYSIZE(items); ++row) {
				if (ImGui::MenuItem(items[row])) {
					for (i = 0; i < maxpoints; ++i) {
						const float px = i / float(maxpoints - 1);
						const float py = float(tween::ease(row - 1, px));
						points[i] = ImRemap(ImVec2(px, py), ImVec2(0, 0), ImVec2(1, 1), rangeMin, rangeMax);
					}
				}
			}

			ImGui::PopID();
			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	PopID();

	if (selection) {
		*selection = currentSelection;
	}

	return changed;
}

}; // namespace ImGui
