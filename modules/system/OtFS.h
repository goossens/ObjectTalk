//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	OtFS
//

class OtFSClass;
typedef std::shared_ptr<OtFSClass> OtFS;

class OtFSClass : public OtSystemClass {
public:
	// get temporary directory
	OtObject gettmp() {
		 return OtPathClass::create(std::filesystem::canonical(
			 std::filesystem::temp_directory_path()));
	}

	// get current working directory
	OtObject getcwd() {
		return OtPathClass::create(std::filesystem::canonical(
			std::filesystem::current_path()));
	}

	// change current working directory
	void chdir(const std::string& path) {
		 std::filesystem::current_path(path);
	}

	// get list of files in specified directory
	OtObject ls(const std::string& path) {
		// get content of directory
		OtArray content = OtArrayClass::create();

		for (auto& p: std::filesystem::directory_iterator(path)) {
			content->push_back(OtPathClass::create(p.path()));
		}

		return content;
	}

	// get file size
	size_t filesize(const std::string& path) {
		return (size_t) std::filesystem::file_size(path);
	}

	// copy a file system object
	void cp(const std::string& from, const std::string& to) {
		std::filesystem::copy(from, to, std::filesystem::copy_options::recursive);
	}

	// move (rename) a file system object
	void mv(const std::string& from, const std::string& to) {
		std::filesystem::rename(from, to);
	}

	// link a file system object
	void ln(const std::string& from, const std::string& to) {
		std::filesystem::create_hard_link(from, to);
	}

	// link a file system object symbolically
	void lns(const std::string& from, const std::string& to) {
		std::filesystem::create_symlink(from, to);
	}

	// resize file
	void resize(const std::string& path, size_t size) {
		std::filesystem::resize_file(path, size);
	}

	// remove file system object
	void rm(const std::string& path) {
		if (!std::filesystem::remove(path)) {
			OT_EXCEPT("can't remove [%s]", path.c_str());
		}
	}

	// create directory
	void mkdir(const std::string& path) {
		if (!std::filesystem::create_directory(path)) {
			OT_EXCEPT("can't create directory [%s]", path.c_str());
		}
	}

	// create (intermediate) directories
	void mkdirs(const std::string& path) {
		if (!std::filesystem::create_directories(path)) {
			OT_EXCEPT("can't create directories [%s]", path.c_str());
		}
	}

	// create temporary directories
	OtObject mktmpdir() {
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

	// remove directory
	void rmdir(const std::string& path) {
		if (!std::filesystem::remove(path)) {
			OT_EXCEPT("can't remove directory [%s]", path.c_str());
		}
	}

	// remove directory and its content
	void rmdirs(const std::string& path) {
		if (!std::filesystem::remove_all(path)) {
			OT_EXCEPT("can't remove directory [%s]", path.c_str());
		}
	}

	// get file system capacity
	size_t capacity(const std::string& path) {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.capacity;
	}

	// get free space on file system
	size_t free(const std::string& path) {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.free;
	}

	// get available space on file system
	size_t available(const std::string& path) {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.available;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtFSClass>("FS", OtSystemClass::getMeta());

			type->set("gettmp", OtFunctionCreate(&OtFSClass::gettmp));
			type->set("getcwd", OtFunctionCreate(&OtFSClass::getcwd));
			type->set("chdir", OtFunctionCreate(&OtFSClass::chdir));
			type->set("ls", OtFunctionCreate(&OtFSClass::ls));
			type->set("filesize", OtFunctionCreate(&OtFSClass::filesize));
			type->set("cp", OtFunctionCreate(&OtFSClass::cp));
			type->set("mv", OtFunctionCreate(&OtFSClass::mv));
			type->set("ln", OtFunctionCreate(&OtFSClass::ln));
			type->set("lns", OtFunctionCreate(&OtFSClass::lns));
			type->set("resize", OtFunctionCreate(&OtFSClass::resize));
			type->set("rm", OtFunctionCreate(&OtFSClass::rm));
			type->set("mkdir", OtFunctionCreate(&OtFSClass::mkdir));
			type->set("mkdirs", OtFunctionCreate(&OtFSClass::mkdirs));
			type->set("mktmpdir", OtFunctionCreate(&OtFSClass::mktmpdir));
			type->set("rmdir", OtFunctionCreate(&OtFSClass::rmdir));
			type->set("rmdirs", OtFunctionCreate(&OtFSClass::rmdirs));
			type->set("capacity", OtFunctionCreate(&OtFSClass::capacity));
			type->set("free", OtFunctionCreate(&OtFSClass::free));
			type->set("available", OtFunctionCreate(&OtFSClass::available));
		}

		return type;
	}

	// create a new object
	static OtFS create() {
		OtFS fs = std::make_shared<OtFSClass>();
		fs->setType(getMeta());
		return fs;
	}
};
