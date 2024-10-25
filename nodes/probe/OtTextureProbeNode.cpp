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

#include "OtNodesFactory.h"


//
//	OtTextureProbeNode
//

class OtTextureProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		auto pin = addInputPin("Texture", texture);
	}

	// nothing to do on execute but we'll use it to determine texture size
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

	// render custom fields
	void customRendering(float width) override {
		if (texture.isValid()) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (width - customW) / 2.0f);
			ImGui::Image((ImTextureID)(intptr_t) texture.getIndex(), ImVec2(customW, customH));

		} else {
			ImGui::TextUnformatted("No Texture available");
		}
	}

	float getCustomRenderingWidth() override {
		return customW;
	}

	float getCustomRenderingHeight() override {
		return customH;
	}

	static constexpr const char* nodeName = "Texture Probe";
	static constexpr int nodeCategory = OtNodeClass::probe;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 170.0f;

protected:
	OtTexture texture;
	float customW;
	float customH;
};

static OtNodesFactoryRegister<OtTextureProbeNode> type;
