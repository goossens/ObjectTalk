//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtInteger.h"
#include "OtModule.h"

#include "OtApp.h"
#include "OtCanvasStack.h"
#include "OtCheckBox.h"
#include "OtColumns.h"
#include "OtComboBox.h"
#include "OtDial.h"
#include "OtFilmStrip.h"
#include "OtHeader.h"
#include "OtIntegerSlider.h"
#include "OtLabel.h"
#include "OtMenuBar.h"
#include "OtPanel.h"
#include "OtPicture.h"
#include "OtProperties.h"
#include "OtRealSlider.h"
#include "OtRows.h"
#include "OtTab.h"
#include "OtTabBar.h"
#include "OtTextField.h"
#include "OtTreeNode.h"
#include "OtTron.h"
#include "OtVectorDisplay.h"
#include "OtWidgetStack.h"


//
//	Module registration
//

static OtModuleRegistration registration{"gui", [](OtModule module) {
	module->set("App", OtClass::create(OtAppClass::getMeta()));
	module->set("CanvasStack", OtClass::create(OtCanvasStackClass::getMeta()));
	module->set("CheckBox", OtClass::create(OtCheckBoxClass::getMeta()));
	module->set("Columns", OtClass::create(OtColumnsClass::getMeta()));
	module->set("ComboBox", OtClass::create(OtComboBoxClass::getMeta()));
	module->set("Dial", OtClass::create(OtDialClass::getMeta()));
	module->set("FilmStrip", OtClass::create(OtFilmStripClass::getMeta()));
	module->set("Header", OtClass::create(OtHeaderClass::getMeta()));
	module->set("IntegerSlider", OtClass::create(OtIntegerSliderClass::getMeta()));
	module->set("Label", OtClass::create(OtLabelClass::getMeta()));
	module->set("MenuBar", OtClass::create(OtMenuBarClass::getMeta()));
	module->set("Panel", OtClass::create(OtPanelClass::getMeta()));
	module->set("Picture", OtClass::create(OtPictureClass::getMeta()));
	module->set("Properties", OtClass::create(OtPropertiesClass::getMeta()));
	module->set("RealSlider", OtClass::create(OtRealSliderClass::getMeta()));
	module->set("Rows", OtClass::create(OtRowsClass::getMeta()));
	module->set("Tab", OtClass::create(OtTabClass::getMeta()));
	module->set("TabBar", OtClass::create(OtTabBarClass::getMeta()));
	module->set("TextField", OtClass::create(OtTextFieldClass::getMeta()));
	module->set("TreeNode", OtClass::create(OtTreeNodeClass::getMeta()));
	module->set("Tron", OtClass::create(OtTronClass::getMeta()));
	module->set("VectorDisplay", OtClass::create(OtVectorDisplayClass::getMeta()));
	module->set("WidgetStack", OtClass::create(OtWidgetStackClass::getMeta()));

	module->set("alignNone", OtInteger::create(int(OtUi::alignNone)));
	module->set("alignLeft", OtInteger::create(int(OtUi::alignLeft)));
	module->set("alignCenter", OtInteger::create(int(OtUi::alignCenter)));
	module->set("alignRight", OtInteger::create(int(OtUi::alignRight)));
	module->set("alignTop", OtInteger::create(int(OtUi::alignTop)));
	module->set("alignMiddle", OtInteger::create(int(OtUi::alignMiddle)));
	module->set("alignBottom", OtInteger::create(int(OtUi::alignBottom)));
}};
