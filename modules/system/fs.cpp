//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <filesystem>
#include <random>

#include "ot/exception.h"
#include "ot/function.h"
#include "ot/array.h"

#include "fs.h"
#include "path.h"


//
//	OtFSClass::gettmp
//

OtObject OtFSClass::gettmp() {
	 return OtPathClass::create(std::filesystem::canonical(
		 std::filesystem::temp_directory_path()));
}


//
//	OtFSClass::getcwd
//

OtObject OtFSClass::getcwd() {
	return OtPathClass::create(std::filesystem::canonical(
		std::filesystem::current_path()));
}


//
//	OtFSClass::chdir
//

void OtFSClass::chdir(const std::string& path) {
	 std::filesystem::current_path(path);
}


//
//	OtFSClass::ls
//

OtObject OtFSClass::ls(const std::string& path) {
	// get content of directory
	OtArray content = OtArrayClass::create();

	for (auto& p: std::filesystem::directory_iterator(path)) {
		content->push_back(OtPathClass::create(p.path()));
	}

	return content;
}


//
//	OtFSClass::filesize
//

size_t OtFSClass::filesize(const std::string& path) {
	return (size_t) std::filesystem::file_size(path);
}


//
//	OtFSClass::cp
//

void OtFSClass::cp(const std::string& from, const std::string& to) {
	std::filesystem::copy(from, to, std::filesystem::copy_options::recursive);
}


//
//	OtFSClass::mv
//

void OtFSClass::mv(const std::string& from, const std::string& to) {
	std::filesystem::rename(from, to);
}


//
//	OtFSClass::ln
//

void OtFSClass::ln(const std::string& from, const std::string& to) {
	std::filesystem::create_hard_link(from, to);
}


//
//	OtFSClass::ln
//

void OtFSClass::lns(const std::string& from, const std::string& to) {
	std::filesystem::create_symlink(from, to);
}


//
//	OtFSClass::ln
//

void OtFSClass::resize(const std::string& path, size_t size) {
	std::filesystem::resize_file(path, size);
}


//
//	OtFSClass::rm
//

void OtFSClass::rm(const std::string& path) {
	if (!std::filesystem::remove(path)) {
		OtExcept("can't remove [%s]", path.c_str());
	}
}


//
//	OtFSClass::mkdir
//

void OtFSClass::mkdir(const std::string& path) {
	if (!std::filesystem::create_directory(path)) {
		OtExcept("can't create directory [%s]", path.c_str());
	}
}


//
//	OtFSClass::mkdirs
//

void OtFSClass::mkdirs(const std::string& path) {
	if (!std::filesystem::create_directories(path)) {
		OtExcept("can't create directories [%s]", path.c_str());
	}
}


//
//	OtFSClass::mktmpdir
//

OtObject OtFSClass::mktmpdir() {
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<uint64_t> rand(0);

	auto tmp = std::filesystem::temp_directory_path();
	std::filesystem::path path;
	bool done = false;

	while (!done) {
		std::stringstream ss;
		ss << std::hex << rand(generator);
		path = tmp / ss.str();

		if (std::filesystem::create_directory(path)) {
			done = true;
		}
	}

	return OtPathClass::create(path);
}


//
//	OtFSClass::rmdir
//

void OtFSClass::rmdir(const std::string& path) {
	if (!std::filesystem::remove(path)) {
		OtExcept("can't remove directory [%s]", path.c_str());
	}
}


//
//	OtFSClass::rmdirs
//

void OtFSClass::rmdirs(const std::string& path) {
	if (!std::filesystem::remove_all(path)) {
		OtExcept("can't remove directory [%s]", path.c_str());
	}
}


//
//	OtFSClass::capacity
//

size_t OtFSClass::capacity(const std::string& path) {
	std::filesystem::space_info space = std::filesystem::space(path);
	return space.capacity;
}


//
//	OtFSClass::free
//

size_t OtFSClass::free(const std::string& path) {
	std::filesystem::space_info space = std::filesystem::space(path);
	return space.free;
}


//
//	OtFSClass::available
//

size_t OtFSClass::available(const std::string& path) {
	std::filesystem::space_info space = std::filesystem::space(path);
	return space.available;
}


//
//	OtFSClass::getMeta
//

OtType OtFSClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtFSClass>("FS", OtSystemClass::getMeta());

		type->set("gettmp", OtFunctionClass::create(&OtFSClass::gettmp));
		type->set("getcwd", OtFunctionClass::create(&OtFSClass::getcwd));
		type->set("chdir", OtFunctionClass::create(&OtFSClass::chdir));
		type->set("ls", OtFunctionClass::create(&OtFSClass::ls));
		type->set("filesize", OtFunctionClass::create(&OtFSClass::filesize));
		type->set("cp", OtFunctionClass::create(&OtFSClass::cp));
		type->set("mv", OtFunctionClass::create(&OtFSClass::mv));
		type->set("ln", OtFunctionClass::create(&OtFSClass::ln));
		type->set("lns", OtFunctionClass::create(&OtFSClass::lns));
		type->set("resize", OtFunctionClass::create(&OtFSClass::resize));
		type->set("rm", OtFunctionClass::create(&OtFSClass::rm));
		type->set("mkdir", OtFunctionClass::create(&OtFSClass::mkdir));
		type->set("mkdirs", OtFunctionClass::create(&OtFSClass::mkdirs));
		type->set("mktmpdir", OtFunctionClass::create(&OtFSClass::mktmpdir));
		type->set("rmdir", OtFunctionClass::create(&OtFSClass::rmdir));
		type->set("rmdirs", OtFunctionClass::create(&OtFSClass::rmdirs));
		type->set("capacity", OtFunctionClass::create(&OtFSClass::capacity));
		type->set("free", OtFunctionClass::create(&OtFSClass::free));
		type->set("available", OtFunctionClass::create(&OtFSClass::available));
	}

	return type;
}


//
//	OtFSClass::create
//

OtFS OtFSClass::create() {
	OtFS fs = std::make_shared<OtFSClass>();
	fs->setType(getMeta());
	return fs;
}
