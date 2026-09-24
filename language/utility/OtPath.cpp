//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdlib>
#include <random>
#include <sstream>

#include "OtLibuv.h" // IWYU pragma: keep
#include "OtPath.h"


//
//	OtPath::toString
//

std::string OtPath::toString(const std::filesystem::path& path) {
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || (__cplusplus >= 202002L)
	auto u8Str = path.generic_u8string();
	std::string str(u8Str.begin(), u8Str.end());
	return str;

#else
	return path.u8string();
#endif
}


//
//	OtPath::toPath
//

std::filesystem::path OtPath::toPath(const std::string& string) {
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || (__cplusplus >= 202002L)
	return std::filesystem::path(
		reinterpret_cast<const char8_t*>(string.data()),
		reinterpret_cast<const char8_t*>(string.data() + string.size()));

#else
	return std::filesystem::u8path(string);
#endif
}


//
//	OtPath::replaceExtension
//

std::string OtPath::replaceExtension(const std::string& path, const std::string& extension) {
	auto work = toPath(path);
	work.replace_extension(extension);
	return toString(work);
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
//	OtPath::getPreferencesDirectory
//

std::string OtPath::getPreferencesDirectory() {
#if __APPLE__
	auto home = getHomeDirectory();
	return join(join(home, "Library"), "Preferences");

#elif _WIN32
	size_t size = 256;
	char* value = (char*) malloc(size);
	int result = uv_os_getenv("LOCALAPPDATA", value, &size);

	if (result == UV_ENOBUFS) {
		value = (char*) realloc(value, size);
		result = uv_os_getenv("LOCALAPPDATA", value, &size);
	}

	if (result == UV_ENOENT) {
		return nullptr;
	}

	std::string v = value;
	free(value);
	return v;

#else
	auto home = getHomeDirectory();
	return join(home, "/.config");
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

	return toString(path);
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
			auto relative = std::filesystem::relative(toPath(path), toPath(*basedir));
			return relative == "." ? std::string() : toString(relative);
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
			auto fullPath = toPath(*basedir) / toPath(path);

			if (std::filesystem::exists(fullPath)) {
				return toString(std::filesystem::canonical(fullPath));

			} else {
				return toString(fullPath);
			}

		} else {
			return path;
		}

	} else {
		return "";
	}
}
