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

#include "OtMathModule.h" // IWYU pragma: keep

#include "OtScene.h"
#include "OtPostProcessingComponent.h"


//
//	Helper macros
//

#define declarePpProperty(T, P, N)													\
	inline void set##N(T value) { component().P = value; }							\
	inline T get##N() { return component().P; }

#define metaPpProperty(N)															\
	type->set("set"#N"", OtFunction::create(&OtPostProcessingObjectClass::set##N));	\
	type->set("get"#N"", OtFunction::create(&OtPostProcessingObjectClass::get##N));


//
//	OtPostProcessingObjectClass
//

class OtPostProcessingObjectClass;
using OtPostProcessingObject = OtObjectPointer<OtPostProcessingObjectClass>;

class OtPostProcessingObjectClass : public OtComponentObjectClass {
public:
	// access properties
	declarePpProperty(bool, fxaa, FXAA)
	declarePpProperty(bool, fog, Fog)
	declarePpProperty(float, fogDensity, FogDensity)
	declarePpProperty(float, backgroundFogRatio, BackgroundFogRatio)
	declarePpProperty(glm::vec3, fogColor, FogColor)
	declarePpProperty(bool, bloom, Bloom)
	declarePpProperty(float, bloomIntensity, BloomIntensity)
	declarePpProperty(bool, godrays, GodRays)
	declarePpProperty(float, exposure, Exposure)
	declarePpProperty(float, contrast, Contrast)

	inline void setToneMap(int value) { component().tonemap = static_cast<OtPostProcessing::Tonemap>(value); }
	inline int getToneMap() { return static_cast<int>(component().tonemap); }

	// get type definition
	static inline OtType getMeta()  {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtPostProcessingObjectClass>("PostProcessing", OtObjectClass::getMeta());
			metaPpProperty(FXAA)
			metaPpProperty(Fog)
			metaPpProperty(FogDensity)
			metaPpProperty(BackgroundFogRatio)
			metaPpProperty(FogColor)
			metaPpProperty(Bloom)
			metaPpProperty(BloomIntensity)
			metaPpProperty(GodRays)
			metaPpProperty(Exposure)
			metaPpProperty(Contrast)

			type->set("setToneMap", OtFunction::create(&OtPostProcessingObjectClass::setToneMap));
			type->set("getToneMap", OtFunction::create(&OtPostProcessingObjectClass::getToneMap));
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtPostProcessingObjectClass>;
	OtPostProcessingObjectClass() = default;
	OtPostProcessingObjectClass(OtScene* scene) : scene(scene) {}

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
