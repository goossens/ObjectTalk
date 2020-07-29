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
	std::wstring gethome()
	{
		size_t size = OT_MAX_BUFFER;
		char path[size];
		int result = uv_os_homedir(path, &size);

		if (result == UV_ENOBUFS)
			OT_EXCEPT(L"fs.gethome of size %d does not fit in buffer of size %d", size, UV_ENOBUFS);

		else if (result)
			OT_EXCEPT(L"fs.gethome failed: %s", uv_strerror(result));

		return OtTextToWide(path);
	}

	// get temporary directory
	std::wstring gettmp()
	{
		size_t size = OT_MAX_BUFFER;
		char path[size];
		int result = uv_os_tmpdir(path, &size);

		if (result == UV_ENOBUFS)
			OT_EXCEPT(L"fs.gettmp of size %d does not fit in buffer of size %d", size, UV_ENOBUFS);

		else if (result)
			OT_EXCEPT(L"fs.gettmp failed: %s", uv_strerror(result));

		return OtTextToWide(path);
	}

	// change current working directory
	void chdir(const std::wstring& path)
	{
		int result = uv_chdir(OtTextToNarrow(path).c_str());

		if (result)
			OT_EXCEPT(L"fs.chdir failed: %s", uv_strerror(result));
	}

	// get current working directory
	std::wstring getcwd()
	{
		size_t size = OT_MAX_BUFFER;
		char path[size];
		int result = uv_cwd(path, &size);

		if (result == UV_ENOBUFS)
			OT_EXCEPT(L"fs.getcwd of size %d does not fit in buffer of size %d", size, UV_ENOBUFS);

		else if (result)
			OT_EXCEPT(L"fs.getcwd failed: %s", uv_strerror(result));

		return OtTextToWide(path);
	}

	// get list of files in specified directory
	OtObject ls(const std::wstring& path)
	{
		// get content of directory
		OtArray content = OtArrayClass::create();
		uv_fs_t req;
		uv_dirent_t ent;
		int result;

		uv_fs_scandir(uv_default_loop(), &req, OtTextToNarrow(path).c_str(), 0, 0);

		while ((result = uv_fs_scandir_next(&req, &ent)) != UV_EOF)
		{
			if (result)
				OT_EXCEPT(L"fs.ls failed: %s", uv_strerror(result));

			content->push_back(OtPathClass::create(OtTextToWide(ent.name)));
		}

		return content;
	}

	// get file size
	size_t filesize(const std::wstring& path)
	{
		int result;
		uv_fs_t req;
		result = uv_fs_stat(uv_default_loop(), &req, OtTextToNarrow(path).c_str(), 0);

		if (result)
			OT_EXCEPT(L"fs.filesize failed: %s", uv_strerror(result));

		uv_stat_t* stat = uv_fs_get_statbuf(&req);
		return (size_t) stat->st_size;
	}

	// remove file
	void rm(const std::wstring& path)
	{
		int result;
		uv_fs_t req;
		result = uv_fs_unlink(uv_default_loop(), &req, OtTextToNarrow(path).c_str(), 0);

		if (result)
			OT_EXCEPT(L"fs.rm failed: %s", uv_strerror(result));
	}

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtFSClass>(L"FS", OtSystemClass::getMeta());

			type->set(L"gethome", OtFunctionCreate(&OtFSClass::gethome));
			type->set(L"gettmp", OtFunctionCreate(&OtFSClass::gettmp));
			type->set(L"chdir", OtFunctionCreate(&OtFSClass::chdir));
			type->set(L"getcwd", OtFunctionCreate(&OtFSClass::getcwd));
			type->set(L"ls", OtFunctionCreate(&OtFSClass::ls));
			type->set(L"filesize", OtFunctionCreate(&OtFSClass::filesize));
			type->set(L"rm", OtFunctionCreate(&OtFSClass::rm));
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
