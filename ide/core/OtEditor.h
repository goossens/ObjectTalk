//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <memory>

#include "OtException.h"


//
//	OtEditorClass
//

class OtEditorClass;
typedef std::shared_ptr<OtEditorClass> OtEditor;

class OtEditorClass : public std::enable_shared_from_this<OtEditorClass> {
public:
	// destructor
	virtual ~OtEditorClass() {}

	// load/save the document content
	virtual void load() {}
	virtual void save() {}

	// render the editor
	virtual void render() {}

	// get the properties
	virtual std::string getFileExtension() { return ""; }
	std::filesystem::path getFilePath() { return path; }
	std::string getFileName() { return path.string(); }
	std::string getShortName() { return path.filename().string(); }

	// get the properties
	void setFilePath(const std::filesystem::path& path);

	// get editor status
	virtual bool isDirty() { return false; }
	bool fileExists();

	// handle exception during a "run"
	virtual void error(OtException e) {}

	// get casted shared pointer
	template <typename CLASS>
	std::shared_ptr<CLASS> cast() { return std::dynamic_pointer_cast<CLASS>(shared_from_this()); }

protected:
	// properties
	std::filesystem::path path;
};
