//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"

#include "OtImage.h"
#include "OtShape.h"
#include "OtTexture.h"
#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtShapeProbeNode
//

class OtShapeProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Shape", shape);
	}

	// convert shape to texture and determine size
	inline void onExecute() override {
		auto fieldWidth = 170.0f;

		if (shape.isValid()) {
			shape.renderFill(image, OtColor(1.0, 1.0, 1.0, 1.0));
			texture.load(image);
			customW = std::min(fieldWidth, static_cast<float>(image.getWidth()));
			customH = customW * image.getHeight() / image.getWidth();

		} else {
			customW = fieldWidth;
			customH = ImGui::GetFrameHeightWithSpacing();
		}

		needsSizing = true;
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		if (shape.isValid()) {
			OtUi::hSpacer((itemWidth - customW) / 2.0f);
			ImGui::Image(texture.getTextureID(), ImVec2(customW, customH));

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiPopupFlags_MouseButtonLeft)) {
				ImGui::OpenPopup("Shape Popup");
			}

			if (ImGui::BeginPopup("Shape Popup")) {
				auto size = ImGui::GetMainViewport()->WorkSize;
				auto scale = std::min(size.x * 0.75f / customW, size.y * 0.75f / customH);
				ImGui::Image(texture.getTextureID(), ImVec2(scale * customW, scale * customH));
				ImGui::EndPopup();
			}

		} else {
			ImGui::TextUnformatted("No shape available");
		}
	}

	inline float getCustomRenderingWidth() override {
		return customW;
	}

	inline float getCustomRenderingHeight() override {
		return customH;
	}

	static constexpr const char* nodeName = "Shape Probe";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::probe;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtShape shape;
	OtImage image;
	OtTexture texture;
	float customW;
	float customH;
};

static OtNodesFactoryRegister<OtShapeProbeNode> registration;
