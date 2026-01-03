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

#include "OtColor.h"

#include "OtNodesFactory.h"


//
//	OtColorInputNode
//

class OtColorInputNode : public OtNodeClass {
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
		}, 50.0f);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGuiColorEditFlags flags =
			ImGuiColorEditFlags_Float |
			ImGuiColorEditFlags_NoInputs |
			ImGuiColorEditFlags_AlphaBar;

		ImGui::SetNextItemWidth(width);
		return ImGui::ColorEdit4("##value", value.data(), flags);
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["value"] = value;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		value = data->value("value", OtColor(0.0f));
	}

	static constexpr const char* nodeName = "Color Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtColor value{0.0f};
};

static OtNodesFactoryRegister<OtColorInputNode> registration;
