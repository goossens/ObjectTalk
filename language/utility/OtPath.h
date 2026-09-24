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
#include <string>

#include "OtText.h"


class OtPath {
public:
	static std::string toString(const std::filesystem::path& path);
	static std::filesystem::path toPath(const std::string& string);

	static inline bool isEmpty(const std::string& path) { return path.empty(); }
	static inline bool isUntitled(const std::string& path) { return OtText::startsWith(path, "untitled"); }

	static inline bool exists(const std::string& path) { return std::filesystem::exists(toPath(path)); }
	static inline bool isRegularFile(const std::string& path) { return std::filesystem::is_regular_file(toPath(path)); }
	static inline bool isDirectory(const std::string& path) { return std::filesystem::is_directory(toPath(path)); }

	static inline bool isAbsolute(const std::string& path) { return toPath(path).is_absolute(); }
	static inline bool isRelative(const std::string& path) { return toPath(path).is_relative(); }

	static inline bool hasRootPath(const std::string& path) { return toPath(path).has_root_path(); }
	static inline bool hasRootName(const std::string& path) { return toPath(path).has_root_name(); }
	static inline bool hasRootDirectory(const std::string& path) { return toPath(path).has_root_directory(); }
	static inline bool hasRelativePath(const std::string& path) { return toPath(path).has_relative_path(); }
	static inline bool hasParentPath(const std::string& path) { return toPath(path).has_parent_path(); }
	static inline bool hasFilename(const std::string& path) { return toPath(path).has_filename(); }
	static inline bool hasStem(const std::string& path) { return toPath(path).has_stem(); }
	static inline bool hasExtension(const std::string& path) { return toPath(path).has_extension(); }

	static inline size_t getFileSize(const std::string& path) { return static_cast<size_t>(std::filesystem::file_size(toPath(path))); }
	static inline std::string getParent(const std::string& path) { return toString(toPath(path).parent_path()); }
	static inline std::string getFilename(const std::string& path) { return toString(toPath(path).filename()); }
	static inline std::string getStem(const std::string& path) { return toString(toPath(path).stem()); }
	static inline std::string getExtension(const std::string& path) { return toString(toPath(path).extension()); }
	static inline std::string getCanonical(const std::string& path) { return toString(std::filesystem::canonical(toPath(path))); }

	static std::string replaceExtension(const std::string& path, const std::string& extension);
	static inline std::string join(const std::string& base, const std::string& tail) { return toString(toPath(base) / toPath(tail)); }

	static std::string getExecutable();
	static inline std::string getCWD() { return toString(std::filesystem::current_path()); }
	static inline void changeDirectory(const std::string& path) { std::filesystem::current_path(toPath(path)); }
	static std::string getHomeDirectory();
	static std::string getDocumentsDirectory();
	static std::string getPreferencesDirectory();
	static inline std::string getTmpDirectory() { return toString(std::filesystem::temp_directory_path()); }
	static std::string getTmpFilename();

	static std::string getRelative(const std::string& path, std::string* basedir);
	static std::string getAbsolute(const std::string& path, std::string* basedir);
};
