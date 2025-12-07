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

#include "OtMarkdownWidget.h"


//
//	OtMarkdownWidgetClass::init
//

void OtMarkdownWidgetClass::init(size_t count, OtObject* parameters) {
	switch (count) {
		case 1:
			setText(parameters[0]->operator std::string());
			[[fallthrough]];

		case 0:
			break;

		default:
			OtLogError("[Markdown] constructor expects up to 1 argument (not {})", count);
	}
}


//
//	OtMarkdownWidgetClass::render
//

void OtMarkdownWidgetClass::render() {
	markdown.render(text);
}


//
//	OtMarkdownWidgetClass::getMeta
//

OtType OtMarkdownWidgetClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtMarkdownWidgetClass>("Markdown", OtWidgetClass::getMeta());
		type->set("__init__", OtFunction::create(&OtMarkdownWidgetClass::init));
		type->set("setText", OtFunction::create(&OtMarkdownWidgetClass::setText));
	}

	return type;
}
