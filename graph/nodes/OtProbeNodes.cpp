//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"

#include "OtTexture.h"

#include "OtGraphNode.h"
#include "OtProbeNodes.h"


//
//	OtGraphNodeFloatProbe
//

class OtGraphNodeFloatProbe : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeFloatProbe() : OtGraphNodeClass(name, OtGraphNodeClass::probe) {}

	// configure node
	inline void configure() override {
		addInputPin("Value", value)->addRenderer([&] () {
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::InputFloat("##value", &value, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}, fieldWidth);
	}

	static constexpr const char* name = "Float Probe";
	static constexpr float fieldWidth = 120.0f;

protected:
	float value = 0.0f;
};


//
//	OtGraphNodeImageProbe
//

class OtGraphNodeImageProbe : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeImageProbe() : OtGraphNodeClass(name, OtGraphNodeClass::probe) {}

	// configure node
	inline void configure() override {
		auto pin = addInputPin("Image", texture);
	}

	// nothing to do on execute but we'll use it to determine image size
	void onExecute() override {
		if (texture.isValid()) {
			customW = std::min(fieldWidth, (float) texture.getWidth());
			customH = customW * texture.getHeight() / texture.getWidth();

		} else {
			customW = fieldWidth;
			customH = ImGui::GetFrameHeightWithSpacing();
		}

		needsSizing = true;
	}

	// rendering custom fields
	void customRendering() override {
		if (texture.isValid()) {
			ImGui::Image((void*)(intptr_t) texture.getTextureIndex(), ImVec2(customW, customH));

		} else {
			ImGui::TextUnformatted("No Image available");
		}
	}

	float getCustomRenderingWidth() override {
		return customW;
	}

	float getCustomRenderingHeight() override {
		return customH;
	}

	static constexpr const char* name = "Image Probe";
	static constexpr float fieldWidth = 170.0f;

protected:
	OtTexture texture;
	float customW;
	float customH;
};


//
//	OtProbeNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Probes", CLASS::name)

void OtProbeNodesRegister(OtGraph& graph) {
	REGISTER(OtGraphNodeFloatProbe);
	REGISTER(OtGraphNodeImageProbe);
}
