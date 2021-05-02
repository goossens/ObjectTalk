//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/codepoint.h"
#include "ot/text.h"

#include "ot/function.h"
#include "ot/string.h"
#include "ot/stringreference.h"
#include "ot/stringiterator.h"
#include "ot/array.h"


//
//	OtStringClass::getEntry
//

std::string OtStringClass::getEntry(size_t index) {
	// sanity check
	if (index < 0 || index >= len()) {
		OtExcept("invalid index [%ld] for string of size [%ld]", index, len());
	}

	return OtText::get(value, index);
}


//
//	OtStringClass::setEntry
//

std::string OtStringClass::setEntry(size_t index, const std::string& string) {
	// sanity check
	if (index < 0 || index >= len()) {
		OtExcept("invalid index [%ld] for string of size [%ld]", index, len());
	}

	value = OtText::set(value, index, string);
	return value;
}


//
//	OtStringClass::index
//

OtObject OtStringClass::index(size_t index) {
	// sanity check
	if (index < 0 || index >= len()) {
		OtExcept("invalid index [%ld] for string of size [%ld]", index, len());
	}

	return OtStringReferenceClass::create(cast<OtStringClass>(), index);
}


//
//	OtStringClass::iterate
//

OtObject OtStringClass::iterate() {
	return OtStringIteratorClass::create(cast<OtStringClass>());
}


//
//	OtStringClass::split
//

OtObject OtStringClass::split(const std::string& delimiter) {
	OtArray result = OtArrayClass::create();
	size_t start = 0;
	size_t end;

	while ((end = value.find(delimiter, start)) != std::string::npos) {
		result->append(OtStringClass::create(value.substr(start, end - start)));
		start = end + delimiter.size();
	}

	result->append(OtStringClass::create(value.substr(start)));
	return result;
}


//
//	OtStringClass::getMeta
//

OtType OtStringClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtStringClass>("String", OtPrimitiveClass::getMeta());

		type->set("boolean", OtFunctionClass::create(&OtStringClass::operator bool));
		type->set("integer", OtFunctionClass::create(&OtStringClass::operator long));
		type->set("real", OtFunctionClass::create(&OtStringClass::operator double));
		type->set("string", OtFunctionClass::create(&OtStringClass::operator std::string));
		type->set("json", OtFunctionClass::create(&OtStringClass::json));

		type->set("__eq__", OtFunctionClass::create(&OtStringClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtStringClass::notEqual));
		type->set("__gt__", OtFunctionClass::create(&OtStringClass::greaterThan));
		type->set("__lt__", OtFunctionClass::create(&OtStringClass::lessThan));
		type->set("__ge__", OtFunctionClass::create(&OtStringClass::greaterEqual));
		type->set("__le__", OtFunctionClass::create(&OtStringClass::lessEqual));

		type->set("casecmp", OtFunctionClass::create(&OtStringClass::casecmp));

		type->set("__index__", OtFunctionClass::create(&OtStringClass::index));
		type->set("__iter__", OtFunctionClass::create(&OtStringClass::iterate));
		type->set("__add__", OtFunctionClass::create(&OtStringClass::add));

		type->set("len", OtFunctionClass::create(&OtStringClass::len));

		type->set("left", OtFunctionClass::create(&OtStringClass::left));
		type->set("right", OtFunctionClass::create(&OtStringClass::right));
		type->set("mid", OtFunctionClass::create(&OtStringClass::mid));

		type->set("find", OtFunctionClass::create(&OtStringClass::find));
		type->set("startsWith", OtFunctionClass::create(&OtStringClass::startsWith));
		type->set("contains", OtFunctionClass::create(&OtStringClass::contains));
		type->set("__contains__", OtFunctionClass::create(&OtStringClass::contains));

		type->set("trim", OtFunctionClass::create(&OtStringClass::trim));
		type->set("ltrim", OtFunctionClass::create(&OtStringClass::ltrim));
		type->set("rtrim", OtFunctionClass::create(&OtStringClass::rtrim));
		type->set("compress", OtFunctionClass::create(&OtStringClass::compress));

		type->set("lower", OtFunctionClass::create(&OtStringClass::lower));
		type->set("upper", OtFunctionClass::create(&OtStringClass::upper));

		type->set("split", OtFunctionClass::create(&OtStringClass::split));
	}

	return type;
}


//
//	OtStringClass::create
//

OtString OtStringClass::create(const std::string& value) {
	auto string = std::make_shared<OtStringClass>(value);
	string->setType(getMeta());
	return string;
}
