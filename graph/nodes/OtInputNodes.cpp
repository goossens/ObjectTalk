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

#include "OtPathTools.h"
#include "OtTexture.h"
#include "OtTextureAsset.h"
#include "OtUi.h"

#include "OtGraphNode.h"
#include "OtInputNodes.h"


//
//	OtGraphNodeIntegerInput
//

class OtGraphNodeIntegerInput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeIntegerInput() : OtGraphNodeClass(name, OtGraphNodeClass::input) {}

	// configure node
	inline void configure() override {
		addOutputPin("Value", value)->addRenderer([this] () {
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::InputInt("##value", &value, 1, 100, ImGuiInputTextFlags_NoUndoRedo);

			if (ImGui::IsItemActivated()) {
				oldState = serialize().dump();
			}

			if (ImGui::IsItemDeactivated()) {
				newState = serialize().dump();

				if (newState != oldState) {
					needsEvaluating = true;
					needsSaving = true;
				}
			}
		}, fieldWidth);
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		(*data)["value"] = value;
	}

	void customDeserialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		value = data->value("value", 0.);
	}

	static constexpr const char* name = "Integer Input";
	static constexpr float fieldWidth = 120.0f;

protected:
	int value = 0;
};


//
//	OtGraphNodeFloatInput
//

class OtGraphNodeFloatInput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeFloatInput() : OtGraphNodeClass(name, OtGraphNodeClass::input) {}

	// configure node
	inline void configure() override {
		addOutputPin("Value", value)->addRenderer([this] () {
			ImGui::SetNextItemWidth(fieldWidth);
			ImGui::InputFloat("##value", &value, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_NoUndoRedo);

			if (ImGui::IsItemActivated()) {
				oldState = serialize().dump();
			}

			if (ImGui::IsItemDeactivated()) {
				newState = serialize().dump();

				if (newState != oldState) {
					needsEvaluating = true;
					needsSaving = true;
				}
			}
		}, fieldWidth);
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		(*data)["value"] = value;
	}

	void customDeserialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		value = data->value("value", 0.0f);
	}

	static constexpr const char* name = "Float Input";
	static constexpr float fieldWidth = 120.0f;

protected:
	float value = 0.0f;
};


//
//	OtGraphNodeImageInput
//

class OtGraphNodeImageInput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeImageInput() : OtGraphNodeClass(name, OtGraphNodeClass::input) {}

	// configure node
	inline void configure() override {
		addOutputPin("Image", texture)->addRenderer([this] () {
			ImGui::SetNextItemWidth(fieldWidth);
			auto old = serialize().dump();

			if (asset.renderUI("##image")) {
				if (asset.isNull()) {
					needsEvaluating = true;

				} else {
					loading = true;
				}

				texture.clear();
				oldState = old;
				newState = serialize().dump();
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// check for completion of texture load
	inline void onUpdate() override {
		if (loading && asset->isReady()) {
			texture = asset->getTexture();
			needsEvaluating = true;
			loading = false;
		}
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		(*data)["image"] = OtPathGetRelative(asset.getPath(), basedir);
	}

	void customDeserialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		asset = OtPathGetAbsolute(*data, "image", basedir);

		if (asset.isNull()) {
			texture.clear();

		} else if (asset.isReady()) {
			texture = asset->getTexture();
			needsEvaluating = true;

		} else {
			loading = true;
		}
	}

	static constexpr const char* name = "Image Input";
	static constexpr float fieldWidth = 180.0f;

protected:
	OtAsset<OtTextureAsset> asset;
	OtTexture texture;
	bool loading = false;
};


//
//	OtInputNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Input", CLASS::name)

void OtInputNodesRegister(OtGraph& graph) {
	REGISTER(OtGraphNodeIntegerInput);
	REGISTER(OtGraphNodeFloatInput);
	REGISTER(OtGraphNodeImageInput);
}
