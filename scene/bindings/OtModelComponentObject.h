//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"

#include "OtModelComponent.h"


//
//	OtModelComponentObjectClass
//

class OtModelComponentObjectClass;
using OtModelComponentObject = OtObjectPointer<OtModelComponentObjectClass>;

class OtModelComponentObjectClass : public OtObjectClass {
public:
	// see if model is ready
	inline bool isReady() { return model->model->isReady(); }

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
	OtModelComponentObjectClass(OtModelComponent* component) : model(component) {}

private:
	// connection to component
	OtModelComponent* model;
};
