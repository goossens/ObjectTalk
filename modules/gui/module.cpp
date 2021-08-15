//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "ot/module.h"
#include "ot/class.h"
#include "ot/function.h"

#include "imgui.h"

#include "gui.h"
#include "application.h"
#include "component.h"

#include "screen.h"
#include "view.h"
#include "camera.h"
#include "light.h"
#include "fog.h"
#include "scene.h"

#include "object3d.h"
#include "mesh.h"
#include "wireframe.h"
#include "material.h"

#include "geometry.h"
#include "plane.h"
#include "circle.h"
#include "grid.h"
#include "sphere.h"
#include "cylinder.h"
#include "cone.h"
#include "torus.h"
#include "model.h"

#include "widget.h"
#include "menubar.h"
#include "menu.h"
#include "menuitem.h"

#include "window.h"
#include "panel.h"

#include "treenode.h"
#include "label.h"
#include "checkbox.h"
#include "combobox.h"

#include "cameracontroller.h"
#include "lightcontroller.h"
#include "fogcontroller.h"
#include "tron.h"


//
//	GUI module
//

extern "C" void init(OtModule module) {
	module->set("GUI", OtClassClass::create(OtGuiClass::getMeta()));
	module->set("Application", OtClassClass::create(OtApplicationClass::getMeta()));
	module->set("Component", OtClassClass::create(OtComponentClass::getMeta()));

	module->set("Screen", OtClassClass::create(OtScreenClass::getMeta()));
	module->set("View", OtClassClass::create(OtViewClass::getMeta()));
	module->set("Camera", OtClassClass::create(OtCameraClass::getMeta()));
	module->set("Light", OtClassClass::create(OtLightClass::getMeta()));
	module->set("Fog", OtClassClass::create(OtFogClass::getMeta()));
	module->set("Scene", OtClassClass::create(OtSceneClass::getMeta()));

	module->set("Object3D", OtClassClass::create(OtObject3dClass::getMeta()));
	module->set("Mesh", OtClassClass::create(OtMeshClass::getMeta()));
	module->set("Wireframe", OtClassClass::create(OtWireframeClass::getMeta()));
	module->set("Material", OtClassClass::create(OtMaterialClass::getMeta()));

	module->set("getDefaultMaterials", OtFunctionClass::create(&OtMaterialClass::getDefaultMaterials));

	module->set("Geometry", OtClassClass::create(OtGeometryClass::getMeta()));
	module->set("Circle", OtClassClass::create(OtCircleClass::getMeta()));
	module->set("Plane", OtClassClass::create(OtPlaneClass::getMeta()));
	module->set("Sphere", OtClassClass::create(OtSphereClass::getMeta()));
	module->set("Grid", OtClassClass::create(OtGridClass::getMeta()));
	module->set("Cylinder", OtClassClass::create(OtCylinderClass::getMeta()));
	module->set("Cone", OtClassClass::create(OtConeClass::getMeta()));
	module->set("Torus", OtClassClass::create(OtTorusClass::getMeta()));
	module->set("Model", OtClassClass::create(OtModelClass::getMeta()));

	module->set("Menubar", OtClassClass::create(OtMenubarClass::getMeta()));
	module->set("Menu", OtClassClass::create(OtMenuClass::getMeta()));
	module->set("MenuItem", OtClassClass::create(OtMenuItemClass::getMeta()));

	module->set("Window", OtClassClass::create(OtWindowClass::getMeta()));
	module->set("Panel", OtClassClass::create(OtPanelClass::getMeta()));

	module->set("windowNoTitleBar", OtIntegerClass::create(ImGuiWindowFlags_NoTitleBar));
	module->set("windowNoResize", OtIntegerClass::create(ImGuiWindowFlags_NoResize));
	module->set("windowNoMove", OtIntegerClass::create(ImGuiWindowFlags_NoMove));
	module->set("windowNoScrollbar", OtIntegerClass::create(ImGuiWindowFlags_NoScrollbar));
	module->set("windowNoScrollWithMouse", OtIntegerClass::create(ImGuiWindowFlags_NoScrollWithMouse));
	module->set("windowNoCollapse", OtIntegerClass::create(ImGuiWindowFlags_NoCollapse));
	module->set("windowAlwaysAutoResize", OtIntegerClass::create(ImGuiWindowFlags_AlwaysAutoResize));
	module->set("windowNoSavedSettings", OtIntegerClass::create(ImGuiWindowFlags_NoSavedSettings));
	module->set("windowNoMouseInputs", OtIntegerClass::create(ImGuiWindowFlags_NoMouseInputs));
	module->set("windowMenuBar", OtIntegerClass::create(ImGuiWindowFlags_MenuBar));
	module->set("windowHorizontalScrollbar", OtIntegerClass::create(ImGuiWindowFlags_HorizontalScrollbar));
	module->set("windowNoFocusOnAppearing", OtIntegerClass::create(ImGuiWindowFlags_NoFocusOnAppearing));
	module->set("windowNoBringToFrontOnFocus", OtIntegerClass::create(ImGuiWindowFlags_NoBringToFrontOnFocus));
	module->set("windowAlwaysVerticalScrollbar", OtIntegerClass::create(ImGuiWindowFlags_AlwaysVerticalScrollbar));
	module->set("windowAlwaysHorizontalScrollbar", OtIntegerClass::create(ImGuiWindowFlags_AlwaysHorizontalScrollbar));
	module->set("windowAlwaysUseWindowPadding", OtIntegerClass::create(ImGuiWindowFlags_AlwaysUseWindowPadding));
	module->set("windowNoNavInputs", OtIntegerClass::create(ImGuiWindowFlags_NoNavInputs));
	module->set("windowNoNavFocus", OtIntegerClass::create(ImGuiWindowFlags_NoNavFocus));
	module->set("windowUnsavedDocument", OtIntegerClass::create(ImGuiWindowFlags_UnsavedDocument));
	module->set("windowNoNav", OtIntegerClass::create(ImGuiWindowFlags_NoNav));
	module->set("windowNoDecoration", OtIntegerClass::create(ImGuiWindowFlags_NoDecoration));
	module->set("windowNoInputs", OtIntegerClass::create(ImGuiWindowFlags_NoInputs));

	module->set("TreeNode", OtClassClass::create(OtTreeNodeClass::getMeta()));
	module->set("Label", OtClassClass::create(OtLabelClass::getMeta()));
	module->set("Checkbox", OtClassClass::create(OtCheckboxClass::getMeta()));
	module->set("Combobox", OtClassClass::create(OtComboboxClass::getMeta()));

	module->set("CameraController", OtClassClass::create(OtCameraControllerClass::getMeta()));
	module->set("LightController", OtClassClass::create(OtLightControllerClass::getMeta()));
	module->set("FogController", OtClassClass::create(OtFogControllerClass::getMeta()));
	module->set("Tron", OtClassClass::create(OtTronClass::getMeta()));
}
