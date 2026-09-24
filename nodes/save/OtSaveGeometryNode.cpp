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

#include "OtGeometry.h"

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

			auto& dialog = FileSelector::Instance();

			if (ImGui::Button("Save As...", ImVec2(width, 0.0f))) {
				dialog.SaveAs();
				selectorOpen = true;
			}

			if (!geometry.isValid()) {
				ImGui::EndDisabled();
			}

			// handle saveas dialog
			if (selectorOpen && dialog.Render()) {
				// save file (if required)
				if (dialog.SelectedSaveAs()) {
					auto path = OtPath::toString(dialog.GetSelectedPath());
					path = OtPath::replaceExtension(path, ".obj");
					geometry.save(path);
				}

				selectorOpen = false;
			}
		});
	}

	static constexpr const char* nodeName = "Save Geometry to File";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::save;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtGeometry geometry;
	bool selectorOpen = false;
};

static OtNodesFactoryRegister<OtSaveGeometryNode> registration;
