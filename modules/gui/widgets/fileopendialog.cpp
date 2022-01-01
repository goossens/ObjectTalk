//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <map>
#include <vector>

#include "ot/function.h"
#include "ot/path.h"
#include "ot/vm.h"

#include "application.h"
#include "fileopendialog.h"


//
//	OtFileOpenDialogClass::init
//

OtObject OtFileOpenDialogClass::init(size_t count, OtObject* parameters) {
	// set attributes
	if (count) {
		switch (count) {
			case 3:
				setCallback(parameters[2]);

			case 2:
				setTypeFilters(parameters[1]->operator std::string());

			case 1:
				setTitle(parameters[0]->operator std::string());
				break;

			default:
				OtExcept("Too many parameters [%ld] for [FileOpenDialog] constructor (max 3)", count);
		}
	}

	return nullptr;
}


//
//	OtFileOpenDialogClass::setTitle
//

OtObject OtFileOpenDialogClass::setTitle(const std::string& t) {
	title = t;
	return shared();
}


//
//	OtFileOpenDialogClass::setTypeFilters
//

OtObject OtFileOpenDialogClass::setTypeFilters(const std::string& f) {
	filters = f;
	return shared();
}


//
//	OtFileOpenDialogClass::setCallback
//

OtObject OtFileOpenDialogClass::setCallback(OtObject cb) {
	callback = cb;
	return shared();
}


//
//	OtFileOpenDialogClass::setCurrentDirectory
//

OtObject OtFileOpenDialogClass::setCurrentDirectory(const std::string& p) {
	path = p;
	return shared();
}


//
//	OtFileOpenDialogClass::open
//

void OtFileOpenDialogClass::open() {
	ImGuiFileDialog::Instance()->OpenModal(
		"OpenFileDialog",
		title, filters.c_str(), path, 1, nullptr,
		ImGuiFileDialogFlags_DontShowHiddenFiles);
}


//
//	OtFileOpenDialogClass::render
//

void OtFileOpenDialogClass::render() {
	int width = OtApplicationClass::getWidth();
	int height = OtApplicationClass::getHeight();
	ImVec2 maxSize = ImVec2(width, height);
	ImVec2 minSize = ImVec2(width * 0.66, height * 0.66);

	if (ImGuiFileDialog::Instance()->Display("OpenFileDialog", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		// call callback if required
		if (ImGuiFileDialog::Instance()->IsOk() && callback) {
			std::map<std::string, std::string> selected = ImGuiFileDialog::Instance()->GetSelection();
			OtVM::callMemberFunction(callback, "__call__", OtPathClass::create(selected.begin()->second));
		}

		// close dialog
		ImGuiFileDialog::Instance()->Close();
	}
}


//
//	OtFileOpenDialogClass::getMeta
//

OtType OtFileOpenDialogClass::getMeta() {
	static OtType type = nullptr;

	if (!type) {
		type = OtTypeClass::create<OtFileOpenDialogClass>("FileOpenDialog", OtDialogClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtFileOpenDialogClass::init));

		type->set("open", OtFunctionClass::create(&OtFileOpenDialogClass::open));

		type->set("setTitle", OtFunctionClass::create(&OtFileOpenDialogClass::setTitle));
		type->set("setTypeFilters", OtFunctionClass::create(&OtFileOpenDialogClass::setTypeFilters));
		type->set("setCallback", OtFunctionClass::create(&OtFileOpenDialogClass::setCallback));
		type->set("setCurrentDirectory", OtFunctionClass::create(&OtFileOpenDialogClass::setCurrentDirectory));
	}

	return type;
}


//
//	OtFileOpenDialogClass::create
//

OtFileOpenDialog OtFileOpenDialogClass::create() {
	OtFileOpenDialog dialog = std::make_shared<OtFileOpenDialogClass>();
	dialog->setType(getMeta());
	return dialog;
}
