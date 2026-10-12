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
#include "OtInstancingComponent.h"


//
//	OtInstancingComponentObjectClass
//

class OtInstancingComponentObjectClass;
using OtInstancingComponentObject = OtObjectPointer<OtInstancingComponentObjectClass>;

class OtInstancingComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareAssetProperty(asset, Asset)

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtInstancingComponentObjectClass>("InstancingComponent", OtComponentObjectClass::getMeta());
			metaProperty(Instancing, Asset)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtInstancingComponentObjectClass>;
	OtInstancingComponentObjectClass() = default;
	OtInstancingComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtInstancingComponent& component() { return OtComponentObjectClass::component<OtInstancingComponent>(scene, entity); }
};
