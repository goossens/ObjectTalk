//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "glm/glm.hpp"

#include "OtGui.h"
#include "OtRenderer.h"


//
//	OtMaterial
//

class OtMaterialClass;
using OtMaterial = OtObjectPointer<OtMaterialClass>;

class OtMaterialClass : public OtGuiClass {
public:
	// determines which side of faces will be rendered
	OtObject setFrontSide() { frontside = true; backside = false; return OtObject(this); }
	OtObject setBackSide() { frontside = false; backside = true; return OtObject(this); }
	OtObject setDoubleSided() { frontside = true; backside = true; return OtObject(this); }
	bool isFrontSided() { return frontside; }
	bool isBackSided() { return backside; }
	bool isDoubleSided() { return frontside && backside; }

	// access transparency
	OtObject setTransparent() { transparent = true; return OtObject(this); }
	OtObject setOpaque() { transparent = false; return OtObject(this); }
	bool isTransparent() { return transparent; }
	bool isOpaque() { return !transparent; }

	// submit to GPU
	virtual void submit(OtRenderer& renderer, bool wireframe, bool instancing) {}

	// get type definition
	static OtType getMeta();

protected:
	// properties
	bool frontside = true;
	bool backside = false;
	bool transparent = false;
};
