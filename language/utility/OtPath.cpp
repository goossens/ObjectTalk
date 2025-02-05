//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <random>
#include <sstream>

#include "OtLibuv.h"
#include "OtPath.h"


//
//	OtPath::replaceExtension
//

std::string OtPath::replaceExtension(const std::string path, const std::string& extension) {
	auto work = std::filesystem::path(path);
	work.replace_extension(extension);
	return work.string();
}


//
//	OtPath::getExecutable
//

std::string OtPath::getExecutable() {
	// figure out where we live
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_exepath(buffer, &length);
	UV_CHECK_ERROR("uv_exepath", status);
	return std::string(buffer, length);
}


//
//	OtPath::getHomeDirectory
//

std::string OtPath::getHomeDirectory() {
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_os_homedir(buffer, &length);
	UV_CHECK_ERROR("uv_os_homedir", status);
	return std::string(buffer, length);
}


//
//	OtPath::getDocumentsDirectory
//

std::string OtPath::getDocumentsDirectory(){
	auto home = getHomeDirectory();

#if _WIN32
	return join(home, "My Documents");

#else
	return join(home, "Documents");
#endif
}


//
//	OtPath::getTmpFilename
//

std::string OtPath::getTmpFilename() {
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
//	OtPath::getRelative
//

std::string OtPath::getRelative(const std::string& path, std::string* basedir) {
	// make a path relative to the provided base directory
	if (basedir) {
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
//	OtPath::getAbsolute
//

std::string OtPath::getAbsolute(const std::string& path, std::string* basedir) {
	// make a path absolute based on a provided base directory
	if (path.size()) {
		if (basedir) {
			auto fullPath = std::filesystem::path(*basedir) / std::filesystem::path(path);

			if (std::filesystem::exists(fullPath)) {
				return std::filesystem::canonical(fullPath).string();

			} else {
				return fullPath.string();
			}

		} else {
			return path;
		}

	} else {
		return "";
	}
}
