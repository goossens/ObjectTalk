//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "glm/ext.hpp"

#include "OtFunction.h"

#include "OtSkybox.h"
#include "OtVertex.h"


static glm::vec3 skyboxVertices[] = {
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },

	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f, -1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },

	{  1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },

	{ -1.0f, -1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f },
	{ -1.0f, -1.0f,  1.0f },

	{ -1.0f,  1.0f, -1.0f },
	{  1.0f,  1.0f, -1.0f },
	{  1.0f,  1.0f,  1.0f },
	{  1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f,  1.0f },
	{ -1.0f,  1.0f, -1.0f },

	{ -1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{  1.0f, -1.0f, -1.0f },
	{  1.0f, -1.0f, -1.0f },
	{ -1.0f, -1.0f,  1.0f },
	{  1.0f, -1.0f,  1.0f }
};

static uint32_t skyboxIndices[] = {
	0, 1, 2,
	3, 4, 5,

	6, 7, 8,
	9, 10, 11,

	12, 13, 14,
	15, 16, 17,

	18, 19, 20,
	21, 22, 23,

	24, 25, 26,
	27, 28, 29,

	30, 31, 32,
	33, 34, 35
};


//
//	OtSkyboxClass::OtSkyboxClass
//

OtSkyboxClass::OtSkyboxClass() {
	vertexBuffer.set(skyboxVertices, sizeof(skyboxVertices), OtVertexPos::getLayout());
	indexBuffer.set(skyboxIndices, sizeof(skyboxIndices));
}


//
//	OtSkyboxClass::init
//

void OtSkyboxClass::init(size_t count, OtObject* parameters) {
	if (count == 6) {
		setCubemap(
			parameters[0]->operator std::string(),
			parameters[1]->operator std::string(),
			parameters[2]->operator std::string(),
			parameters[3]->operator std::string(),
			parameters[4]->operator std::string(),
			parameters[5]->operator std::string());

	} else if (count != 0) {
		OtExcept("[Skybox] constructor expects 0 or 6 arguments (not %ld)", count);
	}
}


//
//	OtSkyboxClass::setCubemap
//

OtObject OtSkyboxClass::setCubemap(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz) {
	cubemap.load(posx, negx, posy, negy, posz, negz);
	return shared();
}


//
//	OtSkyboxClass::render
//

void OtSkyboxClass::render(OtRenderer& renderer) {
	// sanity check
	if (!cubemap.isValid()) {
		OtExcept("[cubemap] missing for [Skybox]");
	}

	// ensure skybox is centered at camera position
	glm::mat4 transform = glm::translate(glm::mat4(1.0), renderer.getCamera()->getPosition());
	renderer.setTransform(transform);

	// submit cubemap
	cubemapSampler.submit(0, cubemap);

	// submit vertices and triangles
	vertexBuffer.submit();
	indexBuffer.submit();

	// run shader
	renderer.setNoDepthState();
	renderer.runShader(shader);
}


//
//	OtSkyboxClass::getMeta
//

OtType OtSkyboxClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtSkyboxClass>("Skybox", OtSceneObjectClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtSkyboxClass::init));
		type->set("setCubemap", OtFunctionClass::create(&OtSkyboxClass::setCubemap));
	}

	return type;
}


//
//	OtSkyboxClass::create
//

OtSkybox OtSkyboxClass::create() {
	OtSkybox skybox = std::make_shared<OtSkyboxClass>();
	skybox->setType(getMeta());
	return skybox;
}
