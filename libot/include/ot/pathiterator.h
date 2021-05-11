//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "ot/iterator.h"

#include "path.h"


//
//	OtPathIteratorClass
//

class OtPathIteratorClass;
typedef std::shared_ptr<OtPathIteratorClass> OtPathIterator;

class OtPathIteratorClass : public OtIteratorClass {
public:
	OtPathIteratorClass() = default;
	OtPathIteratorClass(OtPath p) { path = p; iterator = p->path.begin(); last = p->path.end(); }

	bool end() { return iterator == last; }
	OtObject next() { return OtPathClass::create(*(iterator++)); }

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPathIterator create(OtPath path);

private:
	OtPath path;
	std::filesystem::path::iterator iterator;
	std::filesystem::path::iterator last;
};
