//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtComponentObject.h"
#include "OtModelComponent.h"


//
//	OtModelComponentObjectClass
//

class OtModelComponentObjectClass;
using OtModelComponentObject = OtObjectPointer<OtModelComponentObjectClass>;

class OtModelComponentObjectClass : public OtComponentObjectClass {
public:
	// see if model is ready
	inline bool isReady() { return component().asset->isReady(); }

	// access animations
	bool hasAnimation(const std::string& name);
	size_t getAnimationCount();
	std::string getAnimationName(size_t index);
	OtObject getAnimations();

	// access animations
	void startAnimation(const std::string& name);
	void stopAnimation();
	void fadeToAnimation(const std::string& name, float duration);
	bool isAnimating();
	void setAnimationSpeed(const std::string& name, float speed);
	float getAnimationSpeed();

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtModelComponentObjectClass>;
	OtModelComponentObjectClass() = default;
	OtModelComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtModelComponent& component() { return OtComponentObjectClass::component<OtModelComponent>(scene, entity); }
};
