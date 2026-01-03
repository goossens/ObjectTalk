//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
using OtStringIterator = OtObjectPointer<OtStringIteratorClass>;

class OtStringIteratorClass : public OtIteratorClass {
public:
	// iteration operations
	inline bool end() {
		return pos == last;
	}

	inline std::string next() {
		size_t size = OtCodePoint::size(pos);
		std::string result(pos, pos + size);
		pos += size;
		return result;
	}

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtStringIteratorClass>;
	OtStringIteratorClass() = default;

	inline OtStringIteratorClass(OtString string) {
		pos = string->value.cbegin();
		last = string->value.cend();
	}

private:
	// data
	std::string::const_iterator pos;
	std::string::const_iterator last;
};
