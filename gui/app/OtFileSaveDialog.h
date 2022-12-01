//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtDialog.h"


//
//	OtFileSaveDialogClass
//

class OtFileSaveDialogClass;
typedef std::shared_ptr<OtFileSaveDialogClass> OtFileSaveDialog;

class OtFileSaveDialogClass : public OtDialogClass {
public:
	// initialize dialog
	void init(size_t count, OtObject* parameters);

	// set title
	OtObject setTitle(const std::string& title);

	// set file type filters
	OtObject setTypeFilters(const std::string& filter);

	// set callback
	OtObject setCallback(OtObject callback);

	// set the currrent directory
	OtObject setCurrentDirectory(const std::string& path);

	// open dialog
	void open();

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

	// create a new object
	static OtFileSaveDialog create();

private:
	std::string title;
	std::string filters;
	std::string path = ".";
	OtObject callback;
};
