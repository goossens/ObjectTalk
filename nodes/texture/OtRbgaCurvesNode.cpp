//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdint>
#include <array>

#include "imgui.h"
#include "ImGuiCurve.h"
#include "nlohmann/json.hpp"

#include "OtColor.h"
#include "OtRgbaCurve.h"
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
	inline void customRendering(float itemWidth) override {
		bool changed = false;
		auto old = serialize().dump();

		ImGui::PushID("Mode");
		changed |= OtUi::radioButton("RGB", &curve, OtRgbaCurve::Curve::rgb);
		ImGui::SameLine(0.0f, 5.0f);
		changed |= OtUi::radioButton("R", &curve, OtRgbaCurve::Curve::red);
		ImGui::SameLine(0.0f, 0.0f);
		changed |= OtUi::radioButton("G", &curve, OtRgbaCurve::Curve::green);
		ImGui::SameLine(0.0f, 0.0f);
		changed |= OtUi::radioButton("B", &curve, OtRgbaCurve::Curve::blue);
		ImGui::SameLine(0.0f, 0.0f);
		changed |= OtUi::radioButton("A", &curve, OtRgbaCurve::Curve::alpha);
		ImGui::PopID();

		ImVec2 size{itemWidth, getCustomRenderingWidth()};
		const char* curveSelector = "";

		switch (curve) {
			case OtRgbaCurve::Curve::rgb: curveSelector = "RGB"; break;
			case OtRgbaCurve::Curve::red: curveSelector = "Red"; break;
			case OtRgbaCurve::Curve::green: curveSelector = "Green"; break;
			case OtRgbaCurve::Curve::blue: curveSelector = "Blue"; break;
			case OtRgbaCurve::Curve::alpha: curveSelector = "Alpha"; break;
		}

		changed |= ImGui::Curve(curveSelector, size, curvePoints, lut.data(), &lutSelection);

		if (changed) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}
	}

	inline float getCustomRenderingWidth() override {
		return 200.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing() + 200.0f + ImGui::GetStyle().ItemSpacing.y;
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, std::string* /* basedir */) override {

		(*data)["curve"] = curve;
		(*data)["lut"] = lut;
	}

	inline void customDeserialize(nlohmann::json* data, std::string* /* basedir */) override {
		curve = data->value("curve", OtRgbaCurve::Curve::rgb);
		lut = data->value("lut", std::array<ImVec2, curvePoints>{ImVec2(ImGui::CurveTerminator, 0.0f)});
	}

	// run filter
	inline void onFilter(OtTexture& input, OtFrameBuffer& output) override {
		// update lookup table
		uint8_t lutValues[256];

		for (auto i = 0; i < 256; i++) {
			lutValues[i] = static_cast<uint8_t>(ImGui::CurveValueSmooth(float(i) / 255.0f, curvePoints, lut.data()) * 255.0f);
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

	static constexpr const char* nodeName = "RGBA Curves";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::texture;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

	// properties
	OtRgbaCurve::Curve curve = OtRgbaCurve::Curve::rgb;

	static constexpr int curvePoints = 6;
	std::array<ImVec2, curvePoints> lut{ImVec2(ImGui::CurveTerminator, 0.0f)};

	OtColor blackLevel{0.0f, 0.0f, 0.0f, 1.0f};
	OtColor whiteLevel{1.0f, 1.0f, 1.0f, 1.0f};

	OtTexture lutCurve;
	int lutSelection = -1;

	OtRgbaCurve rgbCurve;
};

static OtNodesFactoryRegister<OtRbgCurvesNode> registration;
