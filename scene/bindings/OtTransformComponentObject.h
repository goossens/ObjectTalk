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

#include "OtTransformComponent.h"


//
//	OtTransformComponentObjectClass
//

class OtTransformComponentObjectClass;
using OtTransformComponentObject = OtObjectPointer<OtTransformComponentObjectClass>;

class OtTransformComponentObjectClass : public OtObjectClass {
public:
	// access transform parts
	inline void setTranslation(glm::vec3 translation) { transform->translation = translation; }
	inline void setRotation(glm::vec3 rotation) { transform->rotation = rotation; }
	inline void setScale(glm::vec3 scale) { transform->scale = scale; }

	inline glm::vec3 getTranslation() { return transform->translation; }
	inline glm::vec3 getRotation() { return transform->rotation; }
	inline glm::vec3 getScale() { return transform->scale; }

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtTransformComponentObjectClass>;
	OtTransformComponentObjectClass() = default;
	OtTransformComponentObjectClass(OtTransformComponent* component) : transform(component) {}

private:
	// connection to component
	OtTransformComponent* transform;
};
