//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
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
	OtPathClass() = default;
	OtPathClass(const char* p) : path(p) {}
	OtPathClass(const std::wstring& p) : path(p) {}
	OtPathClass(const std::filesystem::path& p) : path(p) {}

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

	// modifiers
	void clear() { path.clear(); }
	void removeFilename() { path.remove_filename(); }
	void replaceFilename(OtObject name) { path.replace_filename(name->operator std::wstring()); }
	void replaceExtension(OtObject name) { path.replace_extension(name->operator std::wstring()); }

	// operators
	OtObject add(OtObject operand) { return OtPathClass::create(path / operand->operator std::wstring()); }
	bool equal(OtObject operand) { return path == operand->operator std::wstring(); }
	bool notEqual(OtObject operand) { return !equal(operand); }

	// check path status
	bool isEmpty() { return path.empty(); }
	bool isAbsolute() { return path.is_absolute(); }
	bool isRelative() { return path.is_relative(); }

	bool hasRootPath() { return path.has_root_path(); }
	bool hasRootName() { return path.has_root_name(); }
	bool hasRootDirectory() { return path.has_root_directory(); }
	bool hasRelativePath() { return path.has_relative_path(); }
	bool hasParentPath() { return path.has_parent_path(); }
	bool hasFilename() { return path.has_filename(); }
	bool hasStem() { return path.has_stem(); }
	bool hasExtension() { return path.has_extension(); }

	// get path elements
	OtObject rootName() { return create(path.root_name()); }
	OtObject rootDirectory() { return create(path.root_directory()); }
	OtObject rootPath() { return create(path.root_path()); }
	OtObject directory() { return create(path.parent_path()); }
	OtObject filename() { return create(path.filename()); }
	OtObject stem() { return create(path.stem()); }
	OtObject extension() { return create(path.extension()); }

	OtObject relative(OtObject name) { return create(path.lexically_relative(name->operator std::wstring())); }

	// support iterator
	class OtPathIteratorClass : public OtInternalClass {
	public:
		OtPathIteratorClass() = default;
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

			type->set(L"clear", OtFunctionCreate(&OtPathClass::clear));
			type->set(L"removeFilename", OtFunctionCreate(&OtPathClass::removeFilename));
			type->set(L"replaceFilename", OtFunctionCreate(&OtPathClass::replaceFilename));
			type->set(L"replaceExtension", OtFunctionCreate(&OtPathClass::replaceExtension));

			type->set(L"isEmpty", OtFunctionCreate(&OtPathClass::isEmpty));
			type->set(L"isAbsolute", OtFunctionCreate(&OtPathClass::isAbsolute));
			type->set(L"isRelative", OtFunctionCreate(&OtPathClass::isRelative));

			type->set(L"hasRootPath", OtFunctionCreate(&OtPathClass::hasRootPath));
			type->set(L"hasRootName", OtFunctionCreate(&OtPathClass::hasRootName));
			type->set(L"hasRootDirectory", OtFunctionCreate(&OtPathClass::hasRootDirectory));
			type->set(L"hasRelativePath", OtFunctionCreate(&OtPathClass::hasRelativePath));
			type->set(L"hasParentPath", OtFunctionCreate(&OtPathClass::hasParentPath));
			type->set(L"hasFilename", OtFunctionCreate(&OtPathClass::hasFilename));
			type->set(L"hasStem", OtFunctionCreate(&OtPathClass::hasStem));
			type->set(L"hasExtension", OtFunctionCreate(&OtPathClass::hasExtension));

			type->set(L"rootName", OtFunctionCreate(&OtPathClass::rootName));
			type->set(L"rootDirectory", OtFunctionCreate(&OtPathClass::rootDirectory));
			type->set(L"rootPath", OtFunctionCreate(&OtPathClass::rootPath));
			type->set(L"directory", OtFunctionCreate(&OtPathClass::directory));
			type->set(L"filename", OtFunctionCreate(&OtPathClass::filename));
			type->set(L"stem", OtFunctionCreate(&OtPathClass::stem));
			type->set(L"extension", OtFunctionCreate(&OtPathClass::extension));

			type->set(L"", OtFunctionCreate(&OtPathClass::relative));
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
