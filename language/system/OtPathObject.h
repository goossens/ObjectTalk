//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
//	OtPathObjectObject
//

class OtPathObjectClass;
using OtPathObject = OtObjectPointer<OtPathObjectClass>;

class OtPathObjectClass : public OtSystemClass {
public:
	// convert to string
	inline operator std::string() override { return path.string(); }

	// debugging support
	inline std::string describe() override { return "\"" + path.string() + "\""; }

	// initialize path
	void init(size_t count, OtObject* parameters);

	// modifiers
	inline void clear() { path.clear(); }
	inline void removeFilename() { path.remove_filename(); }
	inline void replaceFilename(OtObject name) { path.replace_filename(name->operator std::string()); }
	inline void replaceExtension(OtObject name) { path.replace_extension(name->operator std::string()); }

	// operators
	inline bool operator==(OtObject operand) override { return path == operand->operator std::string(); }
	inline bool operator<(OtObject operand) override { return path < operand->operator std::string(); }

	inline OtObject join(OtObject operand) { return OtPathObject::create(path / operand->operator std::string()); }
	inline bool equal(OtObject operand) { return path == operand->operator std::string(); }
	inline bool notEqual(OtObject operand) { return !equal(operand); }

	// check path status
	inline bool exists() { return std::filesystem::exists(path); }
	inline bool isEmpty() { return path.empty(); }
	inline bool isAbsolute() { return path.is_absolute(); }
	inline bool isRelative() { return path.is_relative(); }

	inline bool hasRootPath() { return path.has_root_path(); }
	inline bool hasRootName() { return path.has_root_name(); }
	inline bool hasRootDirectory() { return path.has_root_directory(); }
	inline bool hasRelativePath() { return path.has_relative_path(); }
	inline bool hasParentPath() { return path.has_parent_path(); }
	inline bool hasFilename() { return path.has_filename(); }
	inline bool hasStem() { return path.has_stem(); }
	inline bool hasExtension() { return path.has_extension(); }

	// get path elements
	inline OtObject rootName() { return OtPathObject::create(path.root_name()); }
	inline OtObject rootDirectory() { return OtPathObject::create(path.root_directory()); }
	inline OtObject rootPath() { return OtPathObject::create(path.root_path()); }
	inline OtObject directory() { return OtPathObject::create(path.parent_path()); }
	inline OtObject filename() { return OtPathObject::create(path.filename()); }
	inline OtObject stem() { return OtPathObject::create(path.stem()); }
	inline OtObject extension() { return OtPathObject::create(path.extension()); }

	// get canonical form of path (absolute, no dots, resolved symbolic links)
	inline OtObject canonical() { return OtPathObject::create(std::filesystem::canonical(path)); }

	// get relative path to other location
	inline OtObject relative(OtObject name) { return OtPathObject::create(path.lexically_relative(name->operator std::string())); }

	// iterate through path elements
	OtObject iterate();

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtPathObjectClass>;
	OtPathObjectClass() = default;
	OtPathObjectClass(const char* p) : path(p) {}
	OtPathObjectClass(const std::string& p) : path(p) {}
	OtPathObjectClass(const std::filesystem::path& p) : path(p) {}

private:
	friend class OtPathIteratorClass;
	std::filesystem::path path;
};
