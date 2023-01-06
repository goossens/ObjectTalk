//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>

#include "OtException.h"


//
//	OtEditorClass
//

class OtEditorClass;
typedef std::shared_ptr<OtEditorClass> OtEditor;

class OtEditorClass : public std::enable_shared_from_this<OtEditorClass> {
public:
	// constructor/destructor
	OtEditorClass();
	virtual ~OtEditorClass() {}

	// render the editor
	virtual void render(float width, float height) {}

	// get the properties
	std::string getFileName() { return filename; }
	std::string getShortName();

	// see if editor content is dirty
	virtual bool isDirty() { return false; }

	// handle exception during a "run"
	virtual void error(OtException e) {}

	// get casted shared pointer
	template <typename CLASS>
	std::shared_ptr<CLASS> cast() { return std::dynamic_pointer_cast<CLASS>(shared_from_this()); }

protected:
	// properties
	std::string id;
	std::string filename;
};
