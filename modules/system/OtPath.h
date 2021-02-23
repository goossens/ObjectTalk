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

class OtPathClass : public OtSystemClass {
public:
	// constructors
	OtPathClass() = default;
	OtPathClass(const char* p) : path(p) {}
	OtPathClass(const std::string& p) : path(p) {}
	OtPathClass(const std::filesystem::path& p) : path(p) {}

	// convert to string
	operator std::string() { return path.string(); }

	// initialize path
	OtObject init(OtObject, size_t count, OtObject* parameters) {
		if (count != 1) {
			OT_EXCEPT("Path initializer expected 1 parameter not [%d]", count);
		}

		path = parameters[0]->operator std::string();
		return getSharedPtr();
	}

	// modifiers
	void clear() { path.clear(); }
	void removeFilename() { path.remove_filename(); }
	void replaceFilename(OtObject name) { path.replace_filename(name->operator std::string()); }
	void replaceExtension(OtObject name) { path.replace_extension(name->operator std::string()); }

	// operators
	OtObject add(OtObject operand) { return OtPathClass::create(path / operand->operator std::string()); }
	bool equal(OtObject operand) { return path == operand->operator std::string(); }
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

	OtObject relative(OtObject name) { return create(path.lexically_relative(name->operator std::string())); }

	// support iterator
	class OtPathIteratorClass;
	typedef std::shared_ptr<OtPathIteratorClass> OtPathIterator;

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
				type = OtTypeClass::create<OtPathIteratorClass>("PathIterator", OtInternalClass::getMeta());
				type->set("__end__", OtFunctionCreate(&OtPathIteratorClass::end));
				type->set("__next__", OtFunctionCreate(&OtPathIteratorClass::next));
			}

			return type;
		}

		// create a new object
		static OtPathIterator create(OtPath path) {
			OtPathIterator iterator = std::make_shared<OtPathIteratorClass>(path);
			iterator->setType(getMeta());
			return iterator;
		}

	private:
		OtPath path;
		std::filesystem::path::iterator iterator;
		std::filesystem::path::iterator last;
	};

	OtObject iterate() {
		return OtPathIteratorClass::create(cast<OtPathClass>());
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtPathClass>("Path", OtSystemClass::getMeta());

			type->set("__init__", OtFunctionClass::create(&OtPathClass::init));
			type->set("__iter__", OtFunctionCreate(&OtPathClass::iterate));
			type->set("__add__", OtFunctionCreate(&OtPathClass::add));

			type->set("__eq__", OtFunctionCreate(&OtPathClass::equal));
			type->set("__ne__", OtFunctionCreate(&OtPathClass::notEqual));

			type->set("clear", OtFunctionCreate(&OtPathClass::clear));
			type->set("removeFilename", OtFunctionCreate(&OtPathClass::removeFilename));
			type->set("replaceFilename", OtFunctionCreate(&OtPathClass::replaceFilename));
			type->set("replaceExtension", OtFunctionCreate(&OtPathClass::replaceExtension));

			type->set("isEmpty", OtFunctionCreate(&OtPathClass::isEmpty));
			type->set("isAbsolute", OtFunctionCreate(&OtPathClass::isAbsolute));
			type->set("isRelative", OtFunctionCreate(&OtPathClass::isRelative));

			type->set("hasRootPath", OtFunctionCreate(&OtPathClass::hasRootPath));
			type->set("hasRootName", OtFunctionCreate(&OtPathClass::hasRootName));
			type->set("hasRootDirectory", OtFunctionCreate(&OtPathClass::hasRootDirectory));
			type->set("hasRelativePath", OtFunctionCreate(&OtPathClass::hasRelativePath));
			type->set("hasParentPath", OtFunctionCreate(&OtPathClass::hasParentPath));
			type->set("hasFilename", OtFunctionCreate(&OtPathClass::hasFilename));
			type->set("hasStem", OtFunctionCreate(&OtPathClass::hasStem));
			type->set("hasExtension", OtFunctionCreate(&OtPathClass::hasExtension));

			type->set("rootName", OtFunctionCreate(&OtPathClass::rootName));
			type->set("rootDirectory", OtFunctionCreate(&OtPathClass::rootDirectory));
			type->set("rootPath", OtFunctionCreate(&OtPathClass::rootPath));
			type->set("directory", OtFunctionCreate(&OtPathClass::directory));
			type->set("filename", OtFunctionCreate(&OtPathClass::filename));
			type->set("stem", OtFunctionCreate(&OtPathClass::stem));
			type->set("extension", OtFunctionCreate(&OtPathClass::extension));

			type->set("relative", OtFunctionCreate(&OtPathClass::relative));
		}

		return type;
	}

	// create a new object
	static OtPath create(const char* value) {
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

	static OtPath create(const std::string& value) {
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
