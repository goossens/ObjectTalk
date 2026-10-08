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
	inline bool hasAnimation(const std::string& name) {
		if (component().asset->isReady()) {
			return component().asset->getModel().hasAnimation(name);

		} else {
			return false;
		}
	}

	inline size_t getAnimationCount() {
		if (component().asset->isReady()) {
			return component().asset->getModel().getAnimationCount();

		} else {
			return 0;
		}
	}

	inline std::string getAnimationName(size_t index) {
		if (component().asset->isReady()) {
			if (index < component().asset->getModel().getAnimationCount()) {
				return component().asset->getModel().getAnimationName(index);

			} else {
				OtLogError("Invalid animation index");
			}

		} else {
			OtLogError("Can't access model that is not ready");
		}

		return "";
	}

	inline OtObject getAnimations() {
		OtArray result = OtArray::create();

		if (component().asset->isReady()) {
			auto models = component().asset->getModel().getAnimationCount();

			for (size_t i = 0; i < models; i++) {
				result->append(OtString::create(component().asset->getModel().getAnimationName(i)));
			}
		}

		return result;
	}

	inline void startAnimation(const std::string& name) {
		if (component().asset->isReady()) {
			component().asset->getModel().startAnimation(name);

		} else {
			OtLogError("Can't animate model that is not ready");
		}
	}

	inline void stopAnimation() {
		if (component().asset->isReady()) {
			component().asset->getModel().stopAnimation();
		}
	}

	inline void fadeToAnimation(const std::string& name, float duration) {
		if (component().asset->isReady()) {
			component().asset->getModel().fadeToAnimation(name, duration);

		} else {
			OtLogError("Can't animate model that is not ready");
		}
	}

	inline bool isAnimating() {
		if (component().asset->isReady()) {
			return component().asset->getModel().isAnimating();

		} else {
			return false;
		}
	}

	inline void setAnimationSpeed(const std::string& name, float speed) {
		if (component().asset->isReady()) {
			component().asset->getModel().setAnimationSpeed(name, speed);
		}
	}

	inline float getAnimationSpeed() {
		if (component().asset->isReady()) {
			return component().asset->getModel().getAnimationSpeed();

		} else {
			return 0.0f;
		}
	}

	// get type definition
	static inline OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtType::create<OtModelComponentObjectClass>("ModelComponent", OtComponentObjectClass::getMeta());

			type->set("isReady", OtFunction::create(&OtModelComponentObjectClass::isReady));

			type->set("hasAnimation", OtFunction::create(&OtModelComponentObjectClass::hasAnimation));
			type->set("getAnimationCount", OtFunction::create(&OtModelComponentObjectClass::getAnimationCount));
			type->set("getAnimationName", OtFunction::create(&OtModelComponentObjectClass::getAnimationName));
			type->set("getAnimations", OtFunction::create(&OtModelComponentObjectClass::getAnimations));

			type->set("startAnimation", OtFunction::create(&OtModelComponentObjectClass::startAnimation));
			type->set("stopAnimation", OtFunction::create(&OtModelComponentObjectClass::stopAnimation));
			type->set("fadeToAnimation", OtFunction::create(&OtModelComponentObjectClass::fadeToAnimation));
			type->set("isAnimating", OtFunction::create(&OtModelComponentObjectClass::isAnimating));

			type->set("setAnimationSpeed", OtFunction::create(&OtModelComponentObjectClass::setAnimationSpeed));
			type->set("getAnimationSpeed", OtFunction::create(&OtModelComponentObjectClass::getAnimationSpeed));
		}

		return type;
	}

protected:
	// return class name
	inline const char* getClassName() override { return "ModelComponent"; }

	// constructor
	friend class OtObjectPointer<OtModelComponentObjectClass>;
	OtModelComponentObjectClass() = default;
	OtModelComponentObjectClass(OtScene* scene, OtEntity entity) { setEcsReferences(scene, entity); }

private:
	// get component reference
	inline OtModelComponent& component() { return OtComponentObjectClass::component<OtModelComponent>(scene, entity); }
};
