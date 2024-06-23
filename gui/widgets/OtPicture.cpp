//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "glm/glm.hpp"
#include "imgui.h"

#include "OtFunction.h"
#include "OtLog.h"

#include "OtBlit.h"
#include "OtPass.h"
#include "OtTransientVertexBuffer.h"
#include "OtUi.h"
#include "OtVertex.h"

#include "OtPicture.h"


//
//	OtPictureClass::init
//

void OtPictureClass::init(size_t count, OtObject *parameters) {
	switch (count) {
		case 4:
			setHorizontalAlignment(OtUiAlignment(parameters[3]->operator int()));

		case 3:
			setHorizontalAlignment(OtUiAlignment(parameters[2]->operator int()));

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
		OtUiAlign(size, horizontalAlign, verticalAlign);
		ImGui::Image((void*)(intptr_t) texture.getIndex(), size);
	}
}


//
//	OtPictureClass::getMeta
//

OtType OtPictureClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPictureClass>("Picture", OtWidgetClass::getMeta());
		type->set("setPicture", OtFunction::create(&OtPictureClass::setPicture));
		type->set("setScale", OtFunction::create(&OtPictureClass::setScale));
		type->set("setHorizontalAlignment", OtFunction::create(&OtPictureClass::setHorizontalAlignment));
		type->set("setVerticalAlignment", OtFunction::create(&OtPictureClass::setVerticalAlignment));
	}

	return type;
}
