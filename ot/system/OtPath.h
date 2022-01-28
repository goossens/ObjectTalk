//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>

#include "OtSystem.h"


//
//	OtPath
//

class OtPathClass;
typedef std::shared_ptr<OtPathClass> OtPath;

class OtPathClass : public OtSystemClass {
	friend class OtPathIteratorClass;

public:
	// constructors
	OtPathClass() = default;
	OtPathClass(const char* p) : path(p) {}
	OtPathClass(const std::string& p) : path(p) {}
	OtPathClass(const std::filesystem::path& p) : path(p) {}

	// convert to string
	operator std::string() { return path.string(); }

	// initialize path
	OtObject init(size_t count, OtObject* parameters);

	// modifiers
	void clear() { path.clear(); }
	void removeFilename() { path.remove_filename(); }
	void replaceFilename(OtObject name) { path.replace_filename(name->operator std::string()); }
	void replaceExtension(OtObject name) { path.replace_extension(name->operator std::string()); }

	// operators
	bool operator == (OtObject operand) { return path == operand->operator std::string(); }
	bool operator < (OtObject operand) { return path < operand->operator std::string(); }

	OtObject join(OtObject operand) { return OtPathClass::create(path / operand->operator std::string()); }
	bool equal(OtObject operand) { return path == operand->operator std::string(); }
	bool notEqual(OtObject operand) { return !equal(operand); }

	// check path status
	bool exists() { return std::filesystem::exists(path); }
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

	// get relative path to other location
	OtObject relative(OtObject name) { return create(path.lexically_relative(name->operator std::string())); }

	// iterate through path elements
	OtObject iterate();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtPath create(const char* value);
	static OtPath create(const std::string& value);
	static OtPath create(const std::filesystem::path& value);

private:
	std::filesystem::path path;
};
