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
//	OtPath
//

class OtPathClass;
typedef std::shared_ptr<OtPathClass> OtPath;


//
//	OtPathClass
//

class OtPathClass : public OtSystemClass {
public:
	// constructors
	OtPathClass() {}
	OtPathClass(const char* p) { path = p; }
	OtPathClass(const std::wstring& p) { path = p; }
	OtPathClass(const std::filesystem::path& p) { path = p; }

	// convert to string
	operator std::wstring() { return path.wstring(); }

	// initialize path
	OtObject init(OtObject, size_t count, OtObject* parameters) {
		if (count != 1) {
			OT_EXCEPT(L"Path initializer expected 1 parameter not [%d]", count);
		}

		path = parameters[0]->operator std::wstring();
		return getSharedPtr();
	}

	// operators
	OtObject add(OtObject operand) { return OtPathClass::create(path / operand->operator std::wstring()); }
	bool equal(OtObject operand) { return path == operand->operator std::wstring(); }
	bool notEqual(OtObject operand) { return path != operand->operator std::wstring(); }
	bool greaterThan(OtObject operand) { return path > operand->operator std::wstring(); }
	bool lessThan(OtObject operand) { return path < operand->operator std::wstring(); }
	bool greaterEqual(OtObject operand) { return path >= operand->operator std::wstring(); }
	bool lessEqual(OtObject operand) { return path <= operand->operator std::wstring(); }

	// get path elements
	OtObject directory() { return create(path.parent_path()); }
	OtObject filename() { return create(path.filename()); }
	OtObject stem() { return create(path.stem()); }
	OtObject extension() { return create(path.extension()); }

	// support iterator
	class OtPathIteratorClass : public OtInternalClass {
	public:
		OtPathIteratorClass() {}
		OtPathIteratorClass(OtPath p) { path = p; iterator = p->path.begin(); last = p->path.end(); }

		bool end() { return iterator == last; }
		OtObject next() { return OtPathClass::create(*(iterator++)); }

		// get type definition
		static OtType getMeta() {
			static OtType type = nullptr;

			if (!type) {
				type = OtTypeClass::create<OtPathIteratorClass>(L"PathIterator", OtInternalClass::getMeta());
				type->set(L"__end__", OtFunctionCreate(&OtPathIteratorClass::end));
				type->set(L"__next__", OtFunctionCreate(&OtPathIteratorClass::next));
			}

			return type;
		}

		// create a new object
		static OtObject create(OtPath path) { return std::make_shared<OtPathIteratorClass>(path)->setType(getMeta()); }

	private:
		OtPath path;
		std::filesystem::path::iterator iterator;
		std::filesystem::path::iterator last;
	};

	OtObject iterate() { return OtPathIteratorClass::create(OtTypeClass::cast<OtPathClass>(getSharedPtr())); }

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtPathClass>(L"Path", OtSystemClass::getMeta());

			type->set(L"__init__", OtFunctionClass::create(&OtPathClass::init));
			type->set(L"__iter__", OtFunctionCreate(&OtPathClass::iterate));
			type->set(L"__add__", OtFunctionCreate(&OtPathClass::add));

			type->set(L"__eq__", OtFunctionCreate(&OtPathClass::equal));
			type->set(L"__ne__", OtFunctionCreate(&OtPathClass::notEqual));
			type->set(L"__gt__", OtFunctionCreate(&OtPathClass::greaterThan));
			type->set(L"__lt__", OtFunctionCreate(&OtPathClass::lessThan));
			type->set(L"__ge__", OtFunctionCreate(&OtPathClass::greaterEqual));
			type->set(L"__le__", OtFunctionCreate(&OtPathClass::lessEqual));

			type->set(L"directory", OtFunctionCreate(&OtPathClass::directory));
			type->set(L"filename", OtFunctionCreate(&OtPathClass::filename));
			type->set(L"stem", OtFunctionCreate(&OtPathClass::stem));
			type->set(L"extension", OtFunctionCreate(&OtPathClass::extension));
		}

		return type;
	}

	// create a new object
	static OtPath create(const char* value) {
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

	static OtPath create(const std::wstring& value) {
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

	static OtPath create(const std::filesystem::path& value) {
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

private:
	std::filesystem::path path;
};
