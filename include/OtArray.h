//	ObjectTalk Scripting Language
//	Copyright 1993-2020 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


//
//	OtArray
//

class OtArrayClass;
typedef std::shared_ptr<OtArrayClass> OtArray;


//
//	OtArrayClass
//

class OtArrayClass : public OtCollectionClass, public std::vector<OtObject> {
public:
	OtArrayClass() {}

	// convert array to string
	operator std::wstring() {
		std::wstring result(L"[");
		bool first = true;

		for (auto const& entry : *this) {
			if (first) {
				first = false;

			} else {
				result += L",";
			}

			result += entry->repr();
		}

		result += L"]";
		return result;
	}

	// clear array and add all calling parameters
	OtObject init(OtObject, size_t count, OtObject* parameters) {
		clear();

		for (size_t c = 0; c < count; c++) {
			push_back(parameters[c]);
		}

		return getSharedPtr();
	}

	// support index operator
	class OtArrayReferenceClass : public OtInternalClass {
	public:
		OtArrayReferenceClass() {}
		OtArrayReferenceClass(OtArray a, size_t i) { array = a; index = i; }

		OtObject deref() { return array->operator[] (index); }
		OtObject assign(OtObject value) { array->operator[] (index) = value; return value; }

		// get type definition
		static OtType getMeta() {
			static OtType type = nullptr;

			if (!type) {
				type = OtTypeClass::create<OtArrayReferenceClass>(L"ArrayReference", OtInternalClass::getMeta());
				type->set(L"__deref__", OtFunctionCreate(&OtArrayReferenceClass::deref));
				type->set(L"__assign__", OtFunctionCreate(&OtArrayReferenceClass::assign));
			}

			return type;
		}

		// create a new object
		static OtObject create(OtArray a, size_t i) { return std::make_shared<OtArrayReferenceClass>(a, i)->setType(getMeta()); }

	private:
		OtArray array;
		size_t index;
	};

	OtObject index(size_t index) { return OtArrayReferenceClass::create(OtTypeClass::cast<OtArrayClass>(getSharedPtr()), index); }

	// support iterator
	class OtArrayIteratorClass : public OtInternalClass {
	public:
		OtArrayIteratorClass() {}
		OtArrayIteratorClass(OtArray a) { array = a; index = 0; }

		bool end() { return index == array->size(); }
		OtObject next() { return array->operator[](index++); }

		// get type definition
		static OtType getMeta() {
			static OtType type = nullptr;

			if (!type) {
				type = OtTypeClass::create<OtArrayIteratorClass>(L"ArrayIterator", OtInternalClass::getMeta());
				type->set(L"__end__", OtFunctionCreate(&OtArrayIteratorClass::end));
				type->set(L"__next__", OtFunctionCreate(&OtArrayIteratorClass::next));
			}

			return type;
		}

		// create a new object
		static OtObject create(OtArray a) { return std::make_shared<OtArrayIteratorClass>(a)->setType(getMeta()); }

	private:
		OtArray array;
		size_t index;
	};

	OtObject iterate() { return OtArrayIteratorClass::create(OtTypeClass::cast<OtArrayClass>(getSharedPtr())); }

	OtObject add(OtObject value) {
		OtArray result = create();

		for (auto& it : *this) {
			result->push_back(it);
		}

		result->append(value);
		return result;
	}

	OtObject contains(OtObject value) {
		bool result = false;

		for (auto it = begin(); it != end() && !result; it++) {
			result = value->method(L"__eq__", OtObjectClass::create(), 1, &(*it))->operator bool();
		}

		return OtBooleanClass::create(result);
	}

	size_t mySize() {
		return size();
	}

	long find(OtObject value) {
		long result = -1;

		for (size_t i = 0; i < size() && result == -1; i++) {
			if (value->method(L"__eq__", OtObjectClass::create(), 1, &(operator[] (i)))->operator bool()) {
				result = i;
			}
		}

		return result;
	}

	OtObject clone() {
		OtArray result = create();
		for (auto& it : *this) {
			result->push_back(it);
		}

		return result;
	}

	OtObject append(OtObject value) {
		push_back(value);
		return getSharedPtr();
	}

	OtObject insert(size_t index, OtObject value) {
		std::vector<OtObject>::insert(begin() + index, value);
		return getSharedPtr();
	}

	OtObject erase(size_t index) {
		std::vector<OtObject>::erase(begin() + index);
		return getSharedPtr();
	}

	OtObject eraseMultiple(size_t index1, size_t index2) {
		std::vector<OtObject>::erase(begin() + index1, begin() + index2);
		return getSharedPtr();
	}

	OtObject push(OtObject value) {
		push_back(value); return value;
		return getSharedPtr();
	}

	OtObject pop() {
		OtObject value = back();
		pop_back();
		return value;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtArrayClass>(L"Array", OtCollectionClass::getMeta());

			type->set(L"__init__", OtFunctionClass::create(&OtArrayClass::init));
			type->set(L"__index__", OtFunctionCreate(&OtArrayClass::index));
			type->set(L"__iter__", OtFunctionCreate(&OtArrayClass::iterate));
			type->set(L"__add__", OtFunctionCreate(&OtArrayClass::add));
			type->set(L"__contains__", OtFunctionCreate(&OtArrayClass::contains));

			type->set(L"size", OtFunctionCreate(&OtArrayClass::mySize));
			type->set(L"find", OtFunctionCreate(&OtArrayClass::find));

			type->set(L"clone", OtFunctionCreate(&OtArrayClass::clone));
			type->set(L"clear", OtFunctionCreate(&OtArrayClass::clear));

			type->set(L"append", OtFunctionCreate(&OtArrayClass::append));
			type->set(L"insert", OtFunctionCreate(&OtArrayClass::insert));

			type->set(L"erase", OtFunctionCreate(&OtArrayClass::erase));
			type->set(L"eraseMultiple", OtFunctionCreate(&OtArrayClass::eraseMultiple));

			type->set(L"push", OtFunctionCreate(&OtArrayClass::push));
			type->set(L"pop", OtFunctionCreate(&OtArrayClass::pop));
		}

		return type;
	}

	// create a new object
	static OtArray create() {
		OtArray array = std::make_shared<OtArrayClass>();
		array->setType(getMeta());
		return array;
	}

	static OtArray create(size_t count, OtObject* values) {
		OtArray array = create();

		for (size_t c = 0; c < count; c++) {
			array->push_back(values[c]);
		}

		return array;
	}
};
