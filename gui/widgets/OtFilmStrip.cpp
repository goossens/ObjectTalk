//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

void OtFilmStripClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 6:
			setVerticalAlignment(OtUi::Alignment(parameters[5]->operator int()));
			[[fallthrough]];

		case 5:
			setHorizontalAlignment(OtUi::Alignment(parameters[4]->operator int()));
			[[fallthrough]];

		case 4:
			setValue(parameters[3]->operator float());
			[[fallthrough]];

		case 3:
			if (parameters[2]->operator float()) {
				setHorizontal();
			}

			[[fallthrough]];

		case 2:
			setFrames(parameters[1]->operator int());
			[[fallthrough]];

		case 1:
			setFilmStrip(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
		OtLogError("[FilmStrip] constructor expects up to 5 arguments (not {})", count);
	}
}


//
//	OtFilmStripClass::render
//

void OtFilmStripClass::render() {
	// sanity check
	if (filmstrip.isNull()) {
		OtLogError("[FilmStrip] does not have the filmstrip");
	}

	if (!frames) {
		OtLogError("[FilmStrip] does not have frame count");
	}

	// wait until texture is ready
	if (filmstrip.isReady()) {
		// determine framesize
		auto& texture = filmstrip->getTexture();
		float wt = static_cast<float>(texture.getWidth());
		float ht = static_cast<float>(texture.getHeight());
		float w = wt / (horizontal ? frames : 1);
		float h = ht / (!horizontal ? frames : 1);

		// determine coordinates of frame in filmstrip
		auto frame = std::floor(value * (frames - 1)) / frames;
		auto tl = horizontal ? ImVec2(frame, 0.0f) : ImVec2(0.0f, frame);
		auto br = horizontal ? ImVec2(frame + w / wt, 1.0f) : ImVec2(1.0f, frame + h / ht);

		// determine visible size and position
		auto size = ImVec2(w * scale, h * scale);
		ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().FramePadding.y));
		OtUi::align(size, horizontalAlign, verticalAlign);
		auto pos = ImGui::GetCursorScreenPos();

		// render frame
		ImGui::Image(texture.getTextureID(), size, tl, br);

		// handle user interaction (if required)
		if (callback) {
			ImGui::PushID(this);
			ImGuiIO& io = ImGui::GetIO();
			ImGui::SetCursorScreenPos(pos);
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
			ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().FramePadding.y));
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
