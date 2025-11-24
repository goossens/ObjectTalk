//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>

#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"
#include "OtUi.h"

#include "OtGrass.h"
#include "OtVertex.h"


//
//	OtGrass::renderUI
//

bool OtGrass::renderUI() {
	bool changed = false;
	changed |= OtUi::dragFloat("Patch Width", &patchWidth, 1.0f, 100.0f);
	changed |= OtUi::dragFloat("Patch Depth", &patchDepth, 1.0f, 100.0f);
	changed |= OtUi::dragInt("Blades", &blades, 1, 1000000);

	changed |= OtUi::dragInt("Blade Segments", &bladeSegments, 1, 10);
	changed |= OtUi::dragFloat("Blade Width", &bladeWidth, 0.01f, 1.0f);
	changed |= OtUi::dragFloat("Blade Height", &bladeHeight, 0.01f, 5.0f);
	changed |= OtUi::dragFloat("Blade Pointiness", &bladePointiness, 0.0f, 1.0f);
	changed |= OtUi::dragFloat("Blade Curve", &bladeCurve, 0.0f, 2.0f);

	changed |= OtUi::dragFloat("Wind Direction", &windDirection, 0.0f, 360.0f);
	changed |= OtUi::dragFloat("Wind Strength", &windStrength, 0.0f, 1.0f);

	changed |= ImGui::ColorEdit3("Base Color", glm::value_ptr(baseColor));
	changed |= ImGui::ColorEdit3("Tip Color", glm::value_ptr(tipColor));

	changed |= OtUi::dragFloat("Width Variation", &widthVariation, 0.0f, 1.0f);
	changed |= OtUi::dragFloat("Height Variation", &heightVariation, 0.0f, 1.0f);
	changed |= OtUi::dragFloat("Wind Variation", &windVariation, 0.0f, 1.0f);
	changed |= OtUi::dragFloat("Color Variation", &colorVariation, 0.0f, 1.0f);

	changed |= OtUi::toggleButton("Cast Shadow", &castShadow);
	return changed;
}


//
//	OtGrass::serialize
//

nlohmann::json OtGrass::serialize([[maybe_unused]] std::string* basedir) {
	auto data = nlohmann::json::object();
	data["patchWidth"] = patchWidth;
	data["patchDepth"] = patchDepth;
	data["blades"] = blades;

	data["bladeSegments"] = bladeSegments;
	data["bladeWidth"] = bladeWidth;
	data["bladeHeight"] = bladeHeight;
	data["bladePointiness"] = bladePointiness;
	data["bladeCurve"] = bladeCurve;

	data["windDirection"] = windDirection;
	data["windStrength"] = windStrength;

	data["baseColor"] = baseColor;
	data["tipColor"] = tipColor;

	data["widthVariation"] = widthVariation;
	data["heightVariation"] = heightVariation;
	data["windVariation"] = windVariation;
	data["colorVariation"] = colorVariation;

	data["castShadow"] = castShadow;
	return data;
}


//
//	OtGrass::deserialize
//

void OtGrass::deserialize(nlohmann::json data, [[maybe_unused]] std::string* basedir) {
	patchWidth = data.value("patchWidth", 1.0f);
	patchDepth = data.value("patchDepth", 1.0f);
	blades = data.value("blades", 10);

	bladeSegments = data.value("bladeSegments", 4);
	bladeWidth = data.value("bladeWidth", 0.1f);
	bladeHeight = data.value("bladeHeight", 1.0f);
	bladePointiness = data.value("bladePointiness", 0.5f);
	bladeCurve = data.value("bladeCurve", 0.5f);

	windDirection = data.value("windDirection", 0.0f);
	windStrength = data.value("windStrength", 0.2f);

	baseColor = data.value("baseColor", glm::vec3(0.1f, 0.4f, 0.04f));
	tipColor = data.value("tipColor", glm::vec3(0.4f, 0.6f, 0.2f));

	widthVariation = data.value("widthVariation", 0.2f);
	heightVariation = data.value("heightVariation", 0.2f);
	windVariation = data.value("windVariation", 0.2f);
	colorVariation = data.value("colorVariation", 0.2f);

	castShadow = data.value("castShadow", true);
}


//
//	OtGrass::getVertexBuffer
//

OtVertexBuffer& OtGrass::getVertexBuffer() {
	// TODO: insert return statement here

	// update buffers (if required)
	if (bufferBladeSegments != bladeSegments) {
		updateBuffers();
		bufferBladeSegments = bladeSegments;
	}

	// return buffer
	return vertexBuffer;
}


//
//	OtGrass::getIndexBuffer
//

OtIndexBuffer& OtGrass::getIndexBuffer() {
	// update buffers (if required)
	if (bufferBladeSegments != bladeSegments) {
		updateBuffers();
		bufferBladeSegments = bladeSegments;
	}

	// return buffer
	return indexBuffer;
}


//
//	OtGrass::updateBuffers
//

void OtGrass::updateBuffers() {
	// update vertex buffer
	std::vector<glm::vec3> vertices;
	auto verticesPerSide = (bladeSegments + 1) * 2;
	auto vertexCount = verticesPerSide * 2;

	for (auto i = 0; i < vertexCount; i++) {
		vertices.emplace_back(
			float(i & 1) - 0.5f,
			float((i % verticesPerSide) / 2) / bladeSegments,
			0.0f);
	}

	vertexBuffer.set(vertices.data(), vertices.size(), OtVertexPos::getDescription());

	// update index buffer
	std::vector<uint32_t> indices;
	indices.resize(bladeSegments * 12);

	for (auto i = 0; i < bladeSegments; i++) {
		auto vi = i * 2;
		indices[i * 12 +  0] = vi + 0;
		indices[i * 12 +  1] = vi + 1;
		indices[i * 12 +  2] = vi + 2;

		indices[i * 12 +  3] = vi + 2;
		indices[i * 12 +  4] = vi + 1;
		indices[i * 12 +  5] = vi + 3;

		vi += (bladeSegments + 1) * 2;
		indices[i * 12 +  6] = vi + 2;
		indices[i * 12 +  7] = vi + 1;
		indices[i * 12 +  8] = vi + 0;

		indices[i * 12 +  9] = vi + 3;
		indices[i * 12 + 10] = vi + 1;
		indices[i * 12 + 11] = vi + 2;
	}

	indexBuffer.set(indices.data(), indices.size(), false);
}
