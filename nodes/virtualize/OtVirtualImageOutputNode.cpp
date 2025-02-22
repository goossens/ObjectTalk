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

#include "OtImageAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualImageOutputNode
//

class OtVirtualImageOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", image);
		addInputPin("Name", name);
	}

	// synchronize the asset with the incoming image
	void onExecute() override {
		if (!asset.isNull()) {
			asset->clearImage();
		}

		if (name.size()) {
			asset = "virtual:" + name;

			if (image.isValid()) {
				asset->setImage(image);
			}

		} else {
			asset.clear();
		}
	}

	static constexpr const char* nodeName = "Save Image To Virtual";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::virtualize;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtImage image;
	std::string name;
	OtAsset<OtImageAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualImageOutputNode> registration;
