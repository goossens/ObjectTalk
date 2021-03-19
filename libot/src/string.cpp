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

#include "ot/string.h"
#include "ot/function.h"
#include "ot/reference.h"
#include "ot/iterator.h"


//
//	OtStringReference
//

class OtStringReferenceClass;
typedef std::shared_ptr<OtStringReferenceClass> OtStringReference;

class OtStringReferenceClass : public OtReferenceClass {
public:
	// constructors
	OtStringReferenceClass() = default;
	OtStringReferenceClass(OtString t, size_t i) : target(t), index(i) {}

	// index operations
	std::string deref() { return OtTextGet(target->value, index); }

	OtObject assign(OtObject value) {
		auto ch = value->operator std::string();
		target->value = OtTextSet(target->value, index, ch);
		return value;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtStringReferenceClass>("StringReference", OtReferenceClass::getMeta());
			type->set("__deref__", OtFunctionClass::create(&OtStringReferenceClass::deref));
			type->set("__assign__", OtFunctionClass::create(&OtStringReferenceClass::assign));
		}

		return type;
	}

	// create a new object
	static OtStringReference create(OtString t, size_t i) {
		OtStringReference reference = std::make_shared<OtStringReferenceClass>(t, i);
		reference->setType(getMeta());
		return reference;
	}

private:
	OtString target;
	size_t index;
};


//
//	OtStringClass::index
//

OtObject OtStringClass::index(size_t index) {
	if (index < 0) {
		OT_EXCEPT("Negative index [%ld] is not allowed in a string", index);
		
	} else if (index >= value.size()) {
		OT_EXCEPT("Index [%ld] is greater than string length [%ld]", index, value.size());

	}

	return OtStringReferenceClass::create(cast<OtStringClass>(), index);
}


//
//	OtStringIteratorClass
//

class OtStringIteratorClass;
typedef std::shared_ptr<OtStringIteratorClass> OtStringIterator;

class OtStringIteratorClass : public OtIteratorClass {
public:
	// constructors
	OtStringIteratorClass() = default;
	OtStringIteratorClass(OtString t) {
		pos = t->value.cbegin();
		last = t->value.cend();
	}

	// iteration operations
	bool end() { return pos == last; }

	std::string next() {
		size_t size = OtCodePointSize(pos);
		std::string result(pos, pos + size);
		pos += size;
		return result;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtStringIteratorClass>("StringIterator", OtIteratorClass::getMeta());
			type->set("__end__", OtFunctionClass::create(&OtStringIteratorClass::end));
			type->set("__next__", OtFunctionClass::create(&OtStringIteratorClass::next));
		}

		return type;
	}

	// create a new object
	static OtStringIterator create(OtString t) {
		OtStringIterator iterator = std::make_shared<OtStringIteratorClass>(t);
		iterator->setType(getMeta());
		return iterator;
	}

private:
	std::string::const_iterator pos;
	std::string::const_iterator last;
};


//
//	OtStringClass::iterate
//

OtObject OtStringClass::iterate() {
	return OtStringIteratorClass::create(cast<OtStringClass>());
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

		type->set("trim", OtFunctionClass::create(&OtStringClass::trim));
		type->set("ltrim", OtFunctionClass::create(&OtStringClass::ltrim));
		type->set("rtrim", OtFunctionClass::create(&OtStringClass::rtrim));
		type->set("compress", OtFunctionClass::create(&OtStringClass::compress));

		type->set("lower", OtFunctionClass::create(&OtStringClass::lower));
		type->set("upper", OtFunctionClass::create(&OtStringClass::upper));
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
