//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "internal.h"


//
//	OtInternal
//

class OtIteratorClass;
typedef std::shared_ptr<OtIteratorClass> OtIterator;

class OtIteratorClass : public OtInternalClass {
public:
	// get type definition
	static OtType getMeta();
};
