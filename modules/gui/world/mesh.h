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

	// initialize mesh
	OtObject init(size_t count, OtObject* parameters);

	// update geometry or material
	void setGeometry(OtObject geometry);
	void setMaterial(OtObject material);

	// render in BGFX
    void render(int view, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtMesh create();

protected:
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
