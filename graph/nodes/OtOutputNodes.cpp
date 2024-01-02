//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtTexture.h"

#include "OtGraphNode.h"
#include "OtOutputNodes.h"


//
//	OtGraphNodeFloatOutput
//

class OtGraphNodeFloatOutput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeFloatOutput() : OtGraphNodeClass(name) {}

	// configure node
	inline void configure() override {
		auto pin = addInputPin("Value", value);

		pin->addRenderer([&] () {
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::InputFloat("##value", &value, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		}, fieldWidth);
	}

	static constexpr const char* name = "Float Output";
	static constexpr float fieldWidth = 120.0f;

protected:
	float value = 0.0f;
};


//
//	OtGraphNodeImageOutput
//

class OtGraphNodeImageOutput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeImageOutput() : OtGraphNodeClass(name) {}

	// configure node
	inline void configure() override {
		auto pin = addInputPin("Image", texture);

		pin->addRenderer([&] () {
			if (ImGui::Button(showImage ? "Hide Image" : "Show Image", ImVec2(fieldWidth, 0.0f))) {
				showImage = !showImage;
			}

			if (showImage) {
				ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos(), ImGuiCond_FirstUseEver);
				ImGui::Begin("Image", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

				if (texture.isValid()) {
					auto w = texture.getWidth();
					auto h = texture.getHeight();

					while (w > 512.0f || h > 512.0f) {
						w /= 2.0f;
						h /= 2.0f;
					}

					ImGui::Image((void*)(intptr_t) texture.getTextureIndex(), ImVec2(w, h));

				} else {
					ImGui::TextUnformatted("No Image available");
				}

				ImGui::End();
			}
		}, fieldWidth);
	}

	static constexpr const char* name = "Image Output";
	static constexpr float fieldWidth = 120.0f;

protected:
	OtTexture texture;
	bool showImage = false;
};


//
//	OtOutputNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Output", CLASS::name)

void OtOutputNodesRegister(OtGraph& graph) {
	REGISTER(OtGraphNodeFloatOutput);
	REGISTER(OtGraphNodeImageOutput);
}
