//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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
#include "OtMarkdownWidget.h"
#include "OtPanel.h"
#include "OtPicture.h"
#include "OtProperties.h"
#include "OtRealSlider.h"
#include "OtRows.h"
#include "OtSlippyMapWidget.h"
#include "OtTab.h"
#include "OtTabBar.h"
#include "OtTextEditorWidget.h"
#include "OtTextDiffWidget.h"
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
	module->set("Markdown", OtClass::create(OtMarkdownWidgetClass::getMeta()));
	module->set("Panel", OtClass::create(OtPanelClass::getMeta()));
	module->set("Picture", OtClass::create(OtPictureClass::getMeta()));
	module->set("Properties", OtClass::create(OtPropertiesClass::getMeta()));
	module->set("RealSlider", OtClass::create(OtRealSliderClass::getMeta()));
	module->set("Rows", OtClass::create(OtRowsClass::getMeta()));
	module->set("SlippyMap", OtClass::create(OtSlippyMapWidgetClass::getMeta()));
	module->set("Tab", OtClass::create(OtTabClass::getMeta()));
	module->set("TabBar", OtClass::create(OtTabBarClass::getMeta()));
	module->set("TextEditor", OtClass::create(OtTextEditorWidgetClass::getMeta()));
	module->set("TextDiff", OtClass::create(OtTextDiffWidgetClass::getMeta()));
	module->set("TextField", OtClass::create(OtTextFieldClass::getMeta()));
	module->set("TreeNode", OtClass::create(OtTreeNodeClass::getMeta()));
	module->set("Tron", OtClass::create(OtTronClass::getMeta()));
	module->set("VectorDisplay", OtClass::create(OtVectorDisplayClass::getMeta()));
	module->set("WidgetStack", OtClass::create(OtWidgetStackClass::getMeta()));

	module->set("alignNone", OtInteger::create(int(OtUi::Alignment::none)));
	module->set("alignLeft", OtInteger::create(int(OtUi::Alignment::left)));
	module->set("alignCenter", OtInteger::create(int(OtUi::Alignment::center)));
	module->set("alignRight", OtInteger::create(int(OtUi::Alignment::right)));
	module->set("alignTop", OtInteger::create(int(OtUi::Alignment::top)));
	module->set("alignMiddle", OtInteger::create(int(OtUi::Alignment::middle)));
	module->set("alignBottom", OtInteger::create(int(OtUi::Alignment::bottom)));
}};
