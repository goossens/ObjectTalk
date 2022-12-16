//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtException.h"
#include "OtFunction.h"

#include "OtFramework.h"
#include "OtPicture.h"


//
//	OtPictureClass::init
//

void OtPictureClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setMargin(parameters[1]->operator int());

		case 1:
			setTexture(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtExcept("[Picture] constructor expects up to 2 arguments (not %ld)", count);
	}
}


//
//	OtPictureClass::setTexture
//

OtObject OtPictureClass::setTexture(const std::string& textureName) {
	texture.loadFromFile(textureName);
	width = texture.getWidth();
	height = texture.getHeight();
	return shared();
}


//
//	OtPictureClass::setMargin
//

OtObject OtPictureClass::setMargin(int m) {
	margin = m;
	return shared();
}


//
//	OtPictureClass::render
//

void OtPictureClass::render() {
	// sanity check
	if (!texture.isValid()) {
		OtExcept("No image provided for [Picture] widget");
	}

	// add margin if required
	if (margin) {
		ImGui::Dummy(ImVec2(0, margin));
	}

	// position and render image
	float indent = ImGui::GetCursorPosX();
	float availableSpace = ImGui::GetWindowSize().x - indent;

	ImGui::SetCursorPos(ImVec2(
		indent + (availableSpace - width) / 2,
		ImGui::GetCursorPosY()));

	ImGui::Image((void*)(intptr_t) texture.getTextureIndex(), ImVec2(width, height));

	// add margin if required
	if (margin) {
		ImGui::Dummy(ImVec2(0, margin));
	}
}


//
//	OtPictureClass::getMeta
//

OtType OtPictureClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtPictureClass>("Picture", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPictureClass::init));

		type->set("setTexture", OtFunctionClass::create(&OtPictureClass::setTexture));
		type->set("setMargin", OtFunctionClass::create(&OtPictureClass::setMargin));
	}

	return type;
}


//
//	OtPictureClass::create
//

OtPicture OtPictureClass::create() {
	OtPicture picture = std::make_shared<OtPictureClass>();
	picture->setType(getMeta());
	return picture;
}
