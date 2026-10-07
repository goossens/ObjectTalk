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
#include "FileSelector.h"

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

			auto& selector = FileSelector::Instance();

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				selector.SaveAs({}, [&]() {
					auto pos = ImGui::GetCursorScreenPos();
					auto glyphWidth = ImGui::CalcTextSize("#").y;
					auto selectorWidth = glyphWidth * 5.0f;
					auto totalWidth = (7.0f * glyphWidth) + ImGui::GetStyle().ItemSpacing.x + selectorWidth;
					ImGui::SetCursorScreenPos(ImVec2(pos.x + (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f, pos.y));
					ImGui::AlignTextToFramePadding();
					ImGui::TextUnformatted("Format:");
					ImGui::SameLine();
					ImGui::SetNextItemWidth(selectorWidth);

					if (ImGui::BeginCombo("###format", formats[selectedFormat])) {
						for (size_t i = 0; i < formatCount; i++) {
							const auto isSelected = (selectedFormat == i);

							if (ImGui::Selectable(formats[i], isSelected)) {
								selectedFormat = i;
								selector.SetSelectedExtension(formatExtensions[i]);
							}

							if (isSelected) {
								ImGui::SetItemDefaultFocus();
							}
						}

						ImGui::EndCombo();
					}

				}, ImGui::GetFrameHeight());

				selectorOpen = true;
			}

			if (!manifold.isValid()) {
				ImGui::EndDisabled();
			}

			// handle save as selector
			if (selectorOpen && selector.Render()) {
				// save file (if required)
				if (selector.SelectedSaveAs()) {
					auto path = OtPath::toString(selector.GetSelectedPath());
					path = OtPath::replaceExtension(path, formatExtensions[selectedFormat]);

					try {
						manifold.save(path);

					} catch (OtException& e) {
						errorMessage = e.what();
						ImGui::OpenPopup("Save Manifold Error");
					}
				}

				selectorOpen = false;
			}

			if (ImGui::BeginPopupModal("Save Manifold Error")) {
				ImGui::Text("%s\n", errorMessage.c_str());
				ImGui::Separator();

				static constexpr float buttonWidth = 80.0f;
				OtUi::hSpacer(ImGui::GetContentRegionAvail().x - buttonWidth);

				if (ImGui::Button("OK", ImVec2(buttonWidth, 0.0f)) || ImGui::Shortcut(ImGuiKey_Escape)) {
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
	bool selectorOpen = false;

	static constexpr const char* formats[] = { "GLB", "GLTF", "OBJ", "STL" };
	static constexpr const char* formatExtensions[] = { ".glb", ".gltf", ".obj", ".stl" };
	static constexpr size_t formatCount = sizeof(formats) / sizeof(formats[0]);
	size_t selectedFormat = 0;
};

static OtNodesFactoryRegister<OtSaveManifoldNode> registration;
