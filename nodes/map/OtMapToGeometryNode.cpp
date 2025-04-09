//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "OtGeometry.h"
#include "OtMap.h"

#include "OtNodesFactory.h"


//
//	OtMapToGeometryNode
//

class OtMapToGeometryNode : public OtNodeClass {
public:
	// configure node
	inline void configure() override {
		addInputPin("Map", map);
		addInputPin("Scale", scale);
		addOutputPin("Geometry", geometry);
	}

	// generate the geometry
	inline void onExecute() override {
		if (map.isValid()) {
			geometry = map.createGeometry(scale);

		} else {
			geometry.clear();
		}
	}

	static constexpr const char* nodeName = "Map to Geometry";
	static constexpr OtNodeClass::Category nodeCategory = OtNodeClass::Category::map;
	static constexpr OtNodeClass::Kind nodeKind = OtNodeClass::Kind::fixed;

private:
	OtMap map;
	float scale = 10.0f;
	OtGeometry geometry;
};


static OtNodesFactoryRegister<OtMapToGeometryNode> registration;
