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
//	OtSceneClass
//

class OtSceneClass;
typedef std::shared_ptr<OtSceneClass> OtScene;

class OtSceneClass : public OtNodeClass {
public:
	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// pre-render any content that is required for the rendering phase (e.g shadows and reflections)
	void preRender(OtRenderer& renderer);

	// render contents
	void render(OtRenderer& renderer);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtScene create();
};
