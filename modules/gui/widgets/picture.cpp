//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/function.h"

#include "picture.h"


//
//	OtPictureClass::init
//

OtObject OtPictureClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 2:
			setMargin(parameters[1]->operator int());

		case 1:
			setTexture(parameters[0]);

		case 0:
			break;

		default:
			OtExcept("[Picture] constructor expects up to 2 arguments (not %ld)", count);
	}

	return nullptr;
}


//
//	OtPictureClass::setTexture
//
OtObject OtPictureClass::setTexture(OtObject object) {
	// ensure object is a texture
	if (object->isKindOf("Texture")) {
		texture = object->cast<OtTextureClass>();

	} else if (object->isKindOf("String")) {
		texture = OtTextureClass::create();
		texture->loadImage(object->operator std::string());

	} else {
		OtExcept("Expected a [Texture] or [String] object, not a [%s]", object->getType()->getName().c_str());
	}

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
	// add margin if required
	if (margin) {
		ImGui::Dummy(ImVec2(0, margin));
	}

	// position and render image
	ImGui::SetCursorPos(ImVec2((
		ImGui::GetWindowSize().x - texture->getWidth()) / 2,
		ImGui::GetCursorPosY()));

	ImGui::Image(
		(void*)(intptr_t) texture->getTextureHandle().idx,
		ImVec2(texture->getWidth(), texture->getHeight()));

	// add margin if required
	if (margin) {
		ImGui::Dummy(ImVec2(0, margin));
	}
}


//
//	OtPictureClass::getMeta
//

OtType OtPictureClass::getMeta() {
	static OtType type = nullptr;

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
