//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>
#include <cmath>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui.h"

#include "OtCamera.h"
#include "OtMeshPreview.h"


//
//	OtMeshPreview::setSize
//

void OtMeshPreview::setSize(int w, int h) {
	if (w != width || h != height) {
		framebuffer.update(static_cast<int>(w), static_cast<int>(h));
		width = w;
		height = h;
	}
}


//
//	OtMeshPreview::render
//

void OtMeshPreview::render(OtMesh& mesh) {
	// determine mesh information
	auto aabb = mesh.getAABB();
	auto size = aabb.getSize();
	auto center = aabb.getCenter();
	auto scale = 0.8f / std::max(std::max(size.x, size.y), size.z);

	auto model =
		glm::toMat4(glm::quat(glm::radians(glm::vec3(elevation, azimuth, 0.0f)))) *
		glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
		glm::translate(glm::mat4(1.0f), -center);

	// create a camera
	OtCamera camera(
		framebuffer.getWidth(),
		framebuffer.getHeight(),
		0.05f, 3.0f,
		60.0f,
		glm::vec3(0.0f, 0.0f, 1.5f),
		glm::vec3(0.0f));

	// setup the pass
	OtPass pass;
	pass.setClear(true, true);
	pass.setRectangle(0, 0, framebuffer.getWidth(), framebuffer.getHeight());
	pass.setFrameBuffer(framebuffer);
	pass.setTransform(camera.viewMatrix, camera.projectionMatrix);
	pass.touch();

	// set the uniforms
	uniform.setValue(0, 0.0f, 0.0f, 0.8f, 0.0f);
	uniform.setValue(1, 1.0f, 1.0f, 2.0f, 0.0f);
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

	program.setTransform(model);
	pass.runShaderProgram(program);

	// show the mesh
	ImGui::Image((ImTextureID)(intptr_t) framebuffer.getColorTextureIndex(), ImVec2(width, height));

	// process left-mouse drag events (if required)
	if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
		// determine timestep
		float delta = ImGui::GetIO().DeltaTime;

		// handle mouse interactions
		ImVec2 drag = ImGui::GetMouseDragDelta();

		if (drag.x != 0.0f || drag.y != 0.0f) {
			elevation += drag.y * maxRotationPerSecond * delta / 2.0f;
			azimuth += drag.x * maxRotationPerSecond * delta;
			ImGui::ResetMouseDragDelta();
		}
	}

	// ensure all properties are in range
	elevation = std::clamp(elevation, -89.9f, 89.9f);

	if (azimuth < -360.0f) {
		azimuth += 360.0f;
	}

	if (azimuth > 360.0f) {
		azimuth -= 360.0f;
	}
}
