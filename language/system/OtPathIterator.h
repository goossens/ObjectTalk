//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "OtIterator.h"
#include "OtPathObject.h"


//
//	OtPathIteratorClass
//

class OtPathIteratorClass;
using OtPathIterator = OtObjectPointer<OtPathIteratorClass>;

class OtPathIteratorClass : public OtIteratorClass {
public:
	OtPathIteratorClass() = default;
	inline OtPathIteratorClass(OtPathObject p) { path = p; iterator = p->path.begin(); last = p->path.end(); }

	inline bool end() { return iterator == last; }
	inline OtObject next() { return OtPathObject::create(*(iterator++)); }

	// get type definition
	static OtType getMeta();

private:
	OtPathObject path;
	std::filesystem::path::iterator iterator;
	std::filesystem::path::iterator last;
};
