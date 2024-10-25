//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <algorithm>

#include "imgui.h"

#include "OtFunction.h"
#include "OtVM.h"

#include "OtUi.h"

#include "OtFilmStrip.h"


//
//	OtFilmStripClass::init
//

void OtFilmStripClass::init(size_t count, OtObject *parameters) {
	switch (count) {
		case 6:
			setVerticalAlignment(OtUiAlignment(parameters[5]->operator int()));

		case 5:
			setHorizontalAlignment(OtUiAlignment(parameters[4]->operator int()));

		case 4:
			setValue(parameters[3]->operator float());

		case 3:
			if (parameters[2]->operator float()) {
				setHorizontal();
			}

		case 2:
			setFrames(parameters[1]->operator int());

		case 1:
			setFilmStrip(parameters[0]->operator std::string());

		case 0:
			break;

		default:
			OtLogFatal("[FilmStrip] constructor expects up to 5 arguments (not {})", count);
	}
}


//
//	OtFilmStripClass::render
//

void OtFilmStripClass::render() {
	// sanity check
	if (filmstrip.isNull()) {
		OtLogFatal("[FilmStrip] does not have the filmstrip");
	}

	if (!frames) {
		OtLogFatal("[FilmStrip] does not have frame count");
	}

	// wait until texture is ready
	if (filmstrip.isReady()) {
		// determine framesize
		auto& texture = filmstrip->getTexture();
		float wt = texture.getWidth();
		float ht = texture.getHeight();
		float w = wt / (horizontal ? frames : 1);
		float h = ht / (!horizontal ? frames : 1);

		// determine coordinates of frame in filmstrip
		auto frame = std::floor(value * (frames - 1)) / frames;
		auto tl = horizontal ? ImVec2(frame, 0.0f) : ImVec2(0.0f, frame);
		auto br = horizontal ? ImVec2(frame + w / wt, 1.0f) : ImVec2(1.0f, frame + h / ht);

		// determine visible size and position
		auto size = ImVec2(w * scale, h * scale);
		ImGui::Dummy(ImVec2(0, ImGui::GetStyle().FramePadding.y));
		OtUiAlign(size, horizontalAlign, verticalAlign);
		auto pos = ImGui::GetCursorPos();

		// render frame
		ImGui::Image((ImTextureID)(intptr_t) texture.getIndex(), size, tl, br);

		// handle user interaction (if required)
		if (callback) {
			ImGui::PushID(this);
			ImGuiIO& io = ImGui::GetIO();
			ImGui::SetCursorPos(pos);
			ImGui::InvisibleButton("", size, 0);

			if (ImGui::IsItemActive() && io.MouseDelta.y != 0.0) {
				auto newValue = std::clamp(value - io.MouseDelta.y / 200.0f, 0.0f, 1.0f);

				// call user callback if value has changed
				if (callback && newValue != value) {
					OtVM::callMemberFunction(callback, "__call__", OtValue<float>::encode(value));
				}

				value = newValue;
			}

			ImGui::PopID();
			ImGui::Dummy(ImVec2(0, ImGui::GetStyle().FramePadding.y));
		}
	}
}


//
//	OtFilmStripClass::getMeta
//

OtType OtFilmStripClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtFilmStripClass>("FilmStrip", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtFilmStripClass::init));
		type->set("setFilmStrip", OtFunction::create(&OtFilmStripClass::setFilmStrip));
		type->set("setFrames", OtFunction::create(&OtFilmStripClass::setFrames));
		type->set("setHorizontal", OtFunction::create(&OtFilmStripClass::setHorizontal));
		type->set("setValue", OtFunction::create(&OtFilmStripClass::setValue));
		type->set("getValue", OtFunction::create(&OtFilmStripClass::getValue));
		type->set("setScale", OtFunction::create(&OtFilmStripClass::setScale));
		type->set("setHorizontalAlignment", OtFunction::create(&OtFilmStripClass::setHorizontalAlignment));
		type->set("setVerticalAlignment", OtFunction::create(&OtFilmStripClass::setVerticalAlignment));
		type->set("setCallback", OtFunction::create(&OtFilmStripClass::setCallback));
	}

	return type;
}
