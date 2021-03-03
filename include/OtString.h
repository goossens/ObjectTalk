//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtString
//

class OtStringClass;
typedef std::shared_ptr<OtStringClass> OtString;

class OtStringClass : public OtPrimitiveClass {
public:
	// constructors
	OtStringClass() = default;
	OtStringClass(const std::string& string) : value(string) {}

	// convertors
	operator bool() { auto v = value; std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v =="true"; }
	operator long() { return std::stol(value); }
	operator size_t() { return (size_t) std::stol(value); }
	operator double() { return std::stof(value); }
	operator std::string() {return value; }

	std::string json() { return OtTextToJSON(value); }

	// support index operations
	class OtStringReferenceClass;
	typedef std::shared_ptr<OtStringReferenceClass> OtStringReference;

	class OtStringReferenceClass : public OtInternalClass {
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
				type = OtTypeClass::create<OtStringReferenceClass>("StringReference", OtInternalClass::getMeta());
				type->set("__deref__", OtFunctionCreate(&OtStringReferenceClass::deref));
				type->set("__assign__", OtFunctionCreate(&OtStringReferenceClass::assign));
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

	// support indexing
	OtObject index(size_t index) {
		return OtStringReferenceClass::create(cast<OtStringClass>(), index);
	}

	// support iterator
	class OtStringIteratorClass;
	typedef std::shared_ptr<OtStringIteratorClass> OtStringIterator;

	class OtStringIteratorClass : public OtInternalClass {
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
				type = OtTypeClass::create<OtStringIteratorClass>("StringIterator", OtInternalClass::getMeta());
				type->set("__end__", OtFunctionCreate(&OtStringIteratorClass::end));
				type->set("__next__", OtFunctionCreate(&OtStringIteratorClass::next));
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

	// start iterator
	OtObject iterate() {
		return OtStringIteratorClass::create(cast<OtStringClass>());
	}

	// "arithmetic"
	std::string add(const std::string& operand) { return value + operand; }

	// comparison
	bool equal(const std::string& operand) { return value == operand; }
	bool notEqual(const std::string& operand) { return value != operand; }
	bool greaterThan(const std::string& operand) { return value > operand; }
	bool lessThan(const std::string& operand) { return value < operand; }
	bool greaterEqual(const std::string& operand) { return value >= operand; }
	bool lessEqual(const std::string& operand) { return value <= operand; }

	long casecmp(const std::string& operand) { return OtTextCaseCmp(value, operand); }

	// functions
	size_t len() { return (size_t) OtTextLen(value); }

	std::string left(size_t count) { return OtTextLeft(value, count); }
	std::string right(size_t count) { return OtTextRight(value, count); }
	std::string mid(size_t start, size_t count) { return OtTextMid(value, start, count); }

	size_t find(const std::string& sub) { std::string::size_type p = value.find(sub); return p == std::string::npos ? -1 : (size_t) p; }
	bool startsWith(const std::string& sub) { return OtTextStartsWith(value, sub); }
	bool contains(const std::string& sub) { return OtTextContains(value, sub); }

	std::string trim() { return OtTextTrim(value); }
	std::string ltrim() { return OtTextLeftTrim(value); }
	std::string rtrim() { return OtTextRightTrim(value); }
	std::string compress() { return OtTextCompress(value); }

	std::string lower() { return OtTextLower(value); }
	std::string upper() { return OtTextUpper(value); }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtStringClass>("String", OtPrimitiveClass::getMeta());

			type->set("__index__", OtFunctionCreate(&OtStringClass::index));
			type->set("__iter__", OtFunctionCreate(&OtStringClass::iterate));
			type->set("__add__", OtFunctionCreate(&OtStringClass::add));

			type->set("__eq__", OtFunctionCreate(&OtStringClass::equal));
			type->set("__ne__", OtFunctionCreate(&OtStringClass::notEqual));
			type->set("__gt__", OtFunctionCreate(&OtStringClass::greaterThan));
			type->set("__lt__", OtFunctionCreate(&OtStringClass::lessThan));
			type->set("__ge__", OtFunctionCreate(&OtStringClass::greaterEqual));
			type->set("__le__", OtFunctionCreate(&OtStringClass::lessEqual));

			type->set("casecmp", OtFunctionCreate(&OtStringClass::casecmp));

			type->set("len", OtFunctionCreate(&OtStringClass::len));

			type->set("left", OtFunctionCreate(&OtStringClass::left));
			type->set("right", OtFunctionCreate(&OtStringClass::right));
			type->set("mid", OtFunctionCreate(&OtStringClass::mid));

			type->set("find", OtFunctionCreate(&OtStringClass::find));
			type->set("startsWith", OtFunctionCreate(&OtStringClass::startsWith));
			type->set("contains", OtFunctionCreate(&OtStringClass::contains));

			type->set("trim", OtFunctionCreate(&OtStringClass::trim));
			type->set("ltrim", OtFunctionCreate(&OtStringClass::ltrim));
			type->set("rtrim", OtFunctionCreate(&OtStringClass::rtrim));
			type->set("compress", OtFunctionCreate(&OtStringClass::compress));

			type->set("lower", OtFunctionCreate(&OtStringClass::lower));
			type->set("upper", OtFunctionCreate(&OtStringClass::upper));
		}

		return type;
	}

	// create a new object
	static OtString create(const std::string& value) {
		auto string = std::make_shared<OtStringClass>(value);
		string->setType(getMeta());
		return string;
	}

private:
	std::string value {""};
};


//
//	Overloaded conversion function for ObjectTalk function calls
//

inline OtObject OtObjectCreate(const std::string& value) { return OtStringClass::create(value); }
