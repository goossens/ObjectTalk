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

#include "OtInstances.h"
#include "OtPathTools.h"

#include "OtNodesFactory.h"


//
//	OtInstancesOutputNode
//

class OtInstancesOutputNode : public OtNodeClass {
public:
	// constructor
	inline OtInstancesOutputNode() : OtNodeClass(name, OtNodeClass::output) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", instances)->addRenderer([&](float width) {
			// render button to save instances if instances is valid
			if (!instances.isValid()) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				IGFD::FileDialogConfig config;
				config.countSelectionMax = 1;

				config.flags = ImGuiFileDialogFlags_Modal |
						ImGuiFileDialogFlags_DontShowHiddenFiles |
						ImGuiFileDialogFlags_ConfirmOverwrite;

				ImGuiFileDialog::Instance()->OpenDialog("instances-saveas", "Save Instances as...", ".oti", config);
			}

			if (!instances.isValid()) {
				ImGui::EndDisabled();
			}

			// handle saveas dialog
			ImVec2 maxSize = ImGui::GetIO().DisplaySize;
			ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

			if (ImGuiFileDialog::Instance()->Display("instances-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// open selected file if required
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto dialog = ImGuiFileDialog::Instance();
					auto path = OtPathJoin(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
					path = OtPathReplaceExtension(path, ".oti");
					OtPathChangeDirectory(OtPathGetParent(path));
					instances.save(path);
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}
		});
	}

	static constexpr const char* name = "Save Instances to OTI";

protected:
	OtInstances instances;
};

static OtNodesFactoryRegister<OtInstancesOutputNode> type("Output");
