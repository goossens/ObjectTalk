//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


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
	OtObject init(OtContext, size_t count, OtObject* parameters) {
		if (count != 1) {
			throw OtException(OtFormat("Path initializer expected 1 parameter not [%d]", count));
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
				type->set("__end__", OtFunctionClass::create(&OtPathIteratorClass::end));
				type->set("__next__", OtFunctionClass::create(&OtPathIteratorClass::next));
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
			type->set("__iter__", OtFunctionClass::create(&OtPathClass::iterate));
			type->set("__add__", OtFunctionClass::create(&OtPathClass::add));

			type->set("__eq__", OtFunctionClass::create(&OtPathClass::equal));
			type->set("__ne__", OtFunctionClass::create(&OtPathClass::notEqual));

			type->set("clear", OtFunctionClass::create(&OtPathClass::clear));
			type->set("removeFilename", OtFunctionClass::create(&OtPathClass::removeFilename));
			type->set("replaceFilename", OtFunctionClass::create(&OtPathClass::replaceFilename));
			type->set("replaceExtension", OtFunctionClass::create(&OtPathClass::replaceExtension));

			type->set("isEmpty", OtFunctionClass::create(&OtPathClass::isEmpty));
			type->set("isAbsolute", OtFunctionClass::create(&OtPathClass::isAbsolute));
			type->set("isRelative", OtFunctionClass::create(&OtPathClass::isRelative));

			type->set("hasRootPath", OtFunctionClass::create(&OtPathClass::hasRootPath));
			type->set("hasRootName", OtFunctionClass::create(&OtPathClass::hasRootName));
			type->set("hasRootDirectory", OtFunctionClass::create(&OtPathClass::hasRootDirectory));
			type->set("hasRelativePath", OtFunctionClass::create(&OtPathClass::hasRelativePath));
			type->set("hasParentPath", OtFunctionClass::create(&OtPathClass::hasParentPath));
			type->set("hasFilename", OtFunctionClass::create(&OtPathClass::hasFilename));
			type->set("hasStem", OtFunctionClass::create(&OtPathClass::hasStem));
			type->set("hasExtension", OtFunctionClass::create(&OtPathClass::hasExtension));

			type->set("rootName", OtFunctionClass::create(&OtPathClass::rootName));
			type->set("rootDirectory", OtFunctionClass::create(&OtPathClass::rootDirectory));
			type->set("rootPath", OtFunctionClass::create(&OtPathClass::rootPath));
			type->set("directory", OtFunctionClass::create(&OtPathClass::directory));
			type->set("filename", OtFunctionClass::create(&OtPathClass::filename));
			type->set("stem", OtFunctionClass::create(&OtPathClass::stem));
			type->set("extension", OtFunctionClass::create(&OtPathClass::extension));

			type->set("relative", OtFunctionClass::create(&OtPathClass::relative));
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
