//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtDict
//

class OtDictClass;
typedef std::shared_ptr<OtDictClass> OtDict;

class OtDictClass : public OtCollectionClass, public std::map<std::string, OtObject> {
public:
	// convert dictionary to string
	operator std::string() {
		std::ostringstream o;
		bool first = true;

		o << "{";

		for (auto const& entry : *this) {
			if (first) {
				first = false;
			} else {
				o << ",";
			}

			o << "\"" << OtTextToJSON(entry.first) << "\":" << entry.second->json();
		}

		o <<  "}";
		return o.str();
	}

	// initializer
	OtObject init(OtObject, size_t count, OtObject* parameters) {
		// clear dictionary and add all calling parameters
		clear();

		for (size_t c = 0; c < count; c += 2) {
			insert(std::make_pair((std::string) *parameters[c], parameters[c + 1]));
		}

		return getSharedPtr();
	}

	// support index operator
	class OtDictReferenceClass;
	typedef std::shared_ptr<OtDictReferenceClass> OtDictReference;

	class OtDictReferenceClass : public OtInternalClass {
	public:
		// constructors
		OtDictReferenceClass() = default;
		OtDictReferenceClass(OtDict d, const std::string& i) : dict(d), index(i) {}

		// index operations
		OtObject deref() { return dict->operator[] (index); }
		OtObject assign(OtObject value) { dict->operator[] (index) = value; return value; }

		// get type definition
		static OtType getMeta() {
			static OtType type = nullptr;

			if (!type) {
				type = OtTypeClass::create<OtDictReferenceClass>("DictReference", OtInternalClass::getMeta());
				type->set("__deref__", OtFunctionCreate(&OtDictReferenceClass::deref));
				type->set("__assign__", OtFunctionCreate(&OtDictReferenceClass::assign));
			}

			return type;
		}

		// create a new object
		static OtDictReference create(OtDict a, const std::string& i) {
			OtDictReference reference = std::make_shared<OtDictReferenceClass>(a, i);
			reference->setType(getMeta());
			return reference;
		}

	private:
		OtDict dict;
		std::string index;
	};

	// support indexing
	OtObject index(const std::string& index) {
		return OtDictReferenceClass::create(cast<OtDictClass>(), index);
	}

	// get dictionary size
	size_t mySize() {
		return size();
	}

	// return dictionary clone
	OtObject clone() {
		OtDict result = create();

		for (auto& it : *this) {
			result->insert(std::make_pair(it.first, it.second));
		}

		return result;
	}

	// remove dictionary entry
	OtObject eraseEntry(const std::string& name) {
		OtObject value = operator[] (name);
		erase(name);
		return value;
	}

	// get array of dictionary names
	OtObject keys() {
		// create array of all keys in context
		OtArray array = OtArrayClass::create();

		for (auto const& entry : *this) {
			array->push_back(OtStringClass::create(entry.first));
		}

		return array;
	}

	// get array of dictionary values
	OtObject values() {
		// create array of all values in context
		OtArray array = OtArrayClass::create();

		for (auto const& entry : *this) {
			array->push_back(entry.second);
		}

		return array;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtDictClass>("Dict", OtCollectionClass::getMeta());

			type->set("__init__", OtFunctionClass::create(&OtDictClass::init));
			type->set("__index__", OtFunctionCreate(&OtDictClass::index));

			type->set("size", OtFunctionCreate(&OtDictClass::mySize));

			type->set("clone", OtFunctionCreate(&OtDictClass::clone));
			type->set("clear", OtFunctionCreate(&OtDictClass::clear));
			type->set("erase", OtFunctionCreate(&OtDictClass::eraseEntry));

			type->set("keys", OtFunctionCreate(&OtDictClass::keys));
			type->set("values", OtFunctionCreate(&OtDictClass::values));
		}

		return type;
	}

	// create a new object
	static OtDict create() {
		OtDict dict = std::make_shared<OtDictClass>();
		dict->setType(getMeta());
		return dict;
	}

	static OtDict create(size_t count, OtObject* values) {
		OtDict dict = create();

		for (size_t c = 0; c < count; c += 2) {
			dict->insert(std::make_pair((std::string) *values[c], values[c + 1]));
		}

		return dict;
	}
};
