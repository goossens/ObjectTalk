//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtCamera.h"
#include "OtNode.h"
#include "OtRenderer.h"


//
//	OtSceneObject
//

class OtSceneObjectClass;
typedef std::shared_ptr<OtSceneObjectClass> OtSceneObject;

class OtSceneObjectClass : public OtNodeClass {
public:
	// access shadow flags
	virtual OtObject castShadow(bool flag) { castShadowFlag = flag; return shared(); }
	bool castsShadow() { return castShadowFlag; }
	virtual OtObject receiveShadow(bool flag) { receiveShadowFlag = flag; return shared(); }
	bool receivesShadow() { return receiveShadowFlag; }

	// pre-render any content that is required for the rendering phase (e.g shadows and reflections)
	virtual void preRender(OtRenderer& renderer) {}

	// submit to GPU
	virtual void render(OtRenderer& renderer) {}

	// get type definition
	static OtType getMeta();

protected:
	// shadow flags
	bool castShadowFlag = false;
	bool receiveShadowFlag = false;
};
