//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "OtPath.h"

#include "OtImage.h"

#include "OtNodesFactory.h"


//
//	OtSaveImageNode
//

class OtSaveImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", image)->addCustomRenderer([&](float width) {
			// render button to save image if valid
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
			ImVec2 maxSize = ImGui::GetMainViewport()->Size;
			ImVec2 minSize = ImVec2(maxSize.x * 0.5f, maxSize.y * 0.5f);

			if (ImGuiFileDialog::Instance()->Display("image-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// save file (if required)
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto dialog = ImGuiFileDialog::Instance();
					auto path = OtPath::join(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
					path = OtPath::replaceExtension(path, ".png");
					OtPath::changeDirectory(OtPath::getParent(path));
					image.saveToPNG(path);
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}
		});
	}

	static constexpr const char* nodeName = "Save Image to File";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::save;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtImage image;
};

static OtNodesFactoryRegister<OtSaveImageNode> registration;
