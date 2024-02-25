//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "OtGeometryAsset.h"
#include "OtPathTools.h"

#include "OtNodesFactory.h"


//
//	OtGeometryOutputNode
//

class OtGeometryOutputNode : public OtNodeClass {
public:
	// constructor
	inline OtGeometryOutputNode() : OtNodeClass(name, OtNodeClass::output) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", geometry)->addRenderer([&](float width) {
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
			ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

			if (ImGuiFileDialog::Instance()->Display("geometry-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// open selected file if required
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto dialog = ImGuiFileDialog::Instance();
					auto path = OtPathJoin(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
					path = OtPathReplaceExtension(path, ".obj");
					OtPathChangeDirectory(OtPathGetParent(path));
					geometry.save(path);
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}
		});
	}

	static constexpr const char* name = "Save Geometry To OBJ";

protected:
	OtGeometry geometry;
};

static OtNodesFactoryRegister<OtGeometryOutputNode> type("Output");
