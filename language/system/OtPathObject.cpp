//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <string>

#include "OtFunction.h"
#include "OtPathObject.h"
#include "OtPathIterator.h"


//
//	OtPathObjectClass::init
//

void OtPathObjectClass::init(size_t count, OtObject* parameters) {
	if (count >= 1) {
		path = parameters[0]->operator std::string();

		for (size_t c = 1; c < count; c++) {
			path /= parameters[c]->operator std::string();
		}
	}
}


//
//	OtPathObjectClass::iterate
//

OtObject OtPathObjectClass::iterate() {
	return OtPathIterator::create(OtPathObject(this));
}


//
//	OtPathObjectClass::getMeta
//

OtType OtPathObjectClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtPathObjectClass>("Path", OtSystemClass::getMeta());

		type->set("__init__", OtFunction::create(&OtPathObjectClass::init));
		type->set("__iter__", OtFunction::create(&OtPathObjectClass::iterate));
		type->set("__add__", OtFunction::create(&OtPathObjectClass::join));
		type->set("__div__", OtFunction::create(&OtPathObjectClass::join));

		type->set("__eq__", OtFunction::create(&OtPathObjectClass::equal));
		type->set("__ne__", OtFunction::create(&OtPathObjectClass::notEqual));

		type->set("join", OtFunction::create(&OtPathObjectClass::join));
		type->set("clear", OtFunction::create(&OtPathObjectClass::clear));
		type->set("removeFilename", OtFunction::create(&OtPathObjectClass::removeFilename));
		type->set("replaceFilename", OtFunction::create(&OtPathObjectClass::replaceFilename));
		type->set("replaceExtension", OtFunction::create(&OtPathObjectClass::replaceExtension));

		type->set("exists", OtFunction::create(&OtPathObjectClass::exists));
		type->set("isEmpty", OtFunction::create(&OtPathObjectClass::isEmpty));
		type->set("isAbsolute", OtFunction::create(&OtPathObjectClass::isAbsolute));
		type->set("isRelative", OtFunction::create(&OtPathObjectClass::isRelative));

		type->set("hasRootPath", OtFunction::create(&OtPathObjectClass::hasRootPath));
		type->set("hasRootName", OtFunction::create(&OtPathObjectClass::hasRootName));
		type->set("hasRootDirectory", OtFunction::create(&OtPathObjectClass::hasRootDirectory));
		type->set("hasRelativePath", OtFunction::create(&OtPathObjectClass::hasRelativePath));
		type->set("hasParentPath", OtFunction::create(&OtPathObjectClass::hasParentPath));
		type->set("hasFilename", OtFunction::create(&OtPathObjectClass::hasFilename));
		type->set("hasStem", OtFunction::create(&OtPathObjectClass::hasStem));
		type->set("hasExtension", OtFunction::create(&OtPathObjectClass::hasExtension));

		type->set("rootName", OtFunction::create(&OtPathObjectClass::rootName));
		type->set("rootDirectory", OtFunction::create(&OtPathObjectClass::rootDirectory));
		type->set("rootPath", OtFunction::create(&OtPathObjectClass::rootPath));
		type->set("directory", OtFunction::create(&OtPathObjectClass::directory));
		type->set("filename", OtFunction::create(&OtPathObjectClass::filename));
		type->set("stem", OtFunction::create(&OtPathObjectClass::stem));
		type->set("extension", OtFunction::create(&OtPathObjectClass::extension));

		type->set("canonical", OtFunction::create(&OtPathObjectClass::canonical));
		type->set("relative", OtFunction::create(&OtPathObjectClass::relative));
	}

	return type;
}
