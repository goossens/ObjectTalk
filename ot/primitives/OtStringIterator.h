//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtCodePoint.h"
#include "OtIterator.h"
#include "OtString.h"


//
//	OtStringIteratorClass
//

class OtStringIteratorClass;
typedef std::shared_ptr<OtStringIteratorClass> OtStringIterator;

class OtStringIteratorClass : public OtIteratorClass {
public:
	// constructors
	OtStringIteratorClass() = default;

	OtStringIteratorClass(OtString string) {
		pos = string->value.cbegin();
		last = string->value.cend();
	}

	// iteration operations
	bool end() {
		return pos == last;
	}

	std::string next() {
		size_t size = OtCodePoint::size(pos);
		std::string result(pos, pos + size);
		pos += size;
		return result;
	}

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtStringIterator create(OtString string);

private:
	std::string::const_iterator pos;
	std::string::const_iterator last;
};
