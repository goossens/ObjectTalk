//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "ImGuiCurve.h"
#include "ImGuiRangeSlider.h"

#include "OtUi.h"

#include "OtParticleSettings.h"


//
//	OtParticleSettings::renderUI
//

bool OtParticleSettings::renderUI() {
	bool changed = false;
	changed |= OtUiDragInt("Particle Count", &particles, 1, 10000);
	changed |= ImGui::RangeSliderFloat("Lifespan Range", &lifeSpanLow, &lifeSpanHigh, 0.1f, 5.0f);

	changed |= atlas.renderUI("Particle Atlas");
	changed |= OtUiSelectorPowerOfTwo("Atlas Rows", &atlasRows, 1, 32);
	changed |= OtUiSelectorPowerOfTwo("Atlas Columns", &atlasColumns, 1, 32);

	changed |= OtUiSelectorEnum("Emitter Shape", &shape, OtParticleSettings::shapeTypes, OtParticleSettings::shapeTypesCount);
	changed |= OtUiDragFloat("Emitter Speed", &speed, 0.0f, 5.0f);
	changed |= OtUiDragFloat("Gravity", &gravity, -2.0f, 2.0f);
	changed |= ImGui::DragFloat2("Rotation", rotation, 0.0f, 360.0f);

	ImVec2 size{ImGui::GetContentRegionAvail().x, 80.0f};
	changed |= ImGui::Curve("Scale", size, (int) scale.size(), scale.data(), &scaleSelection);
	changed |= ImGui::Curve("Alpha", size, (int) alpha.size(), alpha.data(), &alphaSelection);

	return changed;
}


//
//	OtParticleSettings::serialize
//

nlohmann::json OtParticleSettings::serialize(std::string *basedir) {
	auto data = nlohmann::json::object();
	data["particles"] = particles;
	data["lifeSpanLow"] = lifeSpanLow;
	data["lifeSpanHigh"] = lifeSpanHigh;
	data["atlas"] = OtPathRelative(atlas.getPath(), basedir);
	data["atlasRows"] = atlasRows;
	data["atlasColumns"] = atlasColumns;
	data["shape"] = shape;
	data["speed"] = speed;
	data["gravity"] = gravity;
	data["rotationStart"] = rotation[0];
	data["rotationEnd"] = rotation[1];
	data["scale"] = scale;
	data["alpha"] = alpha;
	return data;
}


//
//	OtParticleSettings::deserialize
//

void OtParticleSettings::deserialize(nlohmann::json data, std::string *basedir) {
	particles = data.value("particles", 1024);
	lifeSpanLow = data.value("lifeSpanLow", 2.0f);
	lifeSpanHigh = data.value("lifeSpanHigh", 4.0f);
	atlas = OtPathGetAbsolute(data, "atlas", basedir);
	atlasRows = data.value("atlasRows", 1);
	atlasColumns = data.value("atlasColumns", 1);
	shape = data.value("shape", OtParticleSettings::pointShape);
	speed = data.value("speed", 0.3f);
	gravity = data.value("gravity", 0.3f);
	rotation[0] = data.value("rotationStart", 0.0f);
	rotation[1] = data.value("rotationEnd", 0.0f);
	scale = data.value("scale", std::array<ImVec2, 6>{ImVec2(ImGui::CurveTerminator, 0.0f)});
	alpha = data.value("alpha", std::array<ImVec2, 6>{ImVec2(ImGui::CurveTerminator, 0.0f)});
}
