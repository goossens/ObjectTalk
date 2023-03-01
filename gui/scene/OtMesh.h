//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "OtGeometry.h"
#include "OtMaterial.h"
#include "OtObject3d.h"


//
//	OtMeshClass
//

class OtMeshClass;
using OtMesh = OtObjectPointer<OtMeshClass>;

class OtMeshClass : public OtObject3dClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// set properties
	OtObject setGeometry(OtObject geometry);
	OtObject setMaterial(OtObject material);
	OtObject setWireframe(bool wireframe);

	// add a new instance to this mesh
	OtObject addInstance(OtObject matrix);

	// submit to GPU
	void render(OtRenderer& renderer) override;

	// get type definition
	static OtType getMeta();

protected:
	// properties
	OtGeometry geometry;
	OtMaterial material;
	bool wireframe = false;

	// list of instances
	std::vector<glm::mat4> instances;
};
