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

#include "object3d.h"
#include "geometry.h"
#include "material.h"


//
//	OtMeshClass
//

class OtMeshClass;
typedef std::shared_ptr<OtMeshClass> OtMesh;

class OtMeshClass : public OtObject3dClass {
public:
	// constructor/destructor
	OtMeshClass();
	~OtMeshClass();

	// set properties
	OtObject setGeometry(OtObject geometry);
	OtObject setMaterial(OtObject material);
	OtObject setHoles(bool holes);

	// render in BGFX
    void render(int view, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMesh create();

protected:
	// render with culling
	void render(int view, glm::mat4 parentTransform, long flag);

	// geometry and material
	OtGeometry geometry;
	OtMaterial material;
	bool holes = false;

	// BGFX buffers
	bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;

	// BGFX shader
	bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
};
