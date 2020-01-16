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
//	OtDict
//

class OtDictClass;
typedef std::shared_ptr<OtDictClass> OtDict;


//
//	OtDictClass
//

class OtDictClass : public OtCollectionClass, public std::map<std::wstring, OtObject>
{
public:
	OtDictClass() {}

	// convert dictionary to string
	operator std::wstring()
	{
		std::wstring result(L"{");
		bool first = true;

		for (auto const& entry : *this)
		{
			if (first)
				first = false;

			else
				result += L",";

			result += L"\"" + entry.first + L"\":" + entry.second->repr();
		}

		result += L"}";
		return result;
	}

	// initializer
	OtObject init(OtObject, size_t count, OtObject* parameters)
	{
		// clear dictionary and add all calling parameters
		clear();

		for (size_t c = 0; c < count; c += 2)
			insert(std::make_pair((std::wstring) *parameters[c], parameters[c + 1]));

		return getSharedPtr();
	}

	// support index operator
	class OtDictReferenceClass : public OtInternalClass
	{
	public:
		OtDictReferenceClass() {}
		OtDictReferenceClass(OtDict d, const std::wstring& i) { dict = d; index = i; }

		OtObject deref() { return dict->operator[] (index); }
		OtObject assign(OtObject value) { dict->operator[] (index) = value; return value; }

		// get type definition
		static OtType getMeta()
		{
			static OtType type = nullptr;

			if (!type)
			{
				type = OtTypeClass::create<OtDictReferenceClass>(L"DictReference", OtInternalClass::getMeta());
				type->set(L"__deref__", OtFunctionCreate(&OtDictReferenceClass::deref));
				type->set(L"__assign__", OtFunctionCreate(&OtDictReferenceClass::assign));
			}

			return type;
		}

		// create a new object
		static OtObject create(OtDict a, const std::wstring& i) { return std::make_shared<OtDictReferenceClass>(a, i)->setType(getMeta()); }

	private:
		OtDict dict;
		std::wstring index;
	};

	OtObject index(const std::wstring& index) { return OtDictReferenceClass::create(OtTypeClass::cast<OtDictClass>(getSharedPtr()), index); }

	// get dictionary size
	size_t mySize()
	{
		return size();
	}

	// return dictionary clone
	OtObject clone()
	{
		OtDict result = create();
		for (auto& it : *this) result->insert(std::make_pair(it.first, it.second));
		return result;
	}

	// remove dictionary entry
	OtObject eraseEntry(const std::wstring& name)
	{
		OtObject value = operator[] (name);
		erase(name);
		return value;
	}

	// get array of dictionary names
	OtObject keys()
	{
		// create array of all keys in context
		OtArray array = OtArrayClass::create();

		for (auto const& entry : *this)
			array->push_back(OtStringClass::create(entry.first));

		return array;
	}

	// get array of dictionary values
	OtObject values()
	{
		// create array of all values in context
		OtArray array = OtArrayClass::create();

		for (auto const& entry : *this)
			array->push_back(entry.second);

		return array;
	}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtDictClass>(L"Dict", OtCollectionClass::getMeta());

			type->set(L"__init__", OtFunctionClass::create(&OtDictClass::init));
			type->set(L"__index__", OtFunctionCreate(&OtDictClass::index));

			type->set(L"size", OtFunctionCreate(&OtDictClass::mySize));

			type->set(L"clone", OtFunctionCreate(&OtDictClass::clone));
			type->set(L"clear", OtFunctionCreate(&OtDictClass::clear));
			type->set(L"erase", OtFunctionCreate(&OtDictClass::eraseEntry));

			type->set(L"keys", OtFunctionCreate(&OtDictClass::keys));
			type->set(L"values", OtFunctionCreate(&OtDictClass::values));
		}

		return type;
	}

	// create a new object
	static OtDict create()
	{
		OtDict dict = std::make_shared<OtDictClass>();
		dict->setType(getMeta());
		return dict;
	}

	static OtDict create(size_t count, OtObject* values)
	{
		OtDict dict = create();

		for (size_t c = 0; c < count; c += 2)
			dict->insert(std::make_pair((std::wstring) *values[c], values[c + 1]));

		return dict;
	}
};
