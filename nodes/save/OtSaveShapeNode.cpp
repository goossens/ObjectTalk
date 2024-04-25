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

#include "OtPathTools.h"
#include "OtShape.h"

#include "OtNodesFactory.h"


//
//	OtSaveShapeNode
//

class OtSaveShapeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", shape)->addCustomRenderer([&](float width) {
			// render button to save shape if shape is valid
			if (!shape.isValid()) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				IGFD::FileDialogConfig config;
				config.countSelectionMax = 1;

				config.flags = ImGuiFileDialogFlags_Modal |
						ImGuiFileDialogFlags_DontShowHiddenFiles |
						ImGuiFileDialogFlags_ConfirmOverwrite;

				ImGuiFileDialog::Instance()->OpenDialog("shape-saveas", "Save Shape as...", ".shape", config);
			}

			if (!shape.isValid()) {
				ImGui::EndDisabled();
			}

			// handle saveas dialog
			ImVec2 maxSize = ImGui::GetIO().DisplaySize;
			ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

			if (ImGuiFileDialog::Instance()->Display("shape-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// save file (if required)
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto dialog = ImGuiFileDialog::Instance();
					auto path = OtPathJoin(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
					path = OtPathReplaceExtension(path, ".shape");
					OtPathChangeDirectory(OtPathGetParent(path));
					shape.save(path);
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}
		});
	}

	static constexpr const char* nodeName = "Save Shape to File";
	static constexpr int nodeCategory = OtNodeClass::save;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtShape shape;
};

static OtNodesFactoryRegister<OtSaveShapeNode> type;
