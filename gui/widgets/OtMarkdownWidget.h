//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtMarkdown.h"

#include "OtWidget.h"


//
//	OtMarkdownWidgetClass
//

class OtMarkdownWidgetClass;
using OtMarkdownWidget = OtObjectPointer<OtMarkdownWidgetClass>;

class OtMarkdownWidgetClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline void setText(const std::string& txt) { text = txt; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtMarkdownWidgetClass>;
	OtMarkdownWidgetClass() = default;

private:
	// properties
	OtMarkdown markdown;
	std::string text;
};
