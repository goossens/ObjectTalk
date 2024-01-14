//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtPathTools.h"
#include "OtTexture.h"
#include "OtTextureAsset.h"
#include "OtUi.h"

#include "OtGraphNode.h"
#include "OtInputNodes.h"


//
//	OtGraphNodeFloatInput
//

#include <iostream>
class OtGraphNodeFloatInput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeFloatInput() : OtGraphNodeClass(name) {}

	// configure node
	inline void configure() override {
		addOutputPin("Value", value)->addRenderer([this] () {
			static constexpr int flags =
				ImGuiInputTextFlags_NoUndoRedo |
				ImGuiInputTextFlags_EnterReturnsTrue;

			oldState = serialize().dump();
			ImGui::SetNextItemWidth(fieldWidth);

			if (ImGui::InputFloat("##value", &value, 0.0f, 0.0f, "%.3f", flags)) {
				newState = serialize().dump();
				needsRunning = true;
				needsSaving = true;
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
	inline OtGraphNodeImageInput() : OtGraphNodeClass(name) {}

	// configure node
	inline void configure() override {
		addOutputPin("Image", texture)->addRenderer([this] () {
			ImGui::SetNextItemWidth(fieldWidth);
			oldState = serialize().dump();

			if (asset.renderUI("##image")) {
				if (asset.isNull()) {
					needsRunning = true;

				} else {
					loading = true;
				}

				newState = serialize().dump();
				texture.clear();
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// check for completion of texture load
	inline void onUpdate() override {
		if (loading && asset->isReady()) {
			texture = asset->getTexture();
			needsRunning = true;
			loading = false;
		}
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		(*data)["image"] = OtPathGetRelative(asset.getPath(), basedir);
	}

	void customDeserialize(nlohmann::json* data, std::filesystem::path* basedir) override {
		asset = OtPathGetAbsolute(*data, "image", basedir);

		texture.clear();

		if (!asset.isNull()) {
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
	REGISTER(OtGraphNodeFloatInput);
	REGISTER(OtGraphNodeImageInput);
}
