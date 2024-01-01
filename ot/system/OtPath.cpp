//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
	return OtPathIterator::create(OtPath(this));
}


//
//	OtPathClass::getMeta
//

OtType OtPathClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPathClass>("Path", OtSystemClass::getMeta());

		type->set("__init__", OtFunction::create(&OtPathClass::init));
		type->set("__iter__", OtFunction::create(&OtPathClass::iterate));
		type->set("__add__", OtFunction::create(&OtPathClass::join));
		type->set("__div__", OtFunction::create(&OtPathClass::join));

		type->set("__eq__", OtFunction::create(&OtPathClass::equal));
		type->set("__ne__", OtFunction::create(&OtPathClass::notEqual));

		type->set("clear", OtFunction::create(&OtPathClass::clear));
		type->set("removeFilename", OtFunction::create(&OtPathClass::removeFilename));
		type->set("replaceFilename", OtFunction::create(&OtPathClass::replaceFilename));
		type->set("replaceExtension", OtFunction::create(&OtPathClass::replaceExtension));

		type->set("exists", OtFunction::create(&OtPathClass::exists));
		type->set("isEmpty", OtFunction::create(&OtPathClass::isEmpty));
		type->set("isAbsolute", OtFunction::create(&OtPathClass::isAbsolute));
		type->set("isRelative", OtFunction::create(&OtPathClass::isRelative));

		type->set("hasRootPath", OtFunction::create(&OtPathClass::hasRootPath));
		type->set("hasRootName", OtFunction::create(&OtPathClass::hasRootName));
		type->set("hasRootDirectory", OtFunction::create(&OtPathClass::hasRootDirectory));
		type->set("hasRelativePath", OtFunction::create(&OtPathClass::hasRelativePath));
		type->set("hasParentPath", OtFunction::create(&OtPathClass::hasParentPath));
		type->set("hasFilename", OtFunction::create(&OtPathClass::hasFilename));
		type->set("hasStem", OtFunction::create(&OtPathClass::hasStem));
		type->set("hasExtension", OtFunction::create(&OtPathClass::hasExtension));

		type->set("rootName", OtFunction::create(&OtPathClass::rootName));
		type->set("rootDirectory", OtFunction::create(&OtPathClass::rootDirectory));
		type->set("rootPath", OtFunction::create(&OtPathClass::rootPath));
		type->set("directory", OtFunction::create(&OtPathClass::directory));
		type->set("filename", OtFunction::create(&OtPathClass::filename));
		type->set("stem", OtFunction::create(&OtPathClass::stem));
		type->set("extension", OtFunction::create(&OtPathClass::extension));

		type->set("relative", OtFunction::create(&OtPathClass::relative));
	}

	return type;
}
