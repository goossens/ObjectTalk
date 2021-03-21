//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "internal.h"


//
//	OtContext
//

class OtContextClass : public OtInternalClass {
public:
	// parent access
	void setParent(OtContext v) { parent = v; }
	OtContext getParent() { return parent; }

	// member acccess
	bool has(const std::string& name);
	OtObject get(const std::string& name);

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtContext create();

private:
	// parent in context chain
	OtContext parent;
};
