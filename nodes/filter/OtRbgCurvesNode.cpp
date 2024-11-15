//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <array>

#include "imgui.h"
#include "ImGuiCurve.h"
#include "nlohmann/json.hpp"

#include "OtColor.h"
#include "OtRgbCurve.h"
#include "OtUi.h"

#include "OtNodesFactory.h"
#include "OtTextureFilterNode.h"


//
//	OtRbgCurvesNode
//

class OtRbgCurvesNode : public OtTextureFilterNode {
public:
	// configure node
	inline void configure() override {
		OtTextureFilterNode::configure();
		addInputPin("Black Level", blackLevel);
		addInputPin("White Level", whiteLevel);
	}

	// render custom fields
	void customRendering(float width) override {
		bool changed = false;
		auto old = serialize().dump();

		ImGui::PushID("Mode");
		changed |= OtUi::radioButton("RGB", &curve, OtRgbCurve::rgbCurve);
		ImGui::SameLine(0.0f, 5.0f);
		changed |= OtUi::radioButton("R", &curve, OtRgbCurve::redCurve);
		ImGui::SameLine(0.0f, 0.0f);
		changed |= OtUi::radioButton("G", &curve, OtRgbCurve::greenCurve);
		ImGui::SameLine(0.0f, 0.0f);
		changed |= OtUi::radioButton("B", &curve, OtRgbCurve::blueCurve);
		ImGui::SameLine(0.0f, 0.0f);
		changed |= OtUi::radioButton("A", &curve, OtRgbCurve::alphaCurve);
		ImGui::PopID();

		ImVec2 size{width, getCustomRenderingWidth()};
		const char* title;

		switch (curve) {
			case OtRgbCurve::rgbCurve: title = "RGB"; break;
			case OtRgbCurve::redCurve: title = "Red"; break;
			case OtRgbCurve::greenCurve: title = "Green"; break;
			case OtRgbCurve::blueCurve: title = "Blue"; break;
			case OtRgbCurve::alphaCurve: title = "Alpha"; break;
		}

		changed |= ImGui::Curve(title, size, curvePoints, lut.data(), &lutSelection);

		if (changed) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	float getCustomRenderingWidth() override {
		return 200.0f;
	}

	float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + 200.0f + ImGui::GetStyle().ItemSpacing.y;
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {

		(*data)["curve"] = curve;
		(*data)["lut"] = lut;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		curve = data->value("curve", OtRgbCurve::rgbCurve);
		lut = data->value("lut", std::array<ImVec2, curvePoints>{ImVec2(ImGui::CurveTerminator, 0.0f)});
	}

	// run filter
	void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		// update lookup table
		u_int8_t lutValues[256];

		for (auto i = 0; i < 256; i++) {
			lutValues[i] = ImGui::CurveValueSmooth(float(i) / 255.0f, curvePoints, lut.data()) * 255;
		}

		if (!lutCurve.isValid()) {
			lutCurve.create(256, 1, OtTexture::r8Texture);
		}

		lutCurve.update(0, 0, 256, 1, lutValues);

		rgbCurve.setCurve(curve);
		rgbCurve.setLUT(lutCurve);
		rgbCurve.setBlackLevel(blackLevel);
		rgbCurve.setWhiteLevel(whiteLevel);
		rgbCurve.render(input, output);
	}

	static constexpr const char* nodeName = "RGB Curves";
	static constexpr int nodeCategory = OtNodeClass::filter;
	static constexpr int nodeKind = OtNodeClass::fixed;

	// properties
	int curve = OtRgbCurve::rgbCurve;

	static constexpr int curvePoints = 6;
	std::array<ImVec2, curvePoints> lut{ImVec2(ImGui::CurveTerminator, 0.0f)};

	OtColor blackLevel{0.0f, 0.0f, 0.0f, 1.0f};
	OtColor whiteLevel{1.0f, 1.0f, 1.0f, 1.0f};

	OtTexture lutCurve;
	int lutSelection = -1;

	OtRgbCurve rgbCurve;
};

static OtNodesFactoryRegister<OtRbgCurvesNode> type;
