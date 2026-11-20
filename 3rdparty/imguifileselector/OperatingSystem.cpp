//	FileSelector - A file select dialog for Dear ImGui.
//	Copyright (c) 2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#if _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <sstream>

#include "FileSelector.h"


//
//	Operating System specific functions
//

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef APIENTRY
#undef APIENTRY
#endif

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>

#pragma comment(lib, "Shell32.lib")

#else
#include <pwd.h>
#include <unistd.h>

#ifdef __APPLE__
#include <sys/stat.h>

#include <CoreFoundation/CoreFoundation.h>

#include <objc/runtime.h>
#include <objc/message.h>

extern "C" {
	void *objc_autoreleasePoolPush(void);
	void objc_autoreleasePoolPop(void* pool);
}

#else
#include <fstream>
#include <mntent.h>
#endif

#endif


//
//	FileSelector::getHome
//

std::filesystem::path FileSelector::getHome() {
	// determine user's home directory and return path
#ifdef _WIN32
	PWSTR wcharPath = nullptr;

	// preferred Windows API over getenv("USERPROFILE")
	if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Profile, KF_FLAG_DEFAULT, nullptr, &wcharPath))) {
		std::filesystem::path home(wcharPath);
		CoTaskMemFree(wcharPath);
		return home;
	}

	// fallback to environment variables if API fails
	const auto userProfile = std::getenv("USERPROFILE");

	if (userProfile) {
		return std::filesystem::path(userProfile);
	}

	const auto homeDrive = std::getenv("HOMEDRIVE");
	const auto homePath = std::getenv("HOMEPATH");

	if (homeDrive && homePath) {
		return std::filesystem::path(std::string(homeDrive) + std::string(homePath));
	}

#else
	// try environment first
	const auto home = std::getenv("HOME");

	if (home) {
		return std::filesystem::path(home);
	}

	// fallback: read the password database record for the current UID
	const struct passwd* pw = getpwuid(geteuid());

	if (pw && pw->pw_dir) {
		return std::filesystem::path(pw->pw_dir);
	}

#endif

	return std::filesystem::path();
}


//
//	FileSelector::isPathHidden
//

bool FileSelector::isPathHidden(const std::filesystem::path& path) {
	// empty path are treated as hidden
	if (path.empty()) {
		return true;
	}

#ifdef _WIN32
	const auto dwAttr = GetFileAttributesW(path.c_str());
	return (dwAttr == INVALID_FILE_ATTRIBUTES) ? false : ((dwAttr & FILE_ATTRIBUTE_HIDDEN) != 0);

#else
	const auto filename = pathToString(path.filename());

	if (!filename.empty() && filename[0] == '.') {
		return true;
	}

#if __APPLE__
	struct stat info;

	if (stat(pathToString(path).c_str(), &info) == 0) {
		if (info.st_flags & UF_HIDDEN) {
			return true;
		}
	}
#endif

	return false;
#endif
}


//
//	FileSelector::isAccessible
//

bool FileSelector::isAccessible(const std::filesystem::path& path) {
	// check if path exists and is a directory
	std::error_code ec;

	if (!std::filesystem::exists(path, ec) || !std::filesystem::is_directory(path, ec)) {
		return false;
	}

	// try iterating through the directory to confirm read access
	const std::filesystem::directory_iterator it(path, ec);

	if (ec) {
		return false;
	}

	return true;
}


//
//	FileSelector::getKnownDirectoryInfo
//

void FileSelector::getKnownDirectoryInfo(KnownDirectory type, std::string& name, std::filesystem::path& path) {
#if __APPLE__
	// set default values
	static const char* types[] = {
		"Desktop",
		"Documents",
		"Downloads",
		"Movies",
		"Music",
		"Pictures"
	};

	name = types[static_cast<size_t>(type)];
	std::filesystem::path home(std::getenv("HOME"));
	path = home / name;

	// use CoreFoundation to find localized display name
	CFStringRef cfPath = CFStringCreateWithCString(kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8);
	CFURLRef cfURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, cfPath, kCFURLPOSIXPathStyle, true);
	CFStringRef localizedNameRef = nullptr;

	if (CFURLCopyResourcePropertyForKey(cfURL, kCFURLLocalizedNameKey, &localizedNameRef, nullptr)) {
		const CFIndex length = CFStringGetLength(localizedNameRef);
		const CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;

		std::vector<char> buffer(maxSize);
		CFStringGetCString(localizedNameRef, buffer.data(), maxSize, kCFStringEncodingUTF8);
		name = std::string(buffer.data());

		CFRelease(localizedNameRef);
	}

	CFRelease(cfURL);
	CFRelease(cfPath);

#elif _WIN32
	// set default values
	static const char* types[] = {
		"Desktop",
		"Documents",
		"Downloads",
		"Videos",
		"Music",
		"Pictures"
	};

	name = types[static_cast<size_t>(type)];
	const auto home = getHome();
	path = home / name;

	// initialize COM library
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	static const KNOWNFOLDERID IDs[] = {
		FOLDERID_Desktop,
		FOLDERID_Documents,
		FOLDERID_Downloads,
		FOLDERID_Videos,
		FOLDERID_Music,
		FOLDERID_Pictures
	};

	// retrieve the known folder path
	PWSTR folderPath = NULL;

	if (SUCCEEDED(SHGetKnownFolderPath(IDs[static_cast<size_t>(type)], 0, NULL, &folderPath))) {
		path = folderPath;
		CoTaskMemFree(folderPath);
	}

	// uninitialize COM library
	CoUninitialize();

#else
	// set default values
	static const char* types[] = {
		"Desktop",
		"Documents",
		"Downloads",
		"Videos",
		"Music",
		"Pictures"
	};

	name = types[static_cast<size_t>(type)];
	const auto home = getHome();
	path = home / name;

	// locate the XDG user-dirs config file
	const char* configHome = std::getenv("XDG_CONFIG_HOME");

	std::filesystem::path configFile = configHome
		? std::filesystem::path(configHome) / "user-dirs.dirs"
		: home / ".config" / "user-dirs.dirs";

	// search the config file for the requested key
	std::ifstream file(configFile);

	if (file.is_open()) {
		static const char* keys[] = {
			"XDG_DESKTOP_DIR",
			"XDG_DOCUMENTS_DIR",
			"XDG_DOWNLOAD_DIR",
			"XDG_VIDEOS_DIR",
			"XDG_MUSIC_DIR",
			"XDG_PICTURES_DIR"
		};

		std::string key = std::string("XDG_") + keys[static_cast<size_t>(type)] + "_DIR=";
		std::string line;

		while (std::getline(file, line)) {
			// look for the specific video/movie folder variable
			if (line.rfind(key) == 0) {
				// extract the path inside the quotes
				size_t firstQuote = line.find('"');
				size_t lastQuote = line.rfind('"');

				if (firstQuote != std::string::npos &&
					lastQuote != std::string::npos &&
					lastQuote > firstQuote) {

					// replace "$HOME" placeholder with the actual home directory path (if required)
					std::string rawPath = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
					path = (rawPath.rfind("$HOME", 0) == 0) ? (home / rawPath.substr(5)) : std::filesystem::path(rawPath);
					name = pathToString(path.filename());
				}
			}
		}
	}
#endif
}


//
//	FileSelector::forEachKnownLocation
//

void FileSelector::forEachKnownLocation(std::function<void(const std::string& name, const std::filesystem::path& path)> callback) {
	// iterate through known locations like mounted volumes and drives

#if __APPLE__
	std::filesystem::path volumes{"/Volumes"};

	for (const auto& entry : std::filesystem::directory_iterator(volumes)) {
		const auto filename = pathToString(entry.path().filename());
		const auto path = std::filesystem::canonical(entry.path());

		if (filename[0] != '.' && filename.rfind("com.", 0) != 0) {
			callback(filename, path);
		}
	}

#elif defined(_WIN32)
	// get list of logical drives
	DWORD bufferLength = GetLogicalDriveStringsW(0, nullptr);
	std::vector<wchar_t> buffer(bufferLength);
	GetLogicalDriveStringsW(bufferLength, buffer.data());

	// parse the null-separated block of strings
	std::vector<wchar_t> buffer2(MAX_PATH + 1);

	for (auto drive = buffer.data(); *drive; drive += wcslen(drive) + 1) {
		// get volume name
		std::string volumeName;
		GetVolumeInformationW(drive, buffer2.data(), MAX_PATH + 1, nullptr, nullptr, nullptr, nullptr, 0);
		const auto size = static_cast<int>(wcslen(buffer2.data()));

		if (size) {
			const auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, buffer2.data(), size, nullptr, 0, nullptr, nullptr);

			if (sizeNeeded) {
				volumeName = std::string(sizeNeeded, 0);
				WideCharToMultiByte(CP_UTF8, 0, buffer2.data(), size, volumeName.data(), sizeNeeded, nullptr, nullptr);
			}
		}

		// convert drive to path and logical name
		std::filesystem::path path{drive};
		auto name = pathToString(path.root_name()) + " " + volumeName;

		// add to list
		callback(name, path);
	}

#else
	// open the mounted filesystems table file
	const auto file = setmntent("/proc/mounts", "r");

	if (file == nullptr) {
		return;
	}

	// iterate through each mount entry
	for (struct mntent* entry = getmntent(file); entry != nullptr; entry = getmntent(file)) {
		std::string mountPoint(entry->mnt_dir);

		// look for paths typically handled by user space managers
		if (mountPoint.rfind("/media/", 0) == 0 || mountPoint.rfind("/run/media/", 0) == 0) {
			callback(entry->mnt_fsname, entry->mnt_dir);
		}
	}

	endmntent(file);
#endif
}


//
//	FileSelector::moveToTrashCan
//

bool FileSelector::moveToTrashCan(const std::filesystem::path& path, std::string& errorMessage) {
	// determine absolute path with .. and symbolic links resolved
	const auto canonicalPath = std::filesystem::canonical(path);

#if __APPLE__
	const auto canonicalString = pathToString(canonicalPath);
	void* pool = objc_autoreleasePoolPush();

	id pathString = ((id(*)(Class, SEL, const char*)) objc_msgSend)(
		objc_getClass("NSString"),
		sel_registerName("stringWithUTF8String:"),
		canonicalString.c_str());

	id fileManager = ((id(*)(Class, SEL)) objc_msgSend)(
		objc_getClass("NSFileManager"),
		sel_registerName("defaultManager"));

	id nsurl = ((id(*)(Class, SEL, id)) objc_msgSend)(
		objc_getClass("NSURL"),
		sel_registerName("fileURLWithPath:"),
		pathString);

	id error = nullptr;

	const auto result = ((BOOL(*)(id, SEL, id, id, id*)) objc_msgSend)(
		fileManager,
		sel_registerName("trashItemAtURL:resultingItemURL:error:"),
		nsurl,
		nullptr,
		&error);

	if (result) {
		errorMessage.clear();

	} else {
		id description = ((id(*)(id, SEL)) objc_msgSend)(error, sel_registerName("localizedDescription"));
		errorMessage = ((const char* (*)(id, SEL)) objc_msgSend)(description, sel_registerName("UTF8String"));
	}

	objc_autoreleasePoolPop(pool);
	return result;

#elif _WIN32
	// double terminate string for WIN32 API
	auto canonicalString = canonicalPath.generic_wstring();
	canonicalString.push_back(L'\0');
	canonicalString.push_back(L'\0');

	// run native API
	SHFILEOPSTRUCTW fileOp = {0};
	fileOp.wFunc = FO_DELETE;
	fileOp.pFrom = canonicalString.data();
	fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;
	const auto errorCode = SHFileOperationW(&fileOp);

	if (errorCode == 0) {
		errorMessage.clear();
		return true;

	} else {
		LPTSTR messageBuffer = nullptr;

		size_t size = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &messageBuffer,
			0,
			nullptr);

		if (size > 0 && messageBuffer != nullptr) {
			errorMessage = messageBuffer;

			while (!errorMessage.empty() && (errorMessage.back() == '\r' || errorMessage.back() == '\n')) {
				errorMessage.pop_back();
			}

			LocalFree(messageBuffer);

		} else {
			errorMessage = "Unknown error";
		}

		return false;
	}

#else
	// use desktop command to move files to trash
	std::string command = "gio trash '" + pathToString(canonicalPath) + "'";
	const auto result = std::system(command.c_str()) == 0;

	if (result) {
		errorMessage.clear();

	} else {
		errorMessage = "System error";
	}

	return result;
#endif
}


//
//	FileSelector::showInFinder
//

void FileSelector::showInFinder(const std::filesystem::path& path) {
	const auto rawUrl = "file://" + pathToString(std::filesystem::canonical(path));
	std::stringstream ss;

	for (auto ch : rawUrl) {
		if (isalnum(ch) ||
			ch == ':' || ch == '/' || ch == '?' || ch == '#' || ch == '@' || ch == '[' || ch == ']' ||
			ch == '!' || ch == '$' || ch == '&' || ch == '(' || ch == ')' ||
			ch == '*' || ch == '+' || ch == ',' || ch == ';' || ch == '=') {

			ss << ch;

		} else {
			ss << "%" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
		}
	}

	const auto encodedUrl = ss.str();

#if __APPLE__
	const auto command ="open '" + encodedUrl + "'";
	[[maybe_unused]] auto result = std::system(command.c_str());

#elif _WIN32
	const auto command ="start '" + encodedUrl + "'";
	[[maybe_unused]] auto result = std::system(command.c_str());

#else
	const auto command ="xdg-open '" + encodedUrl + "'";
	[[maybe_unused]] auto result = std::system(command.c_str());
#endif
}
