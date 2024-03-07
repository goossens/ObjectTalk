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

#include "OtImage.h"
#include "OtPathTools.h"

#include "OtNodesFactory.h"


//
//	OtSaveImageNode
//

class OtSaveImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", image)->addRenderer([&](float width) {
			// render button to save image if image is valid
			if (!image.isValid()) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				IGFD::FileDialogConfig config;
				config.countSelectionMax = 1;

				config.flags = ImGuiFileDialogFlags_Modal |
						ImGuiFileDialogFlags_DontShowHiddenFiles |
						ImGuiFileDialogFlags_ConfirmOverwrite;

				ImGuiFileDialog::Instance()->OpenDialog("image-saveas", "Save Image as...", ".png", config);
			}

			if (!image.isValid()) {
				ImGui::EndDisabled();
			}

			// handle saveas dialog
			ImVec2 maxSize = ImGui::GetIO().DisplaySize;
			ImVec2 minSize = ImVec2(maxSize.x * 0.5, maxSize.y * 0.5);

			if (ImGuiFileDialog::Instance()->Display("image-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// open selected file if required
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto dialog = ImGuiFileDialog::Instance();
					auto path = OtPathJoin(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
					path = OtPathReplaceExtension(path, ".png");
					OtPathChangeDirectory(OtPathGetParent(path));
					image.saveToPNG(path);
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}
		});
	}

	static constexpr const char* nodeName = "Save Image To PNG";
	static constexpr int nodeCategory = OtNodeClass::save;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtImage image;
};

static OtNodesFactoryRegister<OtSaveImageNode> type;
