//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>
#include <filesystem>

#include "ot/exception.h"
#include "ot/function.h"
#include "ot/internal.h"

#include "path.h"


//
//	OtPathClass::init
//

OtObject OtPathClass::init(OtContext, size_t count, OtObject* parameters) {
	if (count != 1) {
		OT_EXCEPT("Path initializer expected 1 parameter not [%d]", count);
	}

	path = parameters[0]->operator std::string();
	return getSharedPtr();
}


//
//	OtPathIterator
//
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


//
//	OtPathClass::iterate
//

OtObject OtPathClass::iterate() {
	return OtPathIteratorClass::create(cast<OtPathClass>());
}


//
//	OtPathClass::getMeta
//

OtType OtPathClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtPathClass>("Path", OtSystemClass::getMeta());

		type->set("__init__", OtFunctionClass::create(&OtPathClass::init));
		type->set("__iter__", OtFunctionClass::create(&OtPathClass::iterate));
		type->set("__add__", OtFunctionClass::create(&OtPathClass::join));
		type->set("__div__", OtFunctionClass::create(&OtPathClass::join));

		type->set("__eq__", OtFunctionClass::create(&OtPathClass::equal));
		type->set("__ne__", OtFunctionClass::create(&OtPathClass::notEqual));

		type->set("clear", OtFunctionClass::create(&OtPathClass::clear));
		type->set("removeFilename", OtFunctionClass::create(&OtPathClass::removeFilename));
		type->set("replaceFilename", OtFunctionClass::create(&OtPathClass::replaceFilename));
		type->set("replaceExtension", OtFunctionClass::create(&OtPathClass::replaceExtension));

		type->set("exists", OtFunctionClass::create(&OtPathClass::exists));
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

//
//	OtPathClass::create
//

OtPath OtPathClass::create(const char* value) {
	OtPath path = std::make_shared<OtPathClass>(value);
	path->setType(getMeta());
	return path;
}

OtPath OtPathClass::create(const std::string& value) {
	OtPath path = std::make_shared<OtPathClass>(value);
	path->setType(getMeta());
	return path;
}

OtPath OtPathClass::create(const std::filesystem::path& value) {
	OtPath path = std::make_shared<OtPathClass>(value);
	path->setType(getMeta());
	return path;
}
