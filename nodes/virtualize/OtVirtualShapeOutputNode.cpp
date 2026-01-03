//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "imgui.h"
#include "nlohmann/json.hpp"

#include "OtShapeAsset.h"

#include "OtNodesFactory.h"


//
//	OtVirtualShapeOutputNode
//

class OtVirtualShapeOutputNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Input", shape);
		addInputPin("Name", name);
	}

	// synchronize the asset with the incoming shape
	inline void onExecute() override {
		if (!asset.isNull()) {
			asset->clearShape();
		}

		if (name.size()) {
			asset = "virtual:" + name;

			if (shape.isValid()) {
				asset->setShape(shape);
			}

		} else {
			asset.clear();
		}
	}

	static constexpr const char* nodeName = "Save Shape to Virtual";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::virtualize;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	// properties
	OtShape shape;
	std::string name;
	OtAsset<OtShapeAsset> asset;
};

static OtNodesFactoryRegister<OtVirtualShapeOutputNode> registration;
