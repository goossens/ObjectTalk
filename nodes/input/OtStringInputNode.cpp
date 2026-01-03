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
#include "nlohmann/json.hpp"

#include "OtUi.h"

#include "OtNodesFactory.h"


//
//	OtStringInputNode
//

class OtStringInputNode : public OtNodeClass {
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
		}, 180.0f);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		auto text = value;

		if (OtUi::inputText("##value", &text)) {
			value = text;
			return true;

		} else {
			return false;
		}
	}


	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["value"] = value;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		value = data->value("value", "");
	}

	static constexpr const char* nodeName = "String Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	std::string value = "";
};

static OtNodesFactoryRegister<OtStringInputNode> registration;
