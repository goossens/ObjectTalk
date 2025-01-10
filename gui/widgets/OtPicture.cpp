//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtUi.h"

#include "OtPicture.h"


//
//	OtPictureClass::init
//

void OtPictureClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 4:
			setVerticalAlignment(OtUi::Alignment(parameters[3]->operator int()));

		case 3:
			setHorizontalAlignment(OtUi::Alignment(parameters[2]->operator int()));

		case 2:
			setScale(parameters[1]->operator float());

		case 1:
			setPicture(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[Picture] constructor expects up to 4 arguments (not {})", count);
	}
}


//
//	OtPictureClass::render
//

void OtPictureClass::render() {
	// sanity check
	if (picture.isNull()) {
		OtLogFatal("[Picture] does not have a texture");
	}

	// wait until picture is ready
	if (picture.isReady()) {
		auto& texture = picture->getTexture();
		auto size = ImVec2(texture.getWidth() * scale, texture.getHeight() * scale);
		OtUi::align(size, horizontalAlign, verticalAlign);
		ImGui::Image((ImTextureID)(intptr_t) texture.getIndex(), size);
	}
}


//
//	OtPictureClass::getMeta
//

OtType OtPictureClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPictureClass>("Picture", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtPictureClass::init));
		type->set("setPicture", OtFunction::create(&OtPictureClass::setPicture));
		type->set("setScale", OtFunction::create(&OtPictureClass::setScale));
		type->set("setHorizontalAlignment", OtFunction::create(&OtPictureClass::setHorizontalAlignment));
		type->set("setVerticalAlignment", OtFunction::create(&OtPictureClass::setVerticalAlignment));
	}

	return type;
}
