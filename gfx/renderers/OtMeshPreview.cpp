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
#include "OtPass.h"


//
//	OtMeshPreview::render
//

void OtMeshPreview::render(int width, int height, OtMesh& mesh, Context& context) {
	// update framebuffer
	framebuffer.update(width, height);

	// determine mesh information
	auto aabb = mesh.getAABB();
	auto size = aabb.getSize();
	auto center = aabb.getCenter();
	auto scale = context.zoom / std::max(std::max(size.x, size.y), size.z);

	auto model =
		glm::toMat4(glm::quat(glm::radians(glm::vec3(context.elevation, context.azimuth, 0.0f)))) *
		glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
		glm::translate(glm::mat4(1.0f), -center);

	// create a camera
	OtCamera camera(
		width,
		height,
		0.05f, 3.0f,
		60.0f,
		glm::vec3(0.0f, 0.0f, 1.5f),
		glm::vec3(0.0f));

	// setup the pass
	OtPass pass;
	pass.setClear(true, true);
	pass.setRectangle(0, 0, width, height);
	pass.setFrameBuffer(framebuffer);
	pass.setViewTransform(camera.viewMatrix, camera.projectionMatrix);
	pass.touch();

	// set the uniforms
	uniform.setValue(0, 0.0f, 0.0f, 0.8f, 0.0f);
	uniform.setValue(1, 1.0f, 1.0f, 2.0f, 0.0f);
	uniform.setValue(2, context.meshColor, 0.0f);
	uniform.setValue(3, context.lightColor, 0.0f);
	uniform.submit();

	// submit the mesh
	if (context.wireframe) {
		mesh.submitLines();

	} else {
		mesh.submitTriangles();
	}

	// render the mesh
	if (context.wireframe) {
		pass.setState(
			OtPass::stateWriteRgb |
			OtPass::stateWriteZ |
			OtPass::stateDepthTestLess |
			OtPass::stateLines);

	} else {
		pass.setState(
			OtPass::stateWriteRgb |
			OtPass::stateWriteZ |
			OtPass::stateDepthTestLess |
			OtPass::stateCullCw);
	}

	pass.setModelTransform(model);
	pass.runShaderProgram(program);

	// show the mesh
	ImGui::Image(framebuffer.getColorTextureID(), ImVec2(static_cast<float>(width), static_cast<float>(height)));

	if (ImGui::IsItemHovered()) {
		// process left-mouse drag events (if required)
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			// determine timestep
			float delta = ImGui::GetIO().DeltaTime;

			// handle mouse interactions
			ImVec2 drag = ImGui::GetMouseDragDelta();

			if (drag.x != 0.0f || drag.y != 0.0f) {
				if (ImGui::IsKeyDown(ImGuiMod_Alt)) {
					context.zoom += drag.y * maxZoomPerSecond * delta;

				} else {
					context.elevation += drag.y * maxRotationPerSecond * delta / 2.0f;
					context.azimuth += drag.x * maxRotationPerSecond * delta;
				}

				ImGui::ResetMouseDragDelta();
			}
		}
	}

	// ensure all properties are in range
	context.elevation = std::clamp(context.elevation, -89.9f, 89.9f);

	if (context.azimuth < -360.0f) {
		context.azimuth += 360.0f;
	}

	if (context.azimuth > 360.0f) {
		context.azimuth -= 360.0f;
	}

	context.zoom = std::clamp(context.zoom, 0.5f, 2.0f);
}
