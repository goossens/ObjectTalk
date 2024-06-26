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

#include "OtTextureAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualTextureOutputNode
//

class OtVirtualTextureOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", texture);
		addInputPin("Name", name);
	}

	// synchronize the asset with the incoming texture
	void onExecute() override {
		if (texture.isValid() && name.size()) {
			asset = "virtual:" + name;
			asset->setTexture(texture);

		} else if (name.size()) {
			asset = "virtual:" + name;
			asset->clearTexture();

		} else {
			asset.clear();
		}
	}

	static constexpr const char* nodeName = "Save Texture To Virtual";
	static constexpr int nodeCategory = OtNodeClass::virtualize;
	static constexpr int nodeKind = OtNodeClass::fixed;

protected:
	OtTexture texture;
	std::string name;
	OtAsset<OtTextureAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualTextureOutputNode> type;
