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
	inline bool hasAnimation(const std::string& name) { return model->model->getModel().hasAnimation(name); }
	inline size_t getAnimationCount() { return model->model->getModel().getAnimationCount(); }
	inline std::string getAnimationName(size_t index) { return model->model->getModel().getAnimationName(index); }
	OtObject getAnimations();

	// set animations
	inline void setAnimation(const std::string& name) { model->model->getModel().setAnimation(name); }
	inline void fadeToAnimation(const std::string& name, float seconds) { model->model->getModel().fadeToAnimation(name, seconds); }

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
