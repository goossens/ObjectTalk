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

#include "view.h"
#include "camera.h"
#include "scene.h"

#include "sceneobject.h"

#include "ambient.h"
#include "light.h"
#include "fog.h"

#include "object3d.h"
#include "background.h"
#include "skybox.h"
#include "sun.h"
#include "sky.h"
#include "water.h"
#include "land.h"
#include "terrain.h"
#include "mesh.h"
#include "group.h"

#include "blendmap.h"
#include "heightmap.h"
#include "material.h"
#include "noisemap.h"
#include "texture.h"
#include "matrix.h"

#include "geometry.h"
#include "boxgeometry.h"
#include "circlegeometry.h"
#include "cylindergeometry.h"
#include "modelgeometry.h"
#include "planegeometry.h"
#include "spheregeometry.h"
#include "torusgeometry.h"

#include "simulation.h"
#include "cloth.h"

#include "math.h"
#include "plane.h"

#include "widget.h"

#include "screen.h"
#include "window.h"
#include "panel.h"

#include "menubar.h"
#include "menu.h"
#include "menuitem.h"

#include "treenode.h"
#include "label.h"
#include "checkbox.h"
#include "combobox.h"
#include "integerslider.h"
#include "realslider.h"

#include "tron.h"


//
//	GUI module
//

extern "C" void init(OtModule module) {
	module->set("GUI", OtClassClass::create(OtGuiClass::getMeta()));
	module->set("Application", OtClassClass::create(OtApplicationClass::getMeta()));
	module->set("Component", OtClassClass::create(OtComponentClass::getMeta()));

	module->set("View", OtClassClass::create(OtViewClass::getMeta()));
	module->set("Camera", OtClassClass::create(OtCameraClass::getMeta()));
	module->set("Scene", OtClassClass::create(OtSceneClass::getMeta()));

	module->set("SceneObject", OtClassClass::create(OtSceneObjectClass::getMeta()));

	module->set("Ambient", OtClassClass::create(OtAmbientClass::getMeta()));
	module->set("Light", OtClassClass::create(OtLightClass::getMeta()));
	module->set("Fog", OtClassClass::create(OtFogClass::getMeta()));

	module->set("Object3D", OtClassClass::create(OtObject3dClass::getMeta()));
	module->set("Background", OtClassClass::create(OtBackgroundClass::getMeta()));
	module->set("Skybox", OtClassClass::create(OtSkyboxClass::getMeta()));
	module->set("Sun", OtClassClass::create(OtSunClass::getMeta()));
	module->set("Sky", OtClassClass::create(OtSkyClass::getMeta()));
	module->set("Water", OtClassClass::create(OtWaterClass::getMeta()));
	module->set("Land", OtClassClass::create(OtLandClass::getMeta()));
	module->set("Terrain", OtClassClass::create(OtTerrainClass::getMeta()));
	module->set("Mesh", OtClassClass::create(OtMeshClass::getMeta()));
	module->set("Group", OtClassClass::create(OtGroupClass::getMeta()));

	module->set("BlendMap", OtClassClass::create(OtBlendMapClass::getMeta()));
	module->set("HeightMap", OtClassClass::create(OtHeightMapClass::getMeta()));
	module->set("Material", OtClassClass::create(OtMaterialClass::getMeta()));
	module->set("NoiseMap", OtClassClass::create(OtNoiseMapClass::getMeta()));
	module->set("Texture", OtClassClass::create(OtTextureClass::getMeta()));
	module->set("Matrix", OtClassClass::create(OtMatrixClass::getMeta()));

	module->set("getDefaultMaterials", OtFunctionClass::create(&OtMaterialClass::getDefaultMaterials));

	module->set("Geometry", OtClassClass::create(OtGeometryClass::getMeta()));
	module->set("BoxGeometry", OtClassClass::create(OtBoxGeometryClass::getMeta()));
	module->set("CircleGeometry", OtClassClass::create(OtCircleGeometryClass::getMeta()));
	module->set("CylinderGeometry", OtClassClass::create(OtCylinderGeometryClass::getMeta()));
	module->set("ModelGeometry", OtClassClass::create(OtModelGeometryClass::getMeta()));
	module->set("PlaneGeometry", OtClassClass::create(OtPlaneGeometryClass::getMeta()));
	module->set("SphereGeometry", OtClassClass::create(OtSphereGeometryClass::getMeta()));
	module->set("TorusGeometry", OtClassClass::create(OtTorusGeometryClass::getMeta()));

	module->set("Simulation", OtClassClass::create(OtSimulationClass::getMeta()));
	module->set("Cloth", OtClassClass::create(OtClothClass::getMeta()));

	module->set("Widget", OtClassClass::create(OtWidgetClass::getMeta()));
	module->set("Screen", OtClassClass::create(OtScreenClass::getMeta()));
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

	module->set("mouseLeft", OtIntegerClass::create(GLFW_MOUSE_BUTTON_LEFT));
	module->set("mouseRight", OtIntegerClass::create(GLFW_MOUSE_BUTTON_RIGHT));
	module->set("mouseMiddle", OtIntegerClass::create(GLFW_MOUSE_BUTTON_MIDDLE));

	module->set("keyLeft", OtIntegerClass::create(GLFW_KEY_LEFT));
	module->set("keyRight", OtIntegerClass::create(GLFW_KEY_RIGHT));
	module->set("keyUp", OtIntegerClass::create(GLFW_KEY_UP));
	module->set("keyDown", OtIntegerClass::create(GLFW_KEY_DOWN));
	module->set("keyPageUp", OtIntegerClass::create(GLFW_KEY_PAGE_UP));
	module->set("keyPageDown", OtIntegerClass::create(GLFW_KEY_PAGE_DOWN));
	module->set("keyHome", OtIntegerClass::create(GLFW_KEY_HOME));
	module->set("keyEnd", OtIntegerClass::create(GLFW_KEY_END));

	module->set("keyModShift", OtIntegerClass::create(GLFW_MOD_SHIFT));
	module->set("keyModCtrl", OtIntegerClass::create(GLFW_MOD_CONTROL));
	module->set("keyModAlt", OtIntegerClass::create(GLFW_MOD_ALT));
	module->set("keyModSuper", OtIntegerClass::create(GLFW_MOD_SUPER));
	module->set("keyModCapsLock", OtIntegerClass::create(GLFW_MOD_CAPS_LOCK));
	module->set("keyModNumLock", OtIntegerClass::create(GLFW_MOD_NUM_LOCK));

	module->set("Menubar", OtClassClass::create(OtMenubarClass::getMeta()));
	module->set("Menu", OtClassClass::create(OtMenuClass::getMeta()));
	module->set("MenuItem", OtClassClass::create(OtMenuItemClass::getMeta()));

	module->set("TreeNode", OtClassClass::create(OtTreeNodeClass::getMeta()));
	module->set("Label", OtClassClass::create(OtLabelClass::getMeta()));
	module->set("Checkbox", OtClassClass::create(OtCheckboxClass::getMeta()));
	module->set("Combobox", OtClassClass::create(OtComboboxClass::getMeta()));
	module->set("IntegerSlider", OtClassClass::create(OtIntegerSliderClass::getMeta()));
	module->set("RealSlider", OtClassClass::create(OtRealSliderClass::getMeta()));

	module->set("AmbientController", OtClassClass::create(OtAmbientControllerClass::getMeta()));
	module->set("CameraController", OtClassClass::create(OtCameraControllerClass::getMeta()));
	module->set("LightController", OtClassClass::create(OtLightControllerClass::getMeta()));
	module->set("FogController", OtClassClass::create(OtFogControllerClass::getMeta()));
	module->set("NoiseMapController", OtClassClass::create(OtNoiseMapControllerClass::getMeta()));
	module->set("HeightMapController", OtClassClass::create(OtHeightMapControllerClass::getMeta()));
	module->set("SkyController", OtClassClass::create(OtSkyControllerClass::getMeta()));
	module->set("SunController", OtClassClass::create(OtSunControllerClass::getMeta()));
	module->set("WaterController", OtClassClass::create(OtWaterControllerClass::getMeta()));
	module->set("LandController", OtClassClass::create(OtLandControllerClass::getMeta()));

	module->set("Tron", OtClassClass::create(OtTronClass::getMeta()));
}
