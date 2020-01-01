//	ObjectTalk Scripting Language
//	Copyright 1993-2019 Johan A. Goossens
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


#include <uv.h>


//
//	OtFS
//

class OtFSClass;
typedef std::shared_ptr<OtFSClass> OtFS;


//
//	OtFSClass
//

class OtFSClass : public OtSystemClass
{
public:
	OtFSClass() {}

	// get user home directory
	std::string gethome()
	{
		size_t size = OT_MAX_BUFFER;
		char path[size];
		int result = uv_os_homedir(path, &size);

		if (result == UV_ENOBUFS)
			OT_EXCEPT("GETHOME of size %d does not fit in buffer of size %d", size, UV_ENOBUFS);

		return std::string(path);
	}

	// get temporary directory
	std::string gettmp()
	{
		size_t size = OT_MAX_BUFFER;
		char path[size];
		int result = uv_os_tmpdir(path, &size);

		if (result == UV_ENOBUFS)
			OT_EXCEPT("GETTMP of size %d does not fit in buffer of size %d", size, UV_ENOBUFS);

		return std::string(path);
	}

	// change current working directory
	void chdir(const std::string& path)
	{
		int result = uv_chdir(path.c_str());

		if (result)
			OT_EXCEPT("CHDIR failed: %s", uv_strerror(result));
	}

	// get current working directory
	std::string getcwd()
	{
		size_t size = OT_MAX_BUFFER;
		char path[size];
		int result = uv_cwd(path, &size);

		if (result == UV_ENOBUFS)
			OT_EXCEPT("GETCWD of size %d does not fit in buffer of size %d", size, UV_ENOBUFS);

		return std::string(path);
	}

	// get list of files in specified directory
	OtObject ls(const std::string& path)
	{
		// get content of directory
		OtArray content = OtArrayClass::create();
		uv_fs_t req;
		uv_dirent_t ent;
		int result;

		uv_fs_scandir(uv_default_loop(), &req, path.c_str(), 0, 0);

		while ((result = uv_fs_scandir_next(&req, &ent)) != UV_EOF)
		{
			if (result)
				OT_EXCEPT("LS failed: %s", uv_strerror(result));

			content->push_back(OtPathClass::create(ent.name));
		}

		return content;
	}

	// get file size
	size_t filesize(const std::string& path)
	{
		int result;
		uv_fs_t req;
		result = uv_fs_stat(uv_default_loop(), &req, path.c_str(), 0);

		if (result)
			OT_EXCEPT("os.filesize failed: %s", uv_strerror(result));
		
		uv_stat_t* stat = uv_fs_get_statbuf(&req);
		return (size_t) stat->st_size;
	}

	// remove file
	void rm(const std::string& path)
	{
		int result;
		uv_fs_t req;
		result = uv_fs_unlink(uv_default_loop(), &req, path.c_str(), 0);

		if (result)
			OT_EXCEPT("os.rm failed: %s", uv_strerror(result));
	}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtFSClass>("FS", OtSystemClass::getMeta());

			type->set("gethome", OtFunctionCreate(&OtFSClass::gethome));
			type->set("gettmp", OtFunctionCreate(&OtFSClass::gettmp));
			type->set("chdir", OtFunctionCreate(&OtFSClass::chdir));
			type->set("getcwd", OtFunctionCreate(&OtFSClass::getcwd));
			type->set("ls", OtFunctionCreate(&OtFSClass::ls));
			type->set("filesize", OtFunctionCreate(&OtFSClass::filesize));
			type->set("rm", OtFunctionCreate(&OtFSClass::rm));
		}

		return type;
	}

	// create a new object
	static OtFS create()
	{
		OtFS fs = std::make_shared<OtFSClass>();
		fs->setType(getMeta());
		return fs;
	}
};
