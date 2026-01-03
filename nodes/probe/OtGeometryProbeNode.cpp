//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"
#include "OtUi.h"

#include "OtGeometry.h"
#include "OtMeshPreview.h"

#include "OtNodesFactory.h"


//
//	OtGeometryProbeNode
//

class OtGeometryProbeNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Geometry", geometry);
	}

	// convert geometry to mesh
	inline void onExecute() override {
		if (geometry.isValid()) {
			customW = size;
			customH = size;

		} else {
			customW = size;
			customH = ImGui::GetFrameHeightWithSpacing();

		}

		needsSizing = true;
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		if (geometry.isValid()) {
			OtUi::hSpacer((itemWidth - customW) / 2.0f);
			preview.render(static_cast<int>(size), static_cast<int>(size), geometry.getMesh(), context);

			if (ImGui::BeginPopupContextItem("Geometry Context")) {
				OtUi::header("Settings:");
				ImGui::Spacing();
				OtUi::toggleButton("Wireframe", &context.wireframe);
				ImGui::ColorEdit3("Light Color", glm::value_ptr(context.lightColor));
				ImGui::ColorEdit3("Mesh Color", glm::value_ptr(context.meshColor));

				ImGui::TextUnformatted("");
				OtUi::header("Statistics:");
				OtUi::readonlySizeT("Vertices", geometry.getMesh().getVertexCount());
				OtUi::readonlySizeT("Triangles", geometry.getMesh().getTriangleCount());
				ImGui::Spacing();
				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiPopupFlags_MouseButtonLeft)) {
				ImGui::OpenPopup("Geometry Popup");
			}

			if (ImGui::BeginPopup("Geometry Popup", ImGuiWindowFlags_NoMove)) {
				auto popupSize = ImGui::GetMainViewport()->WorkSize.y * 0.75f;
				popupPreview.render(static_cast<int>(popupSize), static_cast<int>(popupSize), geometry.getMesh(), context);
				ImGui::EndPopup();
			}

		} else {
			ImGui::TextUnformatted("No geometry available");
		}
	}

	inline float getCustomRenderingWidth() override {
		return customW;
	}

	inline float getCustomRenderingHeight() override {
		return customH;
	}

	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["azimuth"] = context.azimuth;
		(*data)["elevation"] = context.elevation;
		(*data)["zoom"] = context.zoom;
		(*data)["meshColor"] = context.meshColor;
		(*data)["lightColor"] = context.lightColor;
		(*data)["wireframe"] = context.wireframe;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		context.azimuth = data->value("azimuth", 0.0f);
		context.elevation = data->value("elevation", 0.0f);
		context.zoom = data->value("zoom", 0.95f);
		context.meshColor = data->value("meshColor", glm::vec3{1.0f, 0.85f, 0.0f});
		context.lightColor = data->value("lightColor", glm::vec3{1.0f});
		context.wireframe = data->value("wireframe", false);
	}

	static constexpr const char* nodeName = "Geometry Probe";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::probe;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;
	static constexpr float size = 170.0f;

private:
	// properties
	OtGeometry geometry;

	OtMeshPreview preview;
	OtMeshPreview popupPreview;
	OtMeshPreview::Context context;

	float customW;
	float customH;
};

static OtNodesFactoryRegister<OtGeometryProbeNode> registration;
