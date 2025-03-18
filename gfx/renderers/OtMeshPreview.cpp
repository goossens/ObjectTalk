//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "OtCamera.h"
#include "OtMeshPreview.h"


//
//	OtMeshPreview::render
//

void OtMeshPreview::render(OtFrameBuffer& framebuffer, OtMesh& mesh) {
	// determine mesh information
	auto aabb = mesh.getAABB();
	auto center = aabb.getCenter();
	auto radius = aabb.getRadius();

	// determine camera information
	float elevationRad = glm::radians(elevation);
	float azimuthRad = glm::radians(azimuth);

	auto cameraDirection = glm::normalize(glm::vec3(
		std::cos(elevationRad) * std::sin(azimuthRad),
		std::sin(elevationRad),
		std::cos(elevationRad) * -std::cos(azimuthRad)));

	auto cameraDistance = (radius * 1.1f) / std::tan(glm::radians(30.0f));
	auto cameraPosition = center * cameraDistance * cameraDirection;

	// create a camera
	OtCamera camera(
		framebuffer.getWidth(),
		framebuffer.getHeight(),
		0.05f * cameraDistance, cameraDistance * 2.0f,
		60.0f,
		cameraPosition,
		center);

	// setup the pass
	OtPass pass;
	pass.setRectangle(0, 0, framebuffer.getWidth(), framebuffer.getHeight());
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(camera.viewMatrix, camera.projectionMatrix);
	pass.touch();

	// set the uniforms
	uniform.setValue(0, cameraPosition, 0.0f);
	uniform.setValue(1, cameraPosition, 0.0f);
	uniform.setValue(2, meshColor, 0.0f);
	uniform.setValue(3, lightColor, 0.0f);
	uniform.submit();

	// submit the mesh
	if (wireframe) {
		mesh.submitLines();

	} else {
		mesh.submitTriangles();
	}

	// render the mesh
	if (wireframe) {
		program.setState(
			OtStateWriteRgb |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateLines);

	} else {
		program.setState(
			OtStateWriteRgb |
			OtStateWriteZ |
			OtStateDepthTestLess |
			OtStateCullCw);
	}

	pass.runShaderProgram(program);
}
