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


#include "OtShape.h"
#include "OtShapeAsset.h"

#include "OtNodesFactory.h"


//
//	OtShapeInputNode
//

class OtShapeInputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addOutputPin("Shape", shape)->addCustomRenderer([this](float width) {
			auto old = serialize().dump();

			if (customInputRendering(width)) {
				if (asset.isNull()) {
					shape.clear();
					needsEvaluating = true;

				} else if (asset.isReady()) {
					shape = asset->getShape();
					needsEvaluating = true;

				} else {
					shape.clear();
				}

				oldState = old;
				newState = serialize().dump();
				needsSaving = true;
			}
		}, 180.0f);
	}

	// special rendering for input nodes
	inline bool customInputRendering(float width) override {
		ImGui::SetNextItemWidth(width);
		return asset.renderUI("##shape");
	}

	// update status by checking for errors or load completion
	inline bool onUpdate() override {
		error.clear();

		if (asset.isNull() || asset.isMissing() || asset.isInvalid()) {
			shape.clear();
			return false;

		} else if (asset->getShape() != shape) {
			shape = asset->getShape();
			return true;

		} else {
			return false;
		}
	}

	// (de)serialize node
	void customSerialize(nlohmann::json* data, std::string* basedir) override {
		(*data)["path"] = OtAssetSerialize(asset.getPath(), basedir);
	}

	void customDeserialize(nlohmann::json* data, std::string* basedir) override {
		asset = OtAssetDeserialize(data, "path", basedir);

		if (asset.isNull()) {
			shape.clear();

		} else if (asset.isReady()) {
			shape = asset->getShape();
		}
	}

	static constexpr const char* nodeName = "Shape Input";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::input;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtAsset<OtShapeAsset> asset;
	OtShape shape;
};

static OtNodesFactoryRegister<OtShapeInputNode> type;
