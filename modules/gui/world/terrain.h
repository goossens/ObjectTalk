//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "bgfx/bgfx.h"

#include "sceneobject.h"


//
//	OtTerrainClass
//

class OtTerrainClass;
typedef std::shared_ptr<OtTerrainClass> OtTerrain;

class OtTerrainClass : public OtSceneObjectClass {
public:
	// constructor/destructor
	OtTerrainClass();
	~OtTerrainClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// render in BGFX
    void render(int view, OtCamera camera, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtTerrain create();

protected:
	// terrain extremes
	float range = 4096.0;
	float minimum = 256.0;
};
