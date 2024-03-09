//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtVectorInputNode
//

class OtVectorInputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Value", value)->addRenderer([this](float width) {
			auto old = serialize().dump();
			ImGui::SetNextItemWidth(width);

			if (OtUiEditVec3("##value", value, 0.1f, 0.0f, 0.0f)) {
				oldState = old;
				newState = serialize().dump();

				if (newState != oldState) {
					needsEvaluating = true;
					needsSaving = true;
				}
			}
		}, fieldWidth);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		return OtUiEditVec3("##value", value, 0.1f, 0.0f, 0.0f);
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["value"] = value;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		value = data->value("value", glm::vec3(0.0f));
	}

	static constexpr const char* nodeName = "Vector Input";
	static constexpr int nodeCategory = OtNodeClass::input;
	static constexpr int nodeKind = OtNodeClass::fixed;
	static constexpr float fieldWidth = 200.0f;

protected:
	glm::vec3 value{0.0f};
};

static OtNodesFactoryRegister<OtVectorInputNode> type;
