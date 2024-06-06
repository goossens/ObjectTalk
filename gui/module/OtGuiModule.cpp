//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtModule.h"

#include "OtApp.h"
#include "OtCheckbox.h"
#include "OtColumns.h"
#include "OtCombobox.h"
#include "OtHeader.h"
#include "OtIntegerSlider.h"
#include "OtMenubar.h"
#include "OtPanel.h"
#include "OtRealSlider.h"
#include "OtRows.h"
#include "OtTextField.h"
#include "OtTreeNode.h"
#include "OtTron.h"


//
//	Module registration
//

static OtModuleRegistration registration{"gui", [](OtModule module) {
	module->set("App", OtClass::create(OtAppClass::getMeta()));
	module->set("Checkbox", OtClass::create(OtCheckboxClass::getMeta()));
	module->set("Columns", OtClass::create(OtColumnsClass::getMeta()));
	module->set("Combobox", OtClass::create(OtComboboxClass::getMeta()));
	module->set("Header", OtClass::create(OtHeaderClass::getMeta()));
	module->set("IntegerSlider", OtClass::create(OtIntegerSliderClass::getMeta()));
	module->set("Menubar", OtClass::create(OtMenubarClass::getMeta()));
	module->set("Panel", OtClass::create(OtPanelClass::getMeta()));
	module->set("RealSlider", OtClass::create(OtRealSliderClass::getMeta()));
	module->set("Rows", OtClass::create(OtRowsClass::getMeta()));
	module->set("TextField", OtClass::create(OtTextFieldClass::getMeta()));
	module->set("TreeNode", OtClass::create(OtTreeNodeClass::getMeta()));
	module->set("Tron", OtClass::create(OtTronClass::getMeta()));
}};
