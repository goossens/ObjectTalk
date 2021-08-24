//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "bgfx/bgfx.h"
#include <imgui.h>

#include "ot/function.h"

#include "theme.h"
#include "camera.h"


//
//	OtCameraClass::setPosition
//

OtObject OtCameraClass::setPosition(double x, double y, double z) {
	cameraPos = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setDirection
//

OtObject OtCameraClass::setDirection(double x, double y, double z) {
	cameraDir = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setUp
//

OtObject OtCameraClass::setUp(double x, double y, double z) {
	cameraUp = glm::vec3(x, y, z);
	return shared();
}


//
//	OtCameraClass::setFOV
//

OtObject OtCameraClass::setFOV(double f) {
	fov = f;
	return shared();
}


//
//	OtCameraClass::setClipping
//

OtObject OtCameraClass::setClipping(double near, double far) {
	nearClip = near;
	farClip = far;
	return shared();
}


//
//	OtCameraClass::renderGUI
//

void OtCameraClass::renderGUI() {
	ImGui::SliderFloat3("Position", glm::value_ptr(cameraPos), -50.0f, 50.0f);
	ImGui::SliderFloat3("Target", glm::value_ptr(cameraDir), -50.0f, 50.0f);
	ImGui::SliderFloat3("Up", glm::value_ptr(cameraUp), -2.0f, 2.0f);
    ImGui::SliderFloat("FoV (Deg)", &fov, 10.0f, 120.0f);
}


//
//	OtCameraClass::submit
//

void OtCameraClass::submit(int view, float viewAspect) {
	// determine view and projection transformations
	viewMatrix = glm::lookAt(cameraPos, cameraDir, cameraUp);

	projMatrix = glm::perspective(
		(float) glm::radians(fov),
		(float) viewAspect,
		(float) nearClip,
		(float) farClip);

	// setup BGFX
	bgfx::setViewTransform(view, glm::value_ptr(viewMatrix), glm::value_ptr(projMatrix));
}


//
//	OtCameraClass::getMeta
//

OtType OtCameraClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtCameraClass>("Camera", OtGuiClass::getMeta());
		type->set("setPosition", OtFunctionClass::create(&OtCameraClass::setPosition));
		type->set("setDirection", OtFunctionClass::create(&OtCameraClass::setDirection));
		type->set("setUp", OtFunctionClass::create(&OtCameraClass::setUp));
		type->set("setFOV", OtFunctionClass::create(&OtCameraClass::setFOV));
		type->set("setClipping", OtFunctionClass::create(&OtCameraClass::setClipping));
	}

	return type;
}


//
//	OtCameraClass::create
//

OtCamera OtCameraClass::create() {
	OtCamera camera = std::make_shared<OtCameraClass>();
	camera->setType(getMeta());
	return camera;
}
