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

void OtPictureClass::init(const std::string& filename) {
	// load the texture
	texture = OtTextureClass::create();
	texture->loadImage(filename);
}


//
//	OtPictureClass::render
//

void OtPictureClass::render() {
	ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - texture->getWidth()) / 2, 0));

	ImGui::Image(
		(void*)(intptr_t) texture->getTextureHandle().idx,
		ImVec2(texture->getWidth(), texture->getHeight()));
}


//
//	OtPictureClass::getMeta
//

OtType OtPictureClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtPictureClass>("Picture", OtWidgetClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtPictureClass::init));
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
