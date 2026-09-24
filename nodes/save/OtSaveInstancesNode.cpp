//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "FileSelector.h"

#include "OtPath.h"

#include "OtInstances.h"

#include "OtNodesFactory.h"


//
//	OtSaveInstancesNode
//

class OtSaveInstancesNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", instances)->addCustomRenderer([&](float width) {
			// render button to save instances if valid
			if (!instances.isValid()) {
				ImGui::BeginDisabled();
			}

			auto& dialog = FileSelector::Instance();

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				dialog.SaveAs();
				selectorOpen = true;
			}

			if (!instances.isValid()) {
				ImGui::EndDisabled();
			}

			// handle save as dialog
			if (selectorOpen && dialog.Render()) {
				// save file (if required)
				if (dialog.SelectedSaveAs()) {
					auto path = OtPath::toString(dialog.GetSelectedPath());
					path = OtPath::replaceExtension(path, ".oti");
					instances.save(path);
				}

				selectorOpen = false;
			}
		});
	}

	static constexpr const char* nodeName = "Save Instances to File";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::save;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtInstances instances;
	bool selectorOpen = false;
};

static OtNodesFactoryRegister<OtSaveInstancesNode> registration;
