//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"

#include "OtUi.h"

#include "OtFilmStrip.h"


//
//	OtFilmStripClass::init
//

void OtFilmStripClass::init(size_t count, OtObject *paraFilmStrips) {
	switch (count) {
		case 4:
			setHorizontalAlignment(OtUiAlignment(paraFilmStrips[3]->operator int()));

		case 3:
			setHorizontalAlignment(OtUiAlignment(paraFilmStrips[2]->operator int()));

		case 2:
			setValue(paraFilmStrips[1]->operator float());

		case 1:
			setTexture(paraFilmStrips[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[FilmStrip] constructor expects up to 5 argument (not {})", count);
	}
}


//
//	OtFilmStripClass::render
//

void OtFilmStripClass::render() {
	ImGui::PushID(this);
	ImGui::PopID();
}

//
//	OtFilmStripClass::getMeta
//

OtType OtFilmStripClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtFilmStripClass>("FilmStrip", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtFilmStripClass::init));
		type->set("setTexture", OtFunction::create(&OtFilmStripClass::setTexture));
		type->set("setValue", OtFunction::create(&OtFilmStripClass::setValue));
		type->set("getValue", OtFunction::create(&OtFilmStripClass::getValue));
		type->set("setHorizontalAlignment", OtFunction::create(&OtFilmStripClass::setHorizontalAlignment));
		type->set("setVerticalAlignment", OtFunction::create(&OtFilmStripClass::setVerticalAlignment));
	}

	return type;
}
