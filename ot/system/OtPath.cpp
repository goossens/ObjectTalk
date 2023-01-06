//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>
#include <filesystem>

#include "OtException.h"
#include "OtFunction.h"
#include "OtPath.h"
#include "OtPathIterator.h"


//
//	OtPathClass::init
//

void OtPathClass::init(size_t count, OtObject* parameters) {
	if (count >= 1) {
		path = parameters[0]->operator std::string();

		for (auto c = 1; c < count; c++) {
			path /= parameters[c]->operator std::string();
		}
	}
}


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
	static OtType type;

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
