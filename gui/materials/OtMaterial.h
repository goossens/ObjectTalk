//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
typedef std::shared_ptr<OtMaterialClass> OtMaterial;

class OtMaterialClass : public OtGuiClass {
public:
	// determines which side of faces will be rendered
	OtObject setFrontSide() { frontside = true; backside = false; return shared(); }
	OtObject setBackSide() { frontside = false; backside = true; return shared(); }
	OtObject setDoubleSided() { frontside = true; backside = true; return shared(); }
	bool isFrontSided() { return frontside; }
	bool isBackSided() { return backside; }
	bool isDoubleSided() { return frontside && backside; }

	// access transparency
	OtObject setTransparent() { transparent = true; return shared(); }
	OtObject setOpaque() { transparent = false; return shared(); }
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
