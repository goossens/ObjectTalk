//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include <vector>

#include "OtUi.h"

#include "OtGui.h"


//
//	OtWidgetClass
//

class OtWidgetClass;
using OtWidget = OtObjectPointer<OtWidgetClass>;

class OtWidgetClass : public OtGuiClass {
public:
	// add / remove child components
	OtObject add(OtObject object);
	OtObject remove(OtObject object);

	// get our parent
	inline OtWidget getParent() { return OtWidget(parent); }
	inline bool hasParent() { return parent != nullptr; }

	// get maximum number of children
	virtual inline int getMaxChildren() { return 0; }

	// ensure specified widget is allowed as a child
	virtual inline void validateChild(OtWidget /* child */) {}

	// iterate through children
	inline void each(std::function<void(OtWidget)> callback) {
		for (auto child : children) {
			callback(child);
		}
	}

	// remove all children
	void clear();

	// return number of children
	size_t size() { return children.size(); }

	// update enabled flag
	OtObject enable() { enabled = true; return OtWidget(this); }
	OtObject disable() { enabled = false; return OtWidget(this); }
	OtObject setEnabled(bool e) { enabled = e; return OtWidget(this); }
	bool isEnabled() { return enabled; }

	// render content
	virtual void render() {}

	// get type definition
	static OtType getMeta();

protected:
	// parent widget
	OtWidgetClass* parent = nullptr;

	// our children
	std::vector<OtWidget> children;

	// enabled flag
	bool enabled = true;

	// render all enabled children
	void renderChildren();

	// render a single child
	void renderChild(OtWidget child);
};
