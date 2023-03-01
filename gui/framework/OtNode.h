//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>
#include "OtGui.h"


//
//	OtNodeClass
//

class OtNodeClass;
using OtNode = OtObjectPointer<OtNodeClass>;

class OtNodeClass : public OtGuiClass {
public:
	// add / remove child components
	OtObject add(OtObject object);
	OtObject remove(OtObject object);

	// get our parent
	OtNode getParent() { return OtNode(parent); }
	bool hasParent() { return parent != nullptr; }

	// ensure specified node is allowed as a child
	virtual void validateChild(OtNode child);

	// iterate through children
	inline void iterate(std::function<void(OtNode)> callback) {
		for (auto child : children) {
			callback(child);
		}
	}

	// remove all children
	void clear();

	// return number of children
	size_t size() { return children.size(); }

	// update enabled flag
	OtObject enable() { enabled = true; return OtObject(this); }
	OtObject disable() { enabled = false; return OtObject(this); }
	OtObject setEnabled(bool e) { enabled = e; return OtObject(this); }
	bool isEnabled() { return enabled; }

	// get type definition
	static OtType getMeta();

protected:
	// parent node
	OtNodeClass* parent = nullptr;

	// our children
	std::vector<OtNode> children;

	// enabled flag
	bool enabled = true;
};
