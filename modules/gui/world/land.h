//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "bgfx/bgfx.h"

#include "controller.h"
#include "heightmap.h"
#include "object3d.h"
#include "texture.h"


//
//	OtLandClass
//

class OtLandClass;
typedef std::shared_ptr<OtLandClass> OtLand;

class OtLandClass : public OtObject3dClass {
public:
	// constructor/destructor
	OtLandClass();
	~OtLandClass();

	// initialize
	OtObject init(size_t count, OtObject* parameters);

	// specify a size
	OtObject setSize(float size);

	// specify level of detail
	OtObject setLevelOfDetail(int lod);

	// specify a heightmap
	OtObject setHeightMap(OtObject object);

	// specify region parameters
	OtObject setRegion1Limits(float min, float max) { region1Min = min; region1Max = max; return shared(); }
	OtObject setRegion1Color(float r, float g, float b) { region1Color = glm::vec3(r, g, b); return shared(); }
	OtObject setRegion1Texture(OtObject object);

	OtObject setRegion2Limits(float min, float max) { region2Min = min; region2Max = max; return shared(); }
	OtObject setRegion2Color(float r, float g, float b) { region2Color = glm::vec3(r, g, b); return shared(); }
	OtObject setRegion2Texture(OtObject object);

	OtObject setRegion3Limits(float min, float max) { region3Min = min; region3Max = max; return shared(); }
	OtObject setRegion3Color(float r, float g, float b) { region3Color = glm::vec3(r, g, b); return shared(); }
	OtObject setRegion3Texture(OtObject object);

	OtObject setRegion4Limits(float min, float max) { region4Min = min; region4Max = max; return shared(); }
	OtObject setRegion4Color(float r, float g, float b) { region4Color = glm::vec3(r, g, b); return shared(); }
	OtObject setRegion4Texture(OtObject object);

	// GUI to change properties
	void renderGUI();

	// render in BGFX
    void render(int view, OtCamera camera, glm::mat4 parentTransform);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtLand create();

protected:
	// generate mesh
	void generate();

	// properties
	float size = 256.0;
	int levelOfDetail = 6;
	bool dirty = true;

	OtHeightMap heightmap;

	// land vertex
	struct OtLandVertex {
		// elements of a vertex
		glm::vec3 position;
		glm::vec3 normal;

		// constructors
		OtLandVertex() {}
		OtLandVertex(glm::vec3 p, glm::vec3 n) : position(p), normal(n) {}
	};

	// land shape
	std::vector<OtLandVertex> vertices;
	std::vector<uint32_t> triangles;

	// BGFX buffers
	bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
	bgfx::IndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;

	// dummy texture
	OtTexture dummyTexture;

	// region information
	float region1Min = 0.0;
	float region1Max = 5.0;
	glm::vec3 region1Color = glm::vec3(0.965, 0.894, 0.678);
	OtTexture textureRegion1;

	float region2Min = 5.0;
	float region2Max = 20.0;
	glm::vec3 region2Color = glm::vec3(0.494, 0.784, 0.314);
	OtTexture textureRegion2;

	float region3Min = 20.0;
	float region3Max = 50.0;
	glm::vec3 region3Color = glm::vec3(0.584, 0.553, 0.522);
	OtTexture textureRegion3;

	float region4Min = 50.0;
	float region4Max = 200.0;
	glm::vec3 region4Color = glm::vec3(0.90, 0.90, 0.90);
	OtTexture textureRegion4;

	// uniforms
	bgfx::UniformHandle materialUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle landUniform = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform1 = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform2 = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform3 = BGFX_INVALID_HANDLE;
	bgfx::UniformHandle textureUniform4 = BGFX_INVALID_HANDLE;

	// BGFX shader
	bgfx::ProgramHandle shader = BGFX_INVALID_HANDLE;
};


//
//	Controller widget
//

OT_CONTROLLER(Land)
