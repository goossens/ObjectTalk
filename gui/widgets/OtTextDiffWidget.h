//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "TextDiff.h"

#include "OtWidget.h"


//
//	OtTextDiffWidgetClass
//

class OtTextDiffWidgetClass;
using OtTextDiffWidget = OtObjectPointer<OtTextDiffWidgetClass>;

class OtTextDiffWidgetClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access editor options
	inline void setTabSize(int value) { diff.SetTabSize(value); }
	inline int getTabSize() { return diff.GetTabSize(); }
	inline void setLineSpacing(float value) { diff.SetLineSpacing(value); }
	inline float getLineSpacing() { return diff.GetLineSpacing(); }
	inline void setShowWhitespacesEnabled(bool value) { diff.SetShowWhitespacesEnabled(value); }
	inline bool isShowWhitespacesEnabled() { return diff.IsShowWhitespacesEnabled(); }
	inline void setShowScrollbarMiniMapEnabled(bool value) { diff.SetShowScrollbarMiniMapEnabled(value); }
	inline bool isShowScrollbarMiniMapEnabled() { return diff.IsShowScrollbarMiniMapEnabled(); }

	void setLanguage(const std::string& text);
	inline std::string getLanguage() { return diff.GetLanguageName(); }

	// access text (using UTF-8 encoded strings)
	inline void setText(const std::string& left, const std::string& right) { diff.SetText(left, right); }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	// properties
	TextDiff diff;
};
