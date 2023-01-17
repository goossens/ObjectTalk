//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject3d.h"


//
//	OtGroupClass
//

class OtGroupClass;
typedef std::shared_ptr<OtGroupClass> OtGroup;

class OtGroupClass : public OtObject3dClass {
public:
	// ensure specified node is allowed as a child
	void validateChild(OtNode child) override;

	// pass shadow flag to all children
	OtObject castShadow(bool flag) override;
	OtObject receiveShadow(bool flag) override;

	// render children
	void render(OtRenderer& renderer) override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtGroup create();
};
