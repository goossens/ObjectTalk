//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <random>
#include <sstream>
#include <string>

#include "OtLibuv.h"
#include "OtText.h"

#include "nlohmann/json.hpp"


//
//	OtPathIsVirtual
//

static inline bool OtPathIsVirtual(const std::string& path) {
	return OtText::startsWith(path, "virtual:");
}


//
//	OtPathIsUntitled
//

static inline bool OtPathIsUntitled(const std::string& path) {
	return OtText::startsWith(path, "untitled");
}


//
//	OtPathIsVirtual
//

static inline std::string OtPathStripVirtual(const std::string& path) {
	return OtPathIsVirtual(path) ? OtText::from(path, 8) : path;
}


//
//	OtPathExists
//

static inline bool OtPathExists(const std::string& path) {
	return OtPathIsVirtual(path) ? false : std::filesystem::exists(std::filesystem::path(path));
}


//
//	OtPathIsRegularFile
//

static inline bool OtPathIsRegularFile(const std::string& path) {
	return OtPathIsVirtual(path) ? false : std::filesystem::is_regular_file(std::filesystem::path(path));
}


//
//	OtPathIsDirectory
//

static inline bool OtPathIsDirectory(const std::string& path) {
	return OtPathIsVirtual(path) ? false : std::filesystem::is_directory(std::filesystem::path(path));
}


//
//	OtPathGetFileSize
//

static inline std::uintmax_t OtPathGetFileSize(const std::string path) {
	return OtPathIsVirtual(path) ? 0 : std::filesystem::file_size(std::filesystem::path(path));
}


//
//	OtPathGetParent
//

static inline std::string OtPathGetParent(const std::string& path) {
	return OtPathIsVirtual(path) ? OtPathGetParent(OtPathStripVirtual(path)) : std::filesystem::path(path).parent_path().string();
}


//
//	OtPathGetFilename
//

static inline std::string OtPathGetFilename(const std::string& path) {
	return OtPathIsVirtual(path) ? OtPathGetFilename(OtPathStripVirtual(path)) : std::filesystem::path(path).filename().string();
}


//
//	OtPathGetStem
//

static inline std::string OtPathGetStem(const std::string& path) {
	return OtPathIsVirtual(path) ? OtPathGetStem(OtPathStripVirtual(path)) : std::filesystem::path(path).stem().string();
}


//
//	OtPathGetExtension
//

static inline std::string OtPathGetExtension(const std::string& path) {
	return OtPathIsVirtual(path) ? OtPathGetExtension(OtPathStripVirtual(path)) : std::filesystem::path(path).extension().string();
}


//
//	OtPathReplaceExtension
//

static inline std::string OtPathReplaceExtension(const std::string path, const std::string& extension) {
	auto work = std::filesystem::path(OtPathIsVirtual(path) ? OtPathStripVirtual(path) : path);
	work.replace_extension(extension);
	return (OtPathIsVirtual(path) ? "virtual:" : "") + work.string();
}


//
//	OtPathJoin
//

static inline std::string OtPathJoin(const std::string& base, const std::string& tail) {
	return OtPathIsVirtual(tail)
		? tail
		: (std::filesystem::path(base) / std::filesystem::path(tail)).string();
}


//
//	OtPathGetExecutable
//

static inline std::string OtPathGetExecutable() {
	// figure out where we live
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_exepath(buffer, &length);
	UV_CHECK_ERROR("uv_exepath", status);
	buffer[length] = 0;
	return buffer;
}


//
//	OtPathGetCurrentWorkingDirectory
//

static inline std::string OtPathGetCurrentWorkingDirectory() {
	return std::filesystem::current_path().string();
}


//
//	OtPathChangeDirectory
//

static inline void OtPathChangeDirectory(const std::string& path) {
	std::filesystem::current_path(std::filesystem::path(path));
}


//
//	OtPathGetTmpDirectory
//

static inline std::string OtPathGetTmpDirectory() {
	return std::filesystem::temp_directory_path().string();
}


//
//	OtPathGetTmpFilename
//

static inline std::string OtPathGetTmpFilename() {
	static std::random_device device;
	static std::mt19937 generator(device());
	std::uniform_int_distribution<uint64_t> rand(0);

	auto tmp = std::filesystem::temp_directory_path();
	std::filesystem::path path;
	bool done = false;

	while (!done) {
		std::stringstream ss;
		ss << std::hex << rand(generator);
		path = tmp / ss.str();

		if (!std::filesystem::exists(path)) {
			done = true;
		}
	}

	return path.string();
}


//
//	OtPathRelative
//

static inline std::string OtPathRelative(const std::string& path, std::string* basedir) {
	// ignore virtual paths?
	if (OtPathIsVirtual(path)) {
		return path;

	// make a path relative to the provided base directory
	} else if (basedir) {
		if (path.empty()) {
			return path;

		} else {
			auto relative = std::filesystem::relative(
				std::filesystem::path(path),
				std::filesystem::path(*basedir));

			return relative == "." ? std::string() : relative.string();
		}

	} else {
		return path;
	}
}


//
//	OtPathGetAbsolute
//

static inline std::string OtPathGetAbsolute(const std::string& path, std::string* basedir) {
	// make a path absolute based on a provided base directory
	if (path.size()) {
		if (OtPathIsVirtual(path)) {
			return path;

		} else if (basedir) {
			return (std::filesystem::path(*basedir) / std::filesystem::path(path)).string();

		} else {
			return path;
		}

	} else {
		return "";
	}
}

static inline std::string OtPathGetAbsolute(nlohmann::json data, const char* field, std::string* basedir) {
	// make a path absolute based on a provided base directory
	if (data.contains(field)) {
		std::string path = data[field];
		return OtPathGetAbsolute(path, basedir);

	} else {
		return "";
	}
}


//
//	OtPathFollower
//

class OtPathFollower {
public:
	// constructor/destructor
	OtPathFollower() = default;

	OtPathFollower(const std::string& path, std::function<void()> callback) {
		follow(path, callback);
	}

	~OtPathFollower() {
		unfollow();
	}

	// reset the follower
	void clear() {
		unfollow();
	}

	// follow a (new) path
	void follow(const std::string& path, std::function<void()> callback);

	// unfollow the path
	void unfollow();

private:
	// properties
	std::string path;
	std::function<void()> callback;
	uv_fs_event_t* fsEventHandle = nullptr;
	std::filesystem::file_time_type lastUpdateTime;
	bool following = false;
};
