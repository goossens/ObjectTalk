//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "nlohmann/json.hpp"

#include "OtImage.h"
#include "OtThreadPool.h"
#include "OtUi.h"

#include "OtWorld.h"

#include "OtNodesFactory.h"


//
//	OtWorldToImageNode
//

class OtWorldToImageNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("World", world);
		addInputPin("Size", size);
		addOutputPin("Image", image);
	}

	// render custom fields
	inline void customRendering(float itemWidth) override {
		ImGui::SetNextItemWidth(itemWidth);
		auto old = serialize().dump();

		if (OtUi::selectorEnum("##renderType", &renderType, OtWorld::renderTypes, OtWorld::renderTypeCount)) {
			oldState = old;
			newState = serialize().dump();
			needsEvaluating = true;
			needsSaving = true;
		}

		if (generating) {
			auto pos = ImGui::GetCursorScreenPos();
			OtUi::spinner(ImVec2(pos.x + itemWidth * 0.5f, pos.y), OtUi::size(1.0f));
		}
	}

	inline float getCustomRenderingWidth() override {
		return OtUi::size(9.0f);
	}

	inline float getCustomRenderingHeight() override {
		return ImGui::GetFrameHeightWithSpacing();
	}

	// (de)serialize node
	inline void customSerialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		(*data)["renderType"] = renderType;
	}

	inline void customDeserialize(nlohmann::json* data, [[maybe_unused]] std::string* basedir) override {
		renderType = data->value("renderType", OtWorld::RenderType::biomes);
	}

	// update node status
	inline bool onUpdate() override {
		// limit values
		size = std::clamp(size, 32, 1024);

		if (generated) {
			std::swap(image, newImage);
			generated = false;

			if (moreRequests) {
				scheduleGeneration();
				moreRequests = false;

			} else {
				generating = false;
			}

			return true;

		} else {
			return false;
		}
	}

	// execute asynchronous image generation
	inline void onExecute() override {
		if (world.isValid()) {
			if (generating) {
				moreRequests = true;

			} else {
				generating = true;
				scheduleGeneration();
			}

		} else {
			image.clear();
		}
	}

	static constexpr const char* nodeName = "World to Image";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::world;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	int size = 32;
	OtWorld::RenderType renderType = OtWorld::RenderType::biomes;

	// world component
	OtWorld world;
	OtImage image;

	// work variables
	bool generating = false;
	bool generated = false;
	bool moreRequests = false;
	OtImage newImage;

	// local functions
	void scheduleGeneration() {
		OtThreadPool::run([captureWorld = world, this]() {
			captureWorld.render(newImage, size, renderType);
			generated = true;
		});
	}
};


static OtNodesFactoryRegister<OtWorldToImageNode> registration;
