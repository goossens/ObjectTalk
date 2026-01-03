//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "OtException.h"
#include "OtPath.h"

#include "OtManifold.h"
#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtSaveManifoldNode
//

class OtSaveManifoldNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", manifold)->addCustomRenderer([&](float width) {
			// render button to save manifold if valid
			if (!manifold.isValid()) {
				ImGui::BeginDisabled();
			}

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				IGFD::FileDialogConfig config;
				config.countSelectionMax = 1;

				config.flags = ImGuiFileDialogFlags_Modal |
						ImGuiFileDialogFlags_DontShowHiddenFiles |
						ImGuiFileDialogFlags_ConfirmOverwrite;

				ImGuiFileDialog::Instance()->OpenDialog("manifold-saveas", "Save Manifold as...", ".stl,.gltf,.obj", config);
			}

			if (!manifold.isValid()) {
				ImGui::EndDisabled();
			}

			// handle saveas dialog
			ImVec2 maxSize = ImGui::GetMainViewport()->Size;
			ImVec2 minSize = ImVec2(maxSize.x * 0.5f, maxSize.y * 0.5f);

			if (ImGuiFileDialog::Instance()->Display("manifold-saveas", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
				// save file (if required)
				if (ImGuiFileDialog::Instance()->IsOk()) {
					auto dialog = ImGuiFileDialog::Instance();
					auto path = OtPath::join(dialog->GetCurrentPath(), dialog->GetCurrentFileName());
					OtPath::changeDirectory(OtPath::getParent(path));

					try {
						manifold.save(path);

					} catch (OtException& e) {
						errorMessage = e.what();
						ImGui::OpenPopup("Save Manifold Error");
					}
				}

				// close dialog
				ImGuiFileDialog::Instance()->Close();
			}

			if (ImGui::BeginPopupModal("Save Manifold Error", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("%s\n", errorMessage.c_str());
				ImGui::Separator();

				static constexpr float buttonWidth = 80.0f;
				OtUi::hSpacer(ImGui::GetContentRegionAvail().x - buttonWidth);

				if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f)) || ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		});
	}

	static constexpr const char* nodeName = "Save Manifold to File";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::save;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtManifold manifold;
	std::string errorMessage;
};

static OtNodesFactoryRegister<OtSaveManifoldNode> registration;
