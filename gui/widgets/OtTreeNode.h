//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtWidget.h"


//
//	OtTreeNodeClass
//

class OtTreeNodeClass;
using OtTreeNode = OtObjectPointer<OtTreeNodeClass>;

class OtTreeNodeClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// access properties
	inline OtObject setLabel(const std::string& l) {
		label = l;
		return OtWidget(this);
	}

	inline OtObject setOpen(const bool o) {
		openFlag = o;
		return OtWidget(this);
	}

	inline void open() { openFlag = true; }
	inline void close() { openFlag = false; }
	inline bool isOpen() { return openFlag; }

	// get maximum number of children
	inline int getMaxChildren() override { return -1; }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

private:
	std::string label;
	bool openFlag = true;
};
