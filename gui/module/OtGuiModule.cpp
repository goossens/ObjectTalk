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
#include "OtHsplit.h"
#include "OtIntegerSlider.h"
#include "OtMenubar.h"
#include "OtPanel.h"
#include "OtRealSlider.h"
#include "OtTextField.h"
#include "OtVsplit.h"


//
//	Register module
//

static OtModuleRegister registration{"gui", [](OtModule module) {
	module->set("App", OtClass::create(OtAppClass::getMeta()));
	module->set("Checkbox", OtClass::create(OtCheckboxClass::getMeta()));
	module->set("Hsplit", OtClass::create(OtHsplitClass::getMeta()));
	module->set("IntegerSlider", OtClass::create(OtIntegerSliderClass::getMeta()));
	module->set("Menubar", OtClass::create(OtMenubarClass::getMeta()));
	module->set("Panel", OtClass::create(OtPanelClass::getMeta()));
	module->set("RealSlider", OtClass::create(OtRealSliderClass::getMeta()));
	module->set("TextField", OtClass::create(OtTextFieldClass::getMeta()));
	module->set("Vsplit", OtClass::create(OtVsplitClass::getMeta()));
}};
