//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "ot/singleton.h"

#include "ide.h"
#include "editor.h"


//
//	OtWorkspaceClass
//

class OtWorkspaceClass;
typedef std::shared_ptr<OtWorkspaceClass> OtWorkspace;

class OtWorkspaceClass : public OtIdeClass, public OtObjectSingleton<OtWorkspaceClass> {
public:
	// initialize IDE
	void init();

	// create a new file
	void newFile();

	// open a file
	void openFile(const std::string& filename);

	// run IDE
	void run();

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtWorkspace create();

private:
	// list of open editors
	std::vector<OtEditor> editors;
};
