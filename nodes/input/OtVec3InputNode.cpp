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
//	OtVec3InputNode
//

class OtVec3InputNode : public OtNodeClass {
public:
	// constructor
	inline OtVec3InputNode() : OtNodeClass(name, OtNodeClass::input) {}

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

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["value"] = value;
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		value = data->value("value", glm::vec3(0.0f));
	}

	static constexpr const char* name = "Vec3 Input";
	static constexpr float fieldWidth = 200.0f;

protected:
	glm::vec3 value{0.0f};
};

static OtNodesFactoryRegister<OtVec3InputNode> type("Input");
