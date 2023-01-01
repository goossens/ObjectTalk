//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "OtCallback.h"
#include "OtFunction.h"
#include "OtPath.h"
#include "OtVM.h"

#include "OtFramework.h"
#include "OtFileSaveDialog.h"


//
//	OtFileSaveDialogClass::init
//

void OtFileSaveDialogClass::init(size_t count, OtObject* parameters) {
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
				OtExcept("Too many parameters [%ld] for [FileSaveDialog] constructor (max 3)", count);
		}
	}
}


//
//	OtFileSaveDialogClass::setTitle
//

OtObject OtFileSaveDialogClass::setTitle(const std::string& t) {
	title = t;
	return shared();
}


//
//	OtFileSaveDialogClass::setTypeFilters
//

OtObject OtFileSaveDialogClass::setTypeFilters(const std::string& f) {
	filters = f;
	return shared();
}


//
//	OtFileSaveDialogClass::setCallback
//

OtObject OtFileSaveDialogClass::setCallback(OtObject cb) {
	OtCallbackValidate(cb, 1);
	callback = cb;
	return shared();
}


//
//	OtFileSaveDialogClass::setCurrentDirectory
//

OtObject OtFileSaveDialogClass::setCurrentDirectory(const std::string& p) {
	path = p;
	return shared();
}


//
//	OtFileSaveDialogClass::open
//

void OtFileSaveDialogClass::open() {
	ImGuiFileDialog::Instance()->OpenDialog(
		"SaveFileDialog",
		title, filters.c_str(), path, 1, nullptr,
		ImGuiFileDialogFlags_Modal |
			ImGuiFileDialogFlags_ConfirmOverwrite |
			ImGuiFileDialogFlags_DontShowHiddenFiles);
}


//
//	OtFileSaveDialogClass::render
//

void OtFileSaveDialogClass::render() {
	OtFramework framework = OtFrameworkClass::instance();
	int width = framework->getWidth();
	int height = framework->getHeight();
	ImVec2 maxSize = ImVec2(width, height);
	ImVec2 minSize = ImVec2(width * 0.5, height * 0.5);

	if (ImGuiFileDialog::Instance()->Display("SaveFileDialog", ImGuiWindowFlags_NoCollapse, minSize, maxSize)) {
		// call callback if required
		if (ImGuiFileDialog::Instance()->IsOk() && callback) {
			OtVM::instance()->callMemberFunction(callback, "__call__", OtPathClass::create(ImGuiFileDialog::Instance()->GetFilePathName()));
		}

		// close dialog
		ImGuiFileDialog::Instance()->Close();
	}
}


//
//	OtFileSaveDialogClass::getMeta
//

OtType OtFileSaveDialogClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtFileSaveDialogClass>("FileSaveDialog", OtDialogClass::getMeta());
		type->set("__init__", OtFunctionClass::create(&OtFileSaveDialogClass::init));

		type->set("open", OtFunctionClass::create(&OtFileSaveDialogClass::open));

		type->set("setTitle", OtFunctionClass::create(&OtFileSaveDialogClass::setTitle));
		type->set("setTypeFilters", OtFunctionClass::create(&OtFileSaveDialogClass::setTypeFilters));
		type->set("setCallback", OtFunctionClass::create(&OtFileSaveDialogClass::setCallback));
		type->set("setCurrentDirectory", OtFunctionClass::create(&OtFileSaveDialogClass::setCurrentDirectory));
	}

	return type;
}


//
//	OtFileSaveDialogClass::create
//

OtFileSaveDialog OtFileSaveDialogClass::create() {
	OtFileSaveDialog dialog = std::make_shared<OtFileSaveDialogClass>();
	dialog->setType(getMeta());
	return dialog;
}
