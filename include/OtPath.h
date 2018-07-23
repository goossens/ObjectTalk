//	ObjectTalk Scripting Language
//	Copyright 1993-2018 Johan A. Goossens
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
//	OtPath
//

class OtPathClass;
typedef std::shared_ptr<OtPathClass> OtPath;


//
//	OtPathClass
//

class OtPathClass : public OtOSClass
{
public:
	// constructors
	OtPathClass() {}
	OtPathClass(const std::string& p) { path = p; }
	OtPathClass(const std::filesystem::path& p) { path = p; }

	// convert to string
	operator std::string() { return path.string(); }

	// initialize path
	OtObject init(OtObject, size_t count, OtObject* parameters)
	{
		if (count != 1)
			OT_EXCEPT("Path initializer expected 1 parameter not [%d]", count);

		path = parameters[0]->operator std::string();
		return getSharedPtr();
	}

	// operators
	OtObject add(OtObject operand) { return OtPathClass::create(path / operand->operator std::string()); }
	bool equal(OtObject operand) { return path == operand->operator std::string(); }
	bool notEqual(OtObject operand) { return path != operand->operator std::string(); }
	bool greaterThan(OtObject operand) { return path > operand->operator std::string(); }
	bool lessThan(OtObject operand) { return path < operand->operator std::string(); }
	bool greaterEqual(OtObject operand) { return path >= operand->operator std::string(); }
	bool lessEqual(OtObject operand) { return path <= operand->operator std::string(); }

	// get path elements
	OtObject directory() { return create(path.parent_path()); }
	OtObject filename() { return create(path.filename()); }
	OtObject stem() { return create(path.stem()); }
	OtObject extension() { return create(path.extension()); }

	// support iterator
	class OtPathIteratorClass : public OtInternalClass
	{
	public:
		OtPathIteratorClass() {}
		OtPathIteratorClass(std::filesystem::path::iterator b, std::filesystem::path::iterator l) { iterator = b; last = l; }

		bool end() { return iterator == last; }
		OtObject next() { return OtPathClass::create(*(iterator++)); }

		// get type definition
		static OtType getMeta()
		{
			static OtType type = nullptr;

			if (!type)
			{
				type = OtTypeClass::create<OtPathIteratorClass>("PathIterator", OtInternalClass::getMeta());
				type->set("__end__", OtFunctionCreate(&OtPathIteratorClass::end));
				type->set("__next__", OtFunctionCreate(&OtPathIteratorClass::next));
			}

			return type;
		}

		// create a new object
		static OtObject create(OtPath p) { return std::make_shared<OtPathIteratorClass>(p->path.begin(), p->path.end())->setType(getMeta()); }

	private:
		std::filesystem::path::iterator iterator;
		std::filesystem::path::iterator last;
	};

	OtObject iterate() { return OtPathIteratorClass::create(OtTypeClass::cast<OtPathClass>(getSharedPtr())); }

	// get type definition
	static OtType getMeta()
	{
		static OtType type = nullptr;

		if (!type)
		{
			type = OtTypeClass::create<OtPathClass>("Path", OtOSClass::getMeta());

			type->set("__init__", OtFunctionClass::create(&OtPathClass::init));
			type->set("__iter__", OtFunctionCreate(&OtPathClass::iterate));
			type->set("__add__", OtFunctionCreate(&OtPathClass::add));

			type->set("__eq__", OtFunctionCreate(&OtPathClass::equal));
			type->set("__ne__", OtFunctionCreate(&OtPathClass::notEqual));
			type->set("__gt__", OtFunctionCreate(&OtPathClass::greaterThan));
			type->set("__lt__", OtFunctionCreate(&OtPathClass::lessThan));
			type->set("__ge__", OtFunctionCreate(&OtPathClass::greaterEqual));
			type->set("__le__", OtFunctionCreate(&OtPathClass::lessEqual));

			type->set("directory", OtFunctionCreate(&OtPathClass::directory));
			type->set("filename", OtFunctionCreate(&OtPathClass::filename));
			type->set("stem", OtFunctionCreate(&OtPathClass::stem));
			type->set("extension", OtFunctionCreate(&OtPathClass::extension));
		}

		return type;
	}

	// create a new object
	static OtPath create(const std::string& value)
	{
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

	static OtPath create(const std::filesystem::path& value)
	{
		OtPath path = std::make_shared<OtPathClass>(value);
		path->setType(getMeta());
		return path;
	}

private:
	std::filesystem::path path;
};



/*
#pragma once

#include "fwd.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
# include <linux/limits.h>

	path make_absolute() const {
#if !defined(_WIN32)
		char temp[PATH_MAX];
		if (realpath(str().c_str(), temp) == NULL)
			throw std::runtime_error("Internal error in realpath(): " + std::string(strerror(errno)));
		return path(temp);
#else
		std::wstring value = wstr(), out(MAX_PATH, '\0');
		DWORD length = GetFullPathNameW(value.c_str(), MAX_PATH, &out[0], NULL);
		if (length == 0)
			throw std::runtime_error("Internal error in realpath(): " + std::to_string(GetLastError()));
		return path(out.substr(0, length));
#endif
	}

	bool exists() const {
#if defined(_WIN32)
		return GetFileAttributesW(wstr().c_str()) != INVALID_FILE_ATTRIBUTES;
#else
		struct stat sb;
		return stat(str().c_str(), &sb) == 0;
#endif
	}

	size_t file_size() const {
#if defined(_WIN32)
		struct _stati64 sb;
		if (_wstati64(wstr().c_str(), &sb) != 0)
			throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#else
		struct stat sb;
		if (stat(str().c_str(), &sb) != 0)
			throw std::runtime_error("path::file_size(): cannot stat file \"" + str() + "\"!");
#endif
		return (size_t) sb.st_size;
	}

	bool is_directory() const {
#if defined(_WIN32)
		DWORD result = GetFileAttributesW(wstr().c_str());
		if (result == INVALID_FILE_ATTRIBUTES)
			return false;
		return (result & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
		struct stat sb;
		if (stat(str().c_str(), &sb))
			return false;
		return S_ISDIR(sb.st_mode);
#endif
	}

	bool is_file() const {
#if defined(_WIN32)
		DWORD attr = GetFileAttributesW(wstr().c_str());
		return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
		struct stat sb;
		if (stat(str().c_str(), &sb))
			return false;
		return S_ISREG(sb.st_mode);
#endif
	}

	std::string extension() const {
		const std::string &name = filename();
		size_t pos = name.find_last_of(".");
		if (pos == std::string::npos)
			return "";
		return name.substr(pos+1);
	}

	path operator/(const path &other) const {
		if (other.absolute)
			throw std::runtime_error("path::operator/(): expected a relative path!");
		if (m_type != other.m_type)
			throw std::runtime_error("path::operator/(): expected a path of the same type!");

		path result(*this);

		for (size_t i=0; i<other.path.size(); ++i)
			result.path.push_back(other.path[i]);

		return result;
	}


	path &operator=(const path &path) {
		m_type = path.m_type;
		path = path.path;
		absolute = path.absolute;
		return *this;
	}

	path &operator=(path &&path) {
		if (this != &path) {
			m_type = path.m_type;
			path = std::move(path.path);
			absolute = path.absolute;
		}
		return *this;
	}

	friend std::ostream &operator<<(std::ostream &os, const path &path) {
		os << path.str();
		return os;
	}

	bool remove_file() {
#if !defined(_WIN32)
		return std::remove(str().c_str()) == 0;
#else
		return DeleteFileW(wstr().c_str()) != 0;
#endif
	}

	bool resize_file(size_t target_length) {
#if !defined(_WIN32)
		return ::truncate(str().c_str(), (off_t) target_length) == 0;
#else
		HANDLE handle = CreateFileW(wstr().c_str(), GENERIC_WRITE, 0, nullptr, 0, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (handle == INVALID_HANDLE_VALUE)
			return false;
		LARGE_INTEGER size;
		size.QuadPart = (LONGLONG) target_length;
		if (SetFilePointerEx(handle, size, NULL, FILE_BEGIN) == 0) {
			CloseHandle(handle);
			return false;
		}
		if (SetEndOfFile(handle) == 0) {
			CloseHandle(handle);
			return false;
		}
		CloseHandle(handle);
		return true;
#endif
	}

	path &operator=(const std::wstring &str) { set(str); return *this; }
#endif

	bool operator==(const path &p) const { return p.path == path; }
	bool operator!=(const path &p) const { return p.path != path; }

protected:

protected:
	path_type m_type;
	bool absolute;
};

inline bool create_directory(const path& p) {
#if defined(_WIN32)
	return CreateDirectoryW(p.wstr().c_str(), NULL) != 0;
#else
	return mkdir(p.str().c_str(), S_IRUSR | S_IWUSR | S_IXUSR) == 0;
#endif
}

*/
