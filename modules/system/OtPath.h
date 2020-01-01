//	ObjectTalk Scripting Language
//	Copyright 1993-2019 Johan A. Goossens
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

class OtPathClass : public OtSystemClass
{
public:
	// constructors
	OtPathClass() {}
	OtPathClass(const char* p) { path = p; }
	OtPathClass(const std::string& p) { path = p; }
	OtPathClass(const std::filesystem::path& p) { path = p; }

	// convert to string
	operator std::string() { return path.string(); }

	// initialize path
	OtObject init(OtObject, size_t count, OtObject* parameters)
	{
		if (count != 1)
			OT_EXCEPT("Path initializer expected 1 parameter not [%d]", count);

		path = parameters[0]->operator std::string();
		return getSharedPtr();
	}

	// operators
	OtObject add(OtObject operand) { return OtPathClass::create(path / operand->operator std::string()); }
	bool equal(OtObject operand) { return path == operand->operator std::string(); }
	bool notEqual(OtObject operand) { return path != operand->operator std::string(); }
	bool greaterThan(OtObject operand) { return path > operand->operator std::string(); }
	bool lessThan(OtObject operand) { return path < operand->operator std::string(); }
	bool greaterEqual(OtObject operand) { return path >= operand->operator std::string(); }
	bool lessEqual(OtObject operand) { return path <= operand->operator std::string(); }

	// get path elements
	OtObject directory() { return create(path.parent_path()); }
	OtObject filename() { return create(path.filename()); }
	OtObject stem() { return create(path.stem()); }
	OtObject extension() { return create(path.extension()); }

	// support iterator
	class OtPathIteratorClass : public OtInternalClass
	{
	public:
		OtPathIteratorClass() {}
		OtPathIteratorClass(OtPath p) { path = p; iterator = p->path.begin(); last = p->path.end(); }

		bool end() { return iterator == last; }
		OtObject next() { return OtPathClass::create(*(iterator++)); }

		// get type definition
		static OtType getMeta()
		{
			static OtType type = nullptr;

			if (!type)
			{
				type = OtTypeClass::create<OtPathIteratorClass>("PathIterator", OtInternalClass::getMeta());
				type->set("__end__", OtFunctionCreate(&OtPathIteratorClass::end));
				type->set("__next__", OtFunctionCreate(&OtPathIteratorClass::next));
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
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtPathClass>("Path", OtSystemClass::getMeta());

			type->set("__init__", OtFunctionClass::create(&OtPathClass::init));
			type->set("__iter__", OtFunctionCreate(&OtPathClass::iterate));
			type->set("__add__", OtFunctionCreate(&OtPathClass::add));

			type->set("__eq__", OtFunctionCreate(&OtPathClass::equal));
			type->set("__ne__", OtFunctionCreate(&OtPathClass::notEqual));
			type->set("__gt__", OtFunctionCreate(&OtPathClass::greaterThan));
			type->set("__lt__", OtFunctionCreate(&OtPathClass::lessThan));
			type->set("__ge__", OtFunctionCreate(&OtPathClass::greaterEqual));
			type->set("__le__", OtFunctionCreate(&OtPathClass::lessEqual));

			type->set("directory", OtFunctionCreate(&OtPathClass::directory));
			type->set("filename", OtFunctionCreate(&OtPathClass::filename));
			type->set("stem", OtFunctionCreate(&OtPathClass::stem));
			type->set("extension", OtFunctionCreate(&OtPathClass::extension));
		}

		return type;
	}

	// create a new object
	static OtPath create(const char* value)
	{
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

	static OtPath create(const std::string& value)
	{
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

	static OtPath create(const std::filesystem::path& value)
	{
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

private:
	std::filesystem::path path;
};
