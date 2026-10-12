//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtLog.h"

#include "OtComponentObject.h"
#include "OtParticlesComponent.h"


//
//	Helper macros
//

#define declareParticlesProperty(T, P, N)												\
	inline void set##N(T value) { component().settings.P = value; }						\
	inline T get##N() { return component().settings.P; }

#define declareParticlesAssetProperty(P, N)												\
	inline void set##N(const std::string& value) { component().settings.P = value; }	\
	inline std::string get##N() { return component().settings.P.getPath(); }


//
//	OtParticlesComponentObjectClass
//

class OtParticlesComponentObjectClass;
using OtParticlesComponentObject = OtObjectPointer<OtParticlesComponentObjectClass>;

class OtParticlesComponentObjectClass : public OtComponentObjectClass {
public:
	// access the properties
	declareParticlesProperty(size_t, particles, Particles)
	declareParticlesProperty(float, lifeSpanLow, LifeSpanLow)
	declareParticlesProperty(float, lifeSpanHigh, LifeSpanHigh)
	declareParticlesAssetProperty(atlas, Atlas)
	declareParticlesProperty(int, atlasRows, AtlasRows)
	declareParticlesProperty(int, atlasColumns, AtlasColumns)
	declareParticlesProperty(float, speed, Speed)
	declareParticlesProperty(float, gravity, Gravity)

	inline void setShape(int value) { component().settings.shape = static_cast<OtParticleSettings::Shape>(value); }
	inline int getShape() { return static_cast<int>(component().settings.shape); }

	inline void setRotation(glm::vec2 value) {
		auto& rotation = component().settings.rotation;
		rotation[0] = value.x;
		rotation[1] = value.y;
	}

	inline glm::vec2 getRotation() {
		auto& rotation = component().settings.rotation;
		return glm::vec2(rotation[0], rotation[1]);
	}

	inline void setScale(size_t index, glm::vec2 value) {
		validate(index);
		component().settings.scale[index] = ImVec2(value.x, value.y);
	}

	inline glm::vec2 getScale(size_t index) {
		validate(index);
		auto& scale = component().settings.scale[index];
		return glm::vec2(scale.x, scale.y);
	}

	inline void setAlpha(size_t index, glm::vec2 value) {
		validate(index);
		component().settings.alpha[index] = ImVec2(value.x, value.y);
	}

	inline glm::vec2 getAlpha(size_t index) {
		validate(index);
		auto& alpha = component().settings.scale[index];
		return glm::vec2(alpha.x, alpha.y);
	}

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtParticlesComponentObjectClass>("ParticlesComponent", OtComponentObjectClass::getMeta());
			metaProperty(Particles, Particles)
			metaProperty(Particles, LifeSpanLow)
			metaProperty(Particles, LifeSpanHigh)
			metaProperty(Particles, Atlas)
			metaProperty(Particles, AtlasRows)
			metaProperty(Particles, AtlasColumns)
			metaProperty(Particles, Shape)
			metaProperty(Particles, Speed)
			metaProperty(Particles, Gravity)
			metaProperty(Particles, Rotation)
			metaProperty(Particles, Scale)
			metaProperty(Particles, Alpha)
		}

		return type;
	}

protected:
	// constructor
	friend class OtObjectPointer<OtParticlesComponentObjectClass>;
	OtParticlesComponentObjectClass() = default;
	OtParticlesComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtParticlesComponent& component() { return OtComponentObjectClass::component<OtParticlesComponent>(scene, entity); }

	// support functions
	inline void validate(size_t index) {
		if (index > 5) {
			OtLogError("Invalid index [{}], must be between 0 and 5");
		}
	}
};
