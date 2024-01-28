//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtTextureAsset.h"

#include "OtGraphNode.h"
#include "OtOutputNodes.h"


//
//	OtGraphNodeVirtualTextureOutput
//

class OtGraphNodeVirtualTextureOutput : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNodeVirtualTextureOutput() : OtGraphNodeClass(name, OtGraphNodeClass::output) {}

	// configure node
	inline void configure() override {
		addInputPin("Input", texture)->addRenderer([&](float width) {
			ImGui::SetNextItemWidth(width);
			auto old = serialize().dump();

			OtTextureAsset* oldAsset = nullptr;

			if (!asset.isNull()) {
				oldAsset = &(*asset);
			}

			if (asset.renderVirtualUI("##name")) {
				if (oldAsset && asset.isNull()) {
					oldAsset->clearTexture();
				}

				oldState = old;
				newState = serialize().dump();
				needsEvaluating = true;
				needsSaving = true;
			}
		}, fieldWidth);
	}

	// nothing to do on execute but we'll use it to determine texture size
	void onExecute() override {
		if (asset.isReady()) {
			asset->setTexture(texture);

		} else {
			asset.clear();
		}
	}

	// (de)serialize input
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		auto t = asset.getPath();
		(*data)["path"] = asset.getPath();
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = data->value("path", "");
	}

	static constexpr const char* name = "Virtual Texture Output";
	static constexpr float fieldWidth = 170.0f;

protected:
	OtTexture texture;
	OtAsset<OtTextureAsset> asset;
};


//
//	OtOutputNodesRegister
//

#define REGISTER(CLASS) \
	graph.registerNodeType<CLASS>("Output", CLASS::name)

void OtOutputNodesRegister(OtGraph& graph) {
	REGISTER(OtGraphNodeVirtualTextureOutput);
}
