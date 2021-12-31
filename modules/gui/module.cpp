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
#include "world.h"

#include "guimath.h"
#include "perlin.h"
#include "plane.h"
//#include "vec2.h"
#include "vec3.h"

#include "widget.h"

#include "screen.h"
#include "window.h"
#include "guidialog.h"
#include "panel.h"

#include "menubar.h"
#include "menu.h"
#include "menuitem.h"

#include "fileopendialog.h"
#include "filesavedialog.h"

#include "checkbox.h"
#include "combobox.h"
#include "integerslider.h"
#include "filler.h"
#include "knob.h"
#include "label.h"
#include "meter.h"
#include "picture.h"
#include "realslider.h"
#include "treenode.h"
#include "tron.h"

#include "vectordisplay.h"


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

	module->set("Simulation", OtClassClass::create(OtMathClass::getMeta()));
	module->set("Cloth", OtClassClass::create(OtClothClass::getMeta()));
	module->set("World", OtClassClass::create(OtWorldClass::getMeta()));

	module->set("Math", OtClassClass::create(OtSimulationClass::getMeta()));
	module->set("Perlin", OtClassClass::create(OtPerlinClass::getMeta()));
//	module->set("Vec2", OtClassClass::create(OtVec2Class::getMeta()));
	module->set("Vec3", OtClassClass::create(OtVec3Class::getMeta()));

	module->set("Widget", OtClassClass::create(OtWidgetClass::getMeta()));
	module->set("Screen", OtClassClass::create(OtScreenClass::getMeta()));
	module->set("Window", OtClassClass::create(OtWindowClass::getMeta()));
	module->set("Dialog", OtClassClass::create(OtDialogClass::getMeta()));
	module->set("Panel", OtClassClass::create(OtPanelClass::getMeta()));

	module->set("FileOpenDialog", OtClassClass::create(OtFileOpenDialogClass::getMeta()));
	module->set("FileSaveDialog", OtClassClass::create(OtFileSaveDialogClass::getMeta()));

	module->set("Menubar", OtClassClass::create(OtMenubarClass::getMeta()));
	module->set("Menu", OtClassClass::create(OtMenuClass::getMeta()));
	module->set("MenuItem", OtClassClass::create(OtMenuItemClass::getMeta()));

	module->set("Checkbox", OtClassClass::create(OtCheckboxClass::getMeta()));
	module->set("Combobox", OtClassClass::create(OtComboboxClass::getMeta()));
	module->set("Filler", OtClassClass::create(OtFillerClass::getMeta()));
	module->set("Knob", OtClassClass::create(OtKnobClass::getMeta()));
	module->set("Label", OtClassClass::create(OtLabelClass::getMeta()));
	module->set("Meter", OtClassClass::create(OtMeterClass::getMeta()));
	module->set("Picture", OtClassClass::create(OtPictureClass::getMeta()));
	module->set("RealSlider", OtClassClass::create(OtRealSliderClass::getMeta()));
	module->set("TreeNode", OtClassClass::create(OtTreeNodeClass::getMeta()));

	module->set("AmbientController", OtClassClass::create(OtAmbientControllerClass::getMeta()));
	module->set("CameraController", OtClassClass::create(OtCameraControllerClass::getMeta()));
	module->set("LightController", OtClassClass::create(OtLightControllerClass::getMeta()));
	module->set("FogController", OtClassClass::create(OtFogControllerClass::getMeta()));
	module->set("NoiseMapController", OtClassClass::create(OtNoiseMapControllerClass::getMeta()));
	module->set("HeightMapController", OtClassClass::create(OtHeightMapControllerClass::getMeta()));
	module->set("SkyController", OtClassClass::create(OtSkyControllerClass::getMeta()));
	module->set("SunController", OtClassClass::create(OtSunControllerClass::getMeta()));
	module->set("TerrainController", OtClassClass::create(OtTerrainControllerClass::getMeta()));
	module->set("VectorDisplayController", OtClassClass::create(OtVectorDisplayControllerClass::getMeta()));
	module->set("WaterController", OtClassClass::create(OtWaterControllerClass::getMeta()));

	module->set("Tron", OtClassClass::create(OtTronClass::getMeta()));

	module->set("VectorDisplay", OtClassClass::create(OtVectorDisplayClass::getMeta()));

	OtApplicationClass::addEnumsGLFW(module);
	OtApplicationClass::addEnumsIMGUI(module);
}
