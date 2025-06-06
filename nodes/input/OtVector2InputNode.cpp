//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtGlm.h"
#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtVector2InputNode
//

class OtVector2InputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Value", value)->addCustomRenderer([this](float width) {
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				oldState = old;
				newState = serialize().dump();

				if (newState != oldState) {
					needsEvaluating = true;
					needsSaving = true;
				}
			}
		}, 130.0f);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		return OtUi::editVec2("##value", &value);
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, std::string* /* basedir */) override {
		(*data)["value"] = value;
	}

	inline void customDeserialize(nlohmann::json* data, std::string* /* basedir */) override {
		value = data->value("value", glm::vec2(0.0f));
	}

	static constexpr const char* nodeName = "Vector2 Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	glm::vec2 value{0.0f};
};

static OtNodesFactoryRegister<OtVector2InputNode> registration;
