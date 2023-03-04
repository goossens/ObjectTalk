//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <filesystem>
#include <fstream>
#include <random>

#include "OtArray.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtFS.h"
#include "OtLibuv.h"
#include "OtPath.h"


//
//	OtFSClass::gettmp
//

OtObject OtFSClass::gethome() {
	char buffer[1024];
	size_t length = 1024;
	auto status = uv_os_homedir(buffer, &length);
	UV_CHECK_ERROR("uv_os_homedir", status);
	std::string home(buffer, length);
	return OtPath::create(std::filesystem::canonical(
		std::string(buffer, length)));
}


//
//	OtFSClass::gettmp
//

OtObject OtFSClass::gettmp() {
	return OtPath::create(std::filesystem::canonical(
		std::filesystem::temp_directory_path()));
}


//
//	OtFSClass::tmpnam
//

OtObject OtFSClass::tmpnam() {
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

	return OtPath::create(path);
}


//
//	OtFSClass::getcwd
//

OtObject OtFSClass::getcwd() {
	try {
		return OtPath::create(std::filesystem::canonical(
			std::filesystem::current_path()));

	} catch (std::exception& e) {
		OtExcept("Can't get current directory, error: %s", e.what());
	}

	return nullptr;
}


//
//	OtFSClass::chdir
//

void OtFSClass::chdir(const std::string& path) {
	try {
		std::filesystem::current_path(path);

	} catch (std::exception& e) {
		OtExcept("Can't set current directory to [%s], error: %s", path.c_str(), e.what());
	}
}


//
//	OtFSClass::ls
//

OtObject OtFSClass::ls(const std::string& path) {
	try {
		// get content of directory
		OtArray content = OtArray::create();

		for (auto& p : std::filesystem::directory_iterator(path)) {
			content->append(OtPath::create(p.path()));
		}

		return content;

	} catch (std::exception& e) {
		OtExcept("Can't list directory [%s], error: %s", path.c_str(), e.what());
	}

	return nullptr;
}


//
//	OtFSClass::filesize
//

size_t OtFSClass::filesize(const std::string& path) {
	try {
		return (size_t) std::filesystem::file_size(path);

	} catch (std::exception& e) {
		OtExcept("Can't get size of file [%s], error: %s", path.c_str(), e.what());
	}

	return 0;
}


//
//	OtFSClass::touch
//

void OtFSClass::touch(const std::string& path) {
	if (std::filesystem::exists(path)) {
		uv_timeval64_t time;
		auto result = uv_gettimeofday(&time);
		UV_CHECK_ERROR("uv_gettimeofday", result);
		double now = time.tv_sec;

		uv_fs_t req;
		result = uv_fs_utime(nullptr, &req, path.c_str(), now, now, nullptr);
		UV_CHECK_ERROR("uv_fs_utime", result);
		uv_fs_req_cleanup(&req);

	} else {
		std::ofstream file;
		file.open(path, std::ofstream::out);

		if ((file.rdstate() & std::ofstream::failbit) != 0) {
			OtExcept("Can't touch file [%s]", path.c_str());
		}

		file.close();

	}
}


//
//	OtFSClass::cp
//

void OtFSClass::cp(const std::string& from, const std::string& to) {
	try {
		std::filesystem::copy(from, to, std::filesystem::copy_options::recursive);

	} catch (std::exception& e) {
		OtExcept("Can't copy file [%s] to [%s], error: %s", from.c_str(), to.c_str(), e.what());
	}
}


//
//	OtFSClass::mv
//

void OtFSClass::mv(const std::string& from, const std::string& to) {
	try {
		std::filesystem::rename(from, to);

	} catch (std::exception& e) {
		OtExcept("Can't move file [%s] to [%s], error: %s", from.c_str(), to.c_str(), e.what());
	}
}


//
//	OtFSClass::ln
//

void OtFSClass::ln(const std::string& from, const std::string& to) {
	try {
		std::filesystem::create_hard_link(from, to);

	} catch (std::exception& e) {
		OtExcept("Can't link file [%s] to [%s], error: %s", from.c_str(), to.c_str(), e.what());
	}
}


//
//	OtFSClass::ln
//

void OtFSClass::lns(const std::string& from, const std::string& to) {
	try {
		std::filesystem::create_symlink(from, to);

	} catch (std::exception& e) {
		OtExcept("Can't symbolically link file [%s] to [%s], error: %s", from.c_str(), to.c_str(), e.what());
	}
}


//
//	OtFSClass::ln
//

void OtFSClass::resize(const std::string& path, size_t size) {
	try {
		std::filesystem::resize_file(path, size);

	} catch (std::exception& e) {
		OtExcept("Can't resize file [%s], error: %s", path.c_str(), e.what());
	}
}


//
//	OtFSClass::rm
//

void OtFSClass::rm(const std::string& path) {
	if (!std::filesystem::remove(path)) {
		OtExcept("Can't remove [%s]", path.c_str());
	}
}


//
//	OtFSClass::mkdir
//

void OtFSClass::mkdir(const std::string& path) {
	if (!std::filesystem::create_directory(path)) {
		OtExcept("Can't create directory [%s]", path.c_str());
	}
}


//
//	OtFSClass::mkdirs
//

void OtFSClass::mkdirs(const std::string& path) {
	if (!std::filesystem::create_directories(path)) {
		OtExcept("Can't create directories [%s]", path.c_str());
	}
}


//
//	OtFSClass::mktmpdir
//

OtObject OtFSClass::mktmpdir() {
	static std::random_device rd;
	static std::mt19937 generator(rd());
	static std::uniform_int_distribution<uint64_t> distribution;

	auto tmp = std::filesystem::temp_directory_path();
	std::filesystem::path path;
	bool done = false;

	while (!done) {
		std::stringstream ss;
		ss << std::hex << distribution(generator);
		path = tmp / ss.str();

		if (std::filesystem::create_directory(path)) {
			done = true;
		}
	}

	return OtPath::create(path);
}


//
//	OtFSClass::rmdir
//

void OtFSClass::rmdir(const std::string& path) {
	if (!std::filesystem::remove(path)) {
		OtExcept("Can't remove directory [%s]", path.c_str());
	}
}


//
//	OtFSClass::rmdirs
//

void OtFSClass::rmdirs(const std::string& path) {
	if (!std::filesystem::remove_all(path)) {
		OtExcept("Can't remove directory [%s]", path.c_str());
	}
}


//
//	OtFSClass::capacity
//

size_t OtFSClass::capacity(const std::string& path) {
	try {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.capacity;

	} catch (std::exception& e) {
		OtExcept("Can't get capacity for [%s], error: %s", path.c_str(), e.what());
	}

	return 0;
}


//
//	OtFSClass::free
//

size_t OtFSClass::free(const std::string& path) {
	try {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.free;

	} catch (std::exception& e) {
		OtExcept("Can't get free space for [%s], error: %s", path.c_str(), e.what());
	}

	return 0;
}


//
//	OtFSClass::available
//

size_t OtFSClass::available(const std::string& path) {
	try {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.available;

	} catch (std::exception& e) {
		OtExcept("Can't get available space for [%s], error: %s", path.c_str(), e.what());
	}

	return 0;
}


//
//	OtFSClass::getMeta
//

OtType OtFSClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtType::create<OtFSClass>("FS", OtSystemClass::getMeta());
		type->set("gethome", OtFunction::create(&OtFSClass::gethome));
		type->set("gettmp", OtFunction::create(&OtFSClass::gettmp));
		type->set("tmpnam", OtFunction::create(&OtFSClass::tmpnam));
		type->set("getcwd", OtFunction::create(&OtFSClass::getcwd));
		type->set("chdir", OtFunction::create(&OtFSClass::chdir));
		type->set("ls", OtFunction::create(&OtFSClass::ls));
		type->set("filesize", OtFunction::create(&OtFSClass::filesize));
		type->set("touch", OtFunction::create(&OtFSClass::touch));
		type->set("cp", OtFunction::create(&OtFSClass::cp));
		type->set("mv", OtFunction::create(&OtFSClass::mv));
		type->set("ln", OtFunction::create(&OtFSClass::ln));
		type->set("lns", OtFunction::create(&OtFSClass::lns));
		type->set("resize", OtFunction::create(&OtFSClass::resize));
		type->set("rm", OtFunction::create(&OtFSClass::rm));
		type->set("mkdir", OtFunction::create(&OtFSClass::mkdir));
		type->set("mkdirs", OtFunction::create(&OtFSClass::mkdirs));
		type->set("mktmpdir", OtFunction::create(&OtFSClass::mktmpdir));
		type->set("rmdir", OtFunction::create(&OtFSClass::rmdir));
		type->set("rmdirs", OtFunction::create(&OtFSClass::rmdirs));
		type->set("capacity", OtFunction::create(&OtFSClass::capacity));
		type->set("free", OtFunction::create(&OtFSClass::free));
		type->set("available", OtFunction::create(&OtFSClass::available));
	}

	return type;
}
