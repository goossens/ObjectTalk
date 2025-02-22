//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "OtGeometryAsset.h"
#include "OtPath.h"

#include "OtNodesFactory.h"


//
//	OtSaveGeometryNode
//

class OtSaveGeometryNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", geometry)->addCustomRenderer([&](float width) {
			// render button to save geometry if geometry is valid
			if (!geometry.isValid()) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				IGFD::FileDialogConfig config;
				config.countSelectionMax = 1;

				config.flags = ImGuiFileDialogFlags_Modal |
						ImGuiFileDialogFlags_DontShowHiddenFiles |
						ImGuiFileDialogFlags_ConfirmOverwrite;

				ImGuiFileDialog::Instance()->OpenDialog("geometry-saveas", "Save Image as...", ".obj", config);
			}

			if (!geometry.isValid()) {
				ImGui::EndDisabled();
			}

			// handle saveas dialog
			ImVec2 maxSize = ImGui::GetIO().DisplaySize;
			ImVec2 minSize = ImVec2(maxSize.x * 0.5f, maxSize.y * 0.5f);

			if (ImGuiFileDialog::Instance()->Display("geometry-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// save file (if required)
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto dialog = ImGuiFileDialog::Instance();
					auto path = OtPath::join(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
					path = OtPath::replaceExtension(path, ".obj");
					OtPath::changeDirectory(OtPath::getParent(path));
					geometry.save(path);
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}
		});
	}

	static constexpr const char* nodeName = "Save Geometry To File";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::save;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtGeometry geometry;
};

static OtNodesFactoryRegister<OtSaveGeometryNode> registration;
