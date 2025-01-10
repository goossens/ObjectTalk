//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
	static inline bool isEmpty(const std::string& path) { return path.empty(); }
	static inline bool isUntitled(const std::string& path) { return OtText::startsWith(path, "untitled"); }

	static inline bool exists(const std::string& path) { return std::filesystem::exists(std::filesystem::path(path)); }
	static inline bool isRegularFile(const std::string& path) { return std::filesystem::is_regular_file(std::filesystem::path(path)); }
	static inline bool isDirectory(const std::string& path) { return std::filesystem::is_directory(std::filesystem::path(path)); }

	static inline bool isAbsolute(const std::string& path) { return std::filesystem::path(path).is_absolute(); }
	static inline bool isRelative(const std::string& path) { return std::filesystem::path(path).is_relative(); }

	static inline bool hasRootPath(const std::string& path) { return std::filesystem::path(path).has_root_path(); }
	static inline bool hasRootName(const std::string& path) { return std::filesystem::path(path).has_root_name(); }
	static inline bool hasRootDirectory(const std::string& path) { return std::filesystem::path(path).has_root_directory(); }
	static inline bool hasRelativePath(const std::string& path) { return std::filesystem::path(path).has_relative_path(); }
	static inline bool hasParentPath(const std::string& path) { return std::filesystem::path(path).has_parent_path(); }
	static inline bool hasFilename(const std::string& path) { return std::filesystem::path(path).has_filename(); }
	static inline bool hasStem(const std::string& path) { return std::filesystem::path(path).has_stem(); }
	static inline bool hasExtension(const std::string& path) { return std::filesystem::path(path).has_extension(); }

	static inline std::uintmax_t getFileSize(const std::string path) { return std::filesystem::file_size(std::filesystem::path(path)); }
	static inline std::string getParent(const std::string& path) { return std::filesystem::path(path).parent_path().string(); }
	static inline std::string getFilename(const std::string& path) { return std::filesystem::path(path).filename().string(); }
	static inline std::string getStem(const std::string& path) { return std::filesystem::path(path).stem().string(); }
	static inline std::string getExtension(const std::string& path) { return std::filesystem::path(path).extension().string(); }
	static inline std::string getCanonical(const std::string& path) { return std::filesystem::canonical(std::filesystem::path(path)).string(); }

	static std::string replaceExtension(const std::string path, const std::string& extension);
	static inline std::string join(const std::string& base, const std::string& tail) { return (std::filesystem::path(base) / std::filesystem::path(tail)).string(); }

	static std::string getExecutable();
	static inline std::string getCWD() { return std::filesystem::current_path().string(); }
	static inline void changeDirectory(const std::string& path) { std::filesystem::current_path(std::filesystem::path(path)); }
	static std::string getHomeDirectory();
	static std::string getDocumentsDirectory();
	static inline std::string getTmpDirectory() { return std::filesystem::temp_directory_path().string(); }
	static std::string getTmpFilename();

	static std::string getRelative(const std::string& path, std::string* basedir);
	static std::string getAbsolute(const std::string& path, std::string* basedir);
};
