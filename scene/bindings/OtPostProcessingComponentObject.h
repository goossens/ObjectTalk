//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"

#include "OtComponentObject.h"
#include "OtPostProcessingComponent.h"


//
//	OtPostProcessingComponentClass
//

class OtPostProcessingComponentObjectClass;
using OtPostProcessingComponentObject = OtObjectPointer<OtPostProcessingComponentObjectClass>;

class OtPostProcessingComponentObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declareProperty(bool, fxaa, FXAA)
	declareProperty(bool, fog, Fog)
	declareProperty(float, fogDensity, FogDensity)
	declareProperty(float, backgroundFogRatio, BackgroundFogRatio)
	declareVectorProperty(3, fogColor, FogColor)
	declareProperty(bool, bloom, Bloom)
	declareProperty(float, bloomIntensity, BloomIntensity)
	declareProperty(bool, godrays, GodRays)
	declareProperty(float, exposure, Exposure)
	declareProperty(float, contrast, Contrast)

	inline void setToneMap(int value) { component().tonemap = static_cast<OtPostProcessing::Tonemap>(value); }
	inline int getToneMap() { return static_cast<int>(component().tonemap); }

	// get type definition
	static inline OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtPostProcessingComponentObjectClass>("PostProcessing", OtComponentObjectClass::getMeta());
			metaProperty(PostProcessing, FXAA)
			metaProperty(PostProcessing, Fog)
			metaProperty(PostProcessing, FogDensity)
			metaProperty(PostProcessing, BackgroundFogRatio)
			metaProperty(PostProcessing, FogColor)
			metaProperty(PostProcessing, Bloom)
			metaProperty(PostProcessing, BloomIntensity)
			metaProperty(PostProcessing, GodRays)
			metaProperty(PostProcessing, Exposure)
			metaProperty(PostProcessing, Contrast)
			metaProperty(PostProcessing, ToneMap)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtPostProcessingComponentObjectClass>;
	OtPostProcessingComponentObjectClass() = default;
	OtPostProcessingComponentObjectClass(OtScene* scene) : scene(scene) {}

private:
	// get reference
	inline OtPostProcessingComponent& component() {
		if (!scene) {
			OtLogError("Class [{}] can't be directly instantiated, create it via [Scene] class instead", getTypeName());
		}

		return scene->getPostProcessing();
	}

	OtScene* scene = nullptr;
};
