//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <glm/gtx/quaternion.hpp>
#include "glm/gtx/matrix_decompose.hpp"

#include "OtSceneRenderer.h"


//
//	OtSceneRenderer::render
//

int OtSceneRenderer::render(OtScene2 scene, OtCamera2 c, int w, int h) {
	// save information
	camera = c;
	width = w;
	height = h;

	// update buffers
	composite.update(width, height);

	// run composite pass
	renderEnvironmentPass(scene);
	renderCompositePass(scene);

	// render geometry pass
	renderGeometryPass(scene);

	return composite.getColorTextureIndex();
}


//
//	OtSceneRenderer::renderGeometryPass
//

void OtSceneRenderer::renderGeometryPass(OtScene2 scene) {
	// setup pass
	geometryPass.reserveRenderingSlot();
//	geometryPass.setClear(true, false);
	geometryPass.setRectangle(0, 0, width, height);
	geometryPass.setFrameBuffer(composite);
	geometryPass.setTransform(camera->getViewMatrix(), camera->getProjectionMatrix());

	// render all geometries
	for (auto entity : scene->view<OtGeometryComponent>()) {
		if (scene->hasComponent<OtTransformComponent>(entity) && scene->hasComponent<OtMaterialComponent>(entity)) {
			renderGeometry(scene, entity);
		}
	}
}


//
//	OtSceneRenderer::renderEnvironmentPass
//

void OtSceneRenderer::renderEnvironmentPass(OtScene2 scene) {
	// get the camera's view matrix and decompose it
	auto viewMatrix = camera->getViewMatrix();
	glm::vec3 scale;
	glm::quat rotate;
	glm::vec3 translate;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(viewMatrix, scale, rotate, translate, skew, perspective);

	// create a new matrix that only honors the rotation
	glm::mat4 newViewMatrix = glm::toMat4(rotate);

	// setup pass
	environmentPass.reserveRenderingSlot();
	environmentPass.setClear(true, false);
	environmentPass.setRectangle(0, 0, width, height);
	environmentPass.setFrameBuffer(composite);
	environmentPass.setTransform(newViewMatrix, camera->getProjectionMatrix());

	// we can only have one environment component
	bool environmentComponent = false;

	// see if we have any sky boxes
	scene->view<OtSkyBoxComponent>().each([&](auto& component) {
		if (!environmentComponent) {
			if (component.isValid()) {
				renderSkyBox(component);
				environmentComponent = true;
			}
		}
	});

	// see if we have any sky spheres
	scene->view<OtSkySphereComponent>().each([&](auto& component) {
		if (!environmentComponent) {
			if (component.isValid()) {
				renderSkySphere(component);
				environmentComponent = true;
			}
		}
	});
}


//
//	OtSceneRenderer::renderCompositePass
//

void OtSceneRenderer::renderCompositePass(OtScene2 scene) {
}


//
//	OtSceneRenderer::renderSkyBox
//

void OtSceneRenderer::renderSkyBox(OtSkyBoxComponent& component) {
	// setup the mesh
	if (!unityBoxGeometry) {
		unityBoxGeometry = OtBoxGeometryClass::create();
	}

	unityBoxGeometry->submitTriangles();

	// submit texture via sampler
	skyMapSampler.submit(0, component.cubemap, "s_cubemap");

	// load the shader (if required)
	if (!skyBoxShader.isValid()) {
		skyBoxShader.initialize("OtSkyboxVS", "OtSkyboxFS");
	}

	// run the shader
	skyBoxShader.setState(OtShader::noDepth);
	environmentPass.runShader(skyBoxShader);
}


//
//	OtSceneRenderer::renderSkySphere
//

void OtSceneRenderer::renderSkySphere(OtSkySphereComponent& component) {
	// setup the mesh
	if (!unitySphereGeometry) {
		unitySphereGeometry = OtSphereGeometryClass::create();
	}

	unitySphereGeometry->submitTriangles();

	// submit texture via sampler
	skySphereSampler.submit(0, component.texture, "s_skySphereTexture");

	// load the shader (if required)
	if (!skySphereShader.isValid()) {
		skySphereShader.initialize("OtSkySphereVS", "OtSkySphereFS");
	}

	// run the shader
	skySphereShader.setState(OtShader::noDepth);
	environmentPass.runShader(skySphereShader);
}


//
//	OtSceneRenderer::renderGeometry
//

void OtSceneRenderer::renderGeometry(OtScene2 scene, OtEntity entity) {
	// get all relevant components
	auto& geometry = scene->getComponent<OtGeometryComponent>(entity);
	auto& transform = scene->getComponent<OtTransformComponent>(entity);
	auto& material = scene->getComponent<OtMaterialComponent>(entity);

	// submit the geometry
	if (geometry.wireframe) {
		geometry.geometry->submitLines();

	} else {
		geometry.geometry->submitTriangles();
	}

	// create the material uniforms (if required)
	if (!materialUniforms.isValid()) {
		materialUniforms.initialize("u_material", 3);
	}

	// set the material uniform values and submit to GPU
	glm::vec4* uniforms = materialUniforms.getValues();
	uniforms[0] = material.albedo;

	uniforms[1] = glm::vec4(
		material.metalic,
		material.roughness,
		material.ao,
		material.albedoTexture.isValid());

	uniforms[2] = glm::vec4(
		material.metalicTexture.isValid(),
		material.roughnessTexture.isValid(),
		material.aoTexture.isValid(),
		material.normalTexture.isValid());

	materialUniforms.submit();

	// submit all textures (or dummies if they are not set)
	albedoSampler.submit(0, material.albedoTexture, "s_albedoTexture");
	albedoSampler.submit(1, material.normalTexture, "s_normalTexture");
	albedoSampler.submit(2, material.metalicTexture, "s_metalicTexture");
	albedoSampler.submit(3, material.roughnessTexture, "s_roughnessTexture");
	albedoSampler.submit(4, material.aoTexture, "s_aoTexture");

	// load the shader (if required)
	if (!geometryShader.isValid()) {
		geometryShader.initialize("OtGeometryVS", "OtGeometryFS");
	}

	// set the shader state
	if (geometry.wireframe) {
		geometryShader.setState(OtShader::wireframe);

	} else if (geometry.cullback) {
		geometryShader.setState(OtShader::cullBack);

	} else {
		geometryShader.setState(OtShader::noCulling);
	}

	// set the model transform
	auto matrix = transform.getTransform();
	geometryShader.setTransform(matrix);

	// run the shader
	geometryPass.runShader(geometryShader);
}
