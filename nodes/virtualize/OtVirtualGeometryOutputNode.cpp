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

#include "OtGeometryAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualGeometryOutputNode
//

class OtVirtualGeometryOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", geometry);
		addInputPin("Name", name);
	}

	// synchronize the asset with the incoming geometry
	void onExecute() override {
		if (!asset.isNull()) {
			asset->clearGeometry();
		}

		if (name.size()) {
			asset = "virtual:" + name;

			if (geometry.isValid()) {
				asset->setGeometry(geometry);
			}

		} else {
			asset.clear();
		}
	}

	static constexpr const char* nodeName = "Save Geometry To Virtual";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::virtualize;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

protected:
	OtGeometry geometry;
	std::string name;
	OtAsset<OtGeometryAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualGeometryOutputNode> type;
