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
		if (image.isValid() && name.size()) {
			asset = "virtual:" + name;
			asset->setImage(image);

		} else if (name.size()) {
			asset = "virtual:" + name;
			asset->clearImage();

		} else {
			asset.clear();
		}
	}

	static constexpr const char* nodeName = "Save Image To Virtual";
	static constexpr int nodeCategory = OtNodeClass::virtualize;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtImage image;
	std::string name;
	OtAsset<OtImageAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualImageOutputNode> type;
