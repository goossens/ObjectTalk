//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtArray.h"
#include "OtString.h"

#include "OtComponentObject.h"
#include "OtNodesComponent.h"


//
//	OtNodesComponentObjectClass
//

class OtNodesComponentObjectClass;
using OtNodesComponentObject = OtObjectPointer<OtNodesComponentObjectClass>;

class OtNodesComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareAssetProperty(asset, Asset)

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtNodesComponentObjectClass>("NodesComponent", OtComponentObjectClass::getMeta());
			metaProperty(Nodes, Asset)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtNodesComponentObjectClass>;
	OtNodesComponentObjectClass() = default;
	OtNodesComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtNodesComponent& component() { return OtComponentObjectClass::component<OtNodesComponent>(scene, entity); }
};
