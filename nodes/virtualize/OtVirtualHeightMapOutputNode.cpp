//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtHeightMapAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualHeightMapOutputNode
//

class OtVirtualHeightMapOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", heightMap);
		addInputPin("Name", name);
	}

	// synchronize the asset with the incoming heightMap
	inline void onExecute() override {
		if (!asset.isNull()) {
			asset->clearHeightMap();
		}

		if (name.size()) {
			asset = "virtual:" + name;

			if (heightMap.isValid()) {
				asset->setHeightMap(heightMap);
			}

		} else {
			asset.clear();
		}
	}

	static constexpr const char* nodeName = "Save Height Map to Virtual";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::virtualize;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtHeightMap heightMap;
	std::string name;
	OtAsset<OtHeightMapAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualHeightMapOutputNode> registration;
