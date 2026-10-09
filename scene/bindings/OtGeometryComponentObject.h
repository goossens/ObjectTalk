//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtGeometryComponent.h"
#include "OtComponentObject.h"


//
//	OtGeometryComponentObjectClass
//

class OtGeometryComponentObjectClass;
using OtGeometryComponentObject = OtObjectPointer<OtGeometryComponentObjectClass>;

class OtGeometryComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareAssetProperty(asset, Asset)
	declareProperty(bool, transparent, Transparent)
	declareProperty(bool, cullBack, CullBack)
	declareProperty(bool, castShadow, CastShadow)
	declareProperty(bool, wireframe, Wireframe)

	// get type definition
	static inline OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtGeometryComponentObjectClass>("GeometryComponent", OtComponentObjectClass::getMeta());
			metaProperty(Geometry, Asset)
			metaProperty(Geometry, Transparent)
			metaProperty(Geometry, CullBack)
			metaProperty(Geometry, CastShadow)
			metaProperty(Geometry, Wireframe)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtGeometryComponentObjectClass>;
	OtGeometryComponentObjectClass() = default;
	OtGeometryComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtGeometryComponent& component() { return OtComponentObjectClass::component<OtGeometryComponent>(scene, entity); }
};
