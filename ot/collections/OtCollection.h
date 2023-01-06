//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"


//
//	OtCollection
//

class OtCollectionClass;
typedef std::shared_ptr<OtCollectionClass> OtCollection;

class OtCollectionClass : public OtObjectClass {
public:
	// get type definition
	static OtType getMeta();
};
