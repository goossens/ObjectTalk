//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"

#include "OtTexture.h"
#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtTextureProbeNode
//

class OtTextureProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Texture", texture);
	}

	// nothing to do on execute but we'll use it to determine texture size
	inline void onExecute() override {
		auto fieldWidth = 170.0f;

		if (texture.isValid()) {
			customW = std::min(fieldWidth, static_cast<float>(texture.getWidth()));
			customH = customW * texture.getHeight() / texture.getWidth();

		} else {
			customW = fieldWidth;
			customH = ImGui::GetFrameHeightWithSpacing();
		}

		needsSizing = true;
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		if (texture.isValid()) {
			OtUi::hSpacer((itemWidth - customW) / 2.0f);
			ImGui::Image(texture.getTextureID(), ImVec2(customW, customH));

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiPopupFlags_MouseButtonLeft)) {
				ImGui::OpenPopup("Texture Popup");
			}

			if (ImGui::BeginPopup("Texture Popup")) {
				auto size = ImGui::GetMainViewport()->WorkSize;
				auto scale = std::min(size.x * 0.75f / customW, size.y * 0.75f / customH);
				ImGui::Image(texture.getTextureID(), ImVec2(scale * customW, scale * customH));
				ImGui::EndPopup();
			}

		} else {
			ImGui::TextUnformatted("No texture available");
		}
	}

	inline float getCustomRenderingWidth() override {
		return customW;
	}

	inline float getCustomRenderingHeight() override {
		return customH;
	}

	static constexpr const char* nodeName = "Texture Probe";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::probe;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtTexture texture;
	float customW;
	float customH;
};

static OtNodesFactoryRegister<OtTextureProbeNode> registration;
