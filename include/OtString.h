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

			type->set("__index__", OtFunctionClass::create(&OtStringClass::index));
			type->set("__iter__", OtFunctionClass::create(&OtStringClass::iterate));
			type->set("__add__", OtFunctionClass::create(&OtStringClass::add));

			type->set("__eq__", OtFunctionClass::create(&OtStringClass::equal));
			type->set("__ne__", OtFunctionClass::create(&OtStringClass::notEqual));
			type->set("__gt__", OtFunctionClass::create(&OtStringClass::greaterThan));
			type->set("__lt__", OtFunctionClass::create(&OtStringClass::lessThan));
			type->set("__ge__", OtFunctionClass::create(&OtStringClass::greaterEqual));
			type->set("__le__", OtFunctionClass::create(&OtStringClass::lessEqual));

			type->set("casecmp", OtFunctionClass::create(&OtStringClass::casecmp));

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
