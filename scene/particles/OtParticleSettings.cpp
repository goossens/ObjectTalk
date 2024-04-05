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

#include "OtUi.h"

#include "OtParticleSettings.h"


//
//	OtParticleSettings::renderUI
//

bool OtParticleSettings::renderUI() {
	bool changed = false;
	changed |= ImGui::DragInt("Particle Count", &particles, 1.0f, 1, 10000);
	changed |= ImGui::SliderFloat2("Life Span", lifeSpan, 0.1f, 5.0f);

	changed |= atlas.renderUI("Particle Atlas");
	changed |= OtUiSelectorPowerOfTwo("Atlas Rows", atlasRows, 1, 32);
	changed |= OtUiSelectorPowerOfTwo("Atlas Columns", atlasColumns, 1, 32);

	changed |= OtUiSelectorEnum("Emitter Shape", shape, OtParticleSettings::shapeTypes, OtParticleSettings::shapeTypesCount);
	changed |= OtUiSelectorEnum("Emitter Direction", direction, OtParticleSettings::directionTypes, OtParticleSettings::directionTypesCount);

	if (direction == OtParticleSettings::upDirection) {
		changed |= ImGui::SliderFloat("Up Variation", &upVariation, 0.1f, 2.0f);
	}

	changed |= ImGui::SliderFloat("Velocity", &velocity, 0.0f, 5.0f);
	changed |= ImGui::SliderFloat("Gravity", &gravity, -2.0f, 2.0f);

	changed |= ImGui::SliderFloat2("Rotation", rotation, 0.0f, 360.0f);
	changed |= ImGui::SliderFloat2("Scale", scale, 0.1f, 10.0f);
	changed |= ImGui::SliderFloat2("Alpha", alpha, 0.0f, 1.0f);
	return changed;
}


//
//	OtParticleSettings::serialize
//

nlohmann::json OtParticleSettings::serialize(std::string *basedir) {
	auto data = nlohmann::json::object();
	data["particles"] = particles;
	data["lifeSpanLow"] = lifeSpan[0];
	data["lifeSpanHigh"] = lifeSpan[1];
	data["atlas"] = OtPathRelative(atlas.getPath(), basedir);
	data["atlasRows"] = atlasRows;
	data["atlasColumns"] = atlasColumns;
	data["shape"] = shape;
	data["direction"] = direction;
	data["upVariation"] = upVariation;
	data["velocity"] = velocity;
	data["gravity"] = gravity;
	data["rotationStart"] = rotation[0];
	data["rotationEnd"] = rotation[1];
	data["scaleStart"] = scale[0];
	data["scaleEnd"] = scale[1];
	data["alphaStart"] = alpha[0];
	data["alphaEnd"] = alpha[1];
	return data;
}


//
//	OtParticleSettings::deserialize
//

void OtParticleSettings::deserialize(nlohmann::json data, std::string *basedir) {
	particles = data.value("particles", 1024);
	lifeSpan[0] = data.value("lifeSpanLow", 2.0f);
	lifeSpan[1] = data.value("lifeSpanHigh", 4.0f);
	atlas = OtPathGetAbsolute(data, "atlas", basedir);
	atlasRows = data.value("atlasRows", 1);
	atlasColumns = data.value("atlasColumns", 1);
	shape = data.value("shape", OtParticleSettings::circleShape);
	direction = data.value("direction", OtParticleSettings::circleShape);
	upVariation = data.value("upVariation", 0.1f);
	velocity = data.value("velocity", 3.0f);
	gravity = data.value("gravity", 0.5f);
	rotation[0] = data.value("rotationStart", 0.0f);
	rotation[1] = data.value("rotationEnd", 0.0f);
	scale[0] = data.value("scaleStart", 1.0f);
	scale[1] = data.value("scaleEnd", 1.0f);
	alpha[0] = data.value("alphaStart", 1.0f);
	alpha[1] = data.value("alphaEnd", 1.0f);
}
