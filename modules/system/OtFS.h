//	ObjectTalk Scripting Language
//	Copyright 1993-2020 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


//
//	OtFS
//

class OtFSClass;
typedef std::shared_ptr<OtFSClass> OtFS;


//
//	OtFSClass
//

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
	void chdir(const std::wstring& path) {
		 std::filesystem::current_path(path);
	}

	// get list of files in specified directory
	OtObject ls(const std::wstring& path) {
		// get content of directory
		OtArray content = OtArrayClass::create();

		for (auto& p: std::filesystem::directory_iterator(path)) {
			content->push_back(OtPathClass::create(p.path()));
		}

		return content;
	}

	// get file size
	size_t filesize(const std::wstring& path) {
		return (size_t) std::filesystem::file_size(path);
	}

	// copy a file system object
	void cp(const std::wstring& from, const std::wstring& to) {
		std::filesystem::copy(from, to, std::filesystem::copy_options::recursive);
	}

	// move (rename) a file system object
	void mv(const std::wstring& from, const std::wstring& to) {
		std::filesystem::rename(from, to);
	}

	// link a file system object
	void ln(const std::wstring& from, const std::wstring& to) {
		std::filesystem::create_hard_link(from, to);
	}

	// link a file system object symbolically
	void lns(const std::wstring& from, const std::wstring& to) {
		std::filesystem::create_symlink(from, to);
	}

	// resize file
	void resize(const std::wstring& path, size_t size) {
		std::filesystem::resize_file(path, size);
	}

	// remove file system object
	void rm(const std::wstring& path) {
		if (!std::filesystem::remove(path)) {
			OT_EXCEPT(L"can't remove [%ls]", path.c_str());
		}
	}

	// create directory
	void mkdir(const std::wstring& path) {
		if (!std::filesystem::create_directory(path)) {
			OT_EXCEPT(L"can't create directory [%ls]", path.c_str());
		}
	}

	// create (intermediate) directories
	void mkdirs(const std::wstring& path) {
		if (!std::filesystem::create_directories(path)) {
			OT_EXCEPT(L"can't create directories [%ls]", path.c_str());
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
	void rmdir(const std::wstring& path) {
		if (!std::filesystem::remove(path)) {
			OT_EXCEPT(L"can't remove directory [%ls]", path.c_str());
		}
	}

	// remove directory and its content
	void rmdirs(const std::wstring& path) {
		if (!std::filesystem::remove_all(path)) {
			OT_EXCEPT(L"can't remove directory [%ls]", path.c_str());
		}
	}

	// get file system capacity
	size_t capacity(const std::wstring& path) {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.capacity;
	}

	// get free space on file system
	size_t free(const std::wstring& path) {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.free;
	}

	// get available space on file system
	size_t available(const std::wstring& path) {
		std::filesystem::space_info space = std::filesystem::space(path);
		return space.available;
	}

	// get type definition
	static OtType getMeta() {
		static OtType type = nullptr;

		if (!type) {
			type = OtTypeClass::create<OtFSClass>(L"FS", OtSystemClass::getMeta());

			type->set(L"gettmp", OtFunctionCreate(&OtFSClass::gettmp));
			type->set(L"getcwd", OtFunctionCreate(&OtFSClass::getcwd));
			type->set(L"chdir", OtFunctionCreate(&OtFSClass::chdir));
			type->set(L"ls", OtFunctionCreate(&OtFSClass::ls));
			type->set(L"filesize", OtFunctionCreate(&OtFSClass::filesize));
			type->set(L"cp", OtFunctionCreate(&OtFSClass::cp));
			type->set(L"mv", OtFunctionCreate(&OtFSClass::mv));
			type->set(L"ln", OtFunctionCreate(&OtFSClass::ln));
			type->set(L"lns", OtFunctionCreate(&OtFSClass::lns));
			type->set(L"resize", OtFunctionCreate(&OtFSClass::resize));
			type->set(L"rm", OtFunctionCreate(&OtFSClass::rm));
			type->set(L"mkdir", OtFunctionCreate(&OtFSClass::mkdir));
			type->set(L"mkdirs", OtFunctionCreate(&OtFSClass::mkdirs));
			type->set(L"mktmpdir", OtFunctionCreate(&OtFSClass::mktmpdir));
			type->set(L"rmdir", OtFunctionCreate(&OtFSClass::rmdir));
			type->set(L"rmdirs", OtFunctionCreate(&OtFSClass::rmdirs));
			type->set(L"capacity", OtFunctionCreate(&OtFSClass::capacity));
			type->set(L"free", OtFunctionCreate(&OtFSClass::free));
			type->set(L"available", OtFunctionCreate(&OtFSClass::available));
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
