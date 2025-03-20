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

#include "OtManifold.h"
#include "OtMesh.h"
#include "OtMeshPreview.h"

#include "OtNodesFactory.h"


//
//	OtManifoldProbeNode
//

class OtManifoldProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Manifold", manifold);
	}

	// convert manifold to mesh
	void onExecute() override {
		if (manifold.isValid()) {
			manifold.createMesh(mesh);

			customW = size;
			customH = size;

		} else {
			customW = size;
			customH = ImGui::GetFrameHeightWithSpacing();

		}

		needsSizing = true;
	}

	// render custom fields
	void customRendering(float itemWidth) override {
		if (manifold.isValid()) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (itemWidth - size) / 2.0f);
			preview.setSize(static_cast<int>(size), static_cast<int>(size));
			preview.render(mesh);

			if (ImGui::IsItemClicked(ImGuiPopupFlags_MouseButtonLeft) && ImGui::IsMouseDoubleClicked(ImGuiPopupFlags_MouseButtonLeft)) {
				ImGui::OpenPopup("Manifold Popup");
				popupPreview.setAzimuth(preview.getAzimuth());
				popupPreview.setElivation(preview.getElevation());
			}

			if (ImGui::BeginPopup("Manifold Popup", ImGuiWindowFlags_NoMove)) {
				auto popupSize = ImGui::GetMainViewport()->WorkSize.y * 0.75f;
				popupPreview.setSize(static_cast<int>(popupSize), static_cast<int>(popupSize));
				popupPreview.render(mesh);
				ImGui::EndPopup();
			}

		} else {
			ImGui::TextUnformatted("No manifold available");
		}
	}

	float getCustomRenderingWidth() override {
		return customW;
	}

	float getCustomRenderingHeight() override {
		return customH;
	}

	static constexpr const char* nodeName = "Manifold Probe";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::probe;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;
	static constexpr float size = 170.0f;

protected:
	OtManifold manifold;
	OtMesh mesh;

	OtMeshPreview preview;
	OtMeshPreview popupPreview;

	float customW;
	float customH;
};

static OtNodesFactoryRegister<OtManifoldProbeNode> registration;
