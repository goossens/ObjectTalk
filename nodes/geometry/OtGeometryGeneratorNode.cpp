//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGeometry.h"
#include "OtPrimitiveFactory.h"

#include "OtNodesFactory.h"


//
//	OtGeometryGeneratorNode
//

class OtGeometryGeneratorNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		primitive = factory.create("Cube");
		addOutputPin("Geometry", geometry);
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		// render button
		auto geometryType = primitive->getTypeName();

		if (ImGui::Button(geometryType, ImVec2(itemWidth, 0.0f))) {
			ImGui::OpenPopup("primitivePopup");
		}

		// open popup (if required)
		if (ImGui::BeginPopup("primitivePopup")) {
			auto old = serialize().dump();

			bool changed = false;

			if (ImGui::BeginCombo("Type", geometryType)) {
				factory.each([&](const char* name) {
					bool isSelectedOne = !std::strcmp(geometryType, name);

					if (ImGui::Selectable(name, isSelectedOne)) {
						if (std::strcmp(geometryType, name)) {
							primitive = factory.create(name);
							changed = true;
						}
					}

					// ensure selected entry is in focus
					if (isSelectedOne) {
						ImGui::SetItemDefaultFocus();
					}
				});

				ImGui::EndCombo();
			}

			changed |= primitive->renderUI();

			if (changed) {
				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}

			ImGui::EndPopup();
		}
	}

	inline float getCustomRenderingWidth() override {
		return 150.0f;
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["primitive"] = primitive->serialize(basedir);
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		if (data->contains("primitive") && (*data)["primitive"].contains("type") ) {
			std::string geometryType = (*data)["primitive"]["type"];
			primitive = factory.create(geometryType);
		}

		primitive->deserialize((*data)["primitive"], basedir);
	}

	// create the geometry
	inline void onExecute() override {
		geometry.setMesh(primitive->createMesh());
	}

	static constexpr const char* nodeName = "Geometry Generator";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::geometry;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// our primitive and geometry
	std::shared_ptr<OtPrimitiveBase> primitive;
	OtGeometry geometry;

	// factory to create primitives
	OtPrimitiveFactory factory;
};

static OtNodesFactoryRegister<OtGeometryGeneratorNode> registration;
