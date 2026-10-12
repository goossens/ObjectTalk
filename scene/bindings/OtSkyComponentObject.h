//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtSkyComponent.h"
#include "OtComponentObject.h"


//
//	OtSkyComponentObjectClass
//

class OtSkyComponentObjectClass;
using OtSkyComponentObject = OtObjectPointer<OtSkyComponentObjectClass>;

class OtSkyComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareProperty(float, elevation, Elevation)
	declareProperty(float, azimuth, Azimuth)

	declareProperty(bool, castShadow, CastShadow)

	declareProperty(float, rayleighCoefficient, RayleighCoefficient)
	declareProperty(float, mieCoefficient, MieCoefficient)
	declareProperty(float, mieScattering, MieScattering)

	declareProperty(float, cirrus, Cirrus)
	declareProperty(float, cumulus, Cumulus)
	declareProperty(float, speed, Speed)

	// get type definition
	static inline OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtSkyComponentObjectClass>("SkyComponent", OtComponentObjectClass::getMeta());
			metaProperty(Sky, Elevation)
			metaProperty(Sky, Azimuth)

			metaProperty(Sky, CastShadow)

			metaProperty(Sky, RayleighCoefficient)
			metaProperty(Sky, MieCoefficient)
			metaProperty(Sky, MieScattering)

			metaProperty(Sky, Cirrus)
			metaProperty(Sky, Cumulus)
			metaProperty(Sky, Speed)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtSkyComponentObjectClass>;
	OtSkyComponentObjectClass() = default;
	OtSkyComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtSkyComponent& component() { return OtComponentObjectClass::component<OtSkyComponent>(scene, entity); }
};
