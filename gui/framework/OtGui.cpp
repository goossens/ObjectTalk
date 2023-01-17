//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cstdlib>
#include <functional>
#include <iostream>

#include "OtClass.h"
#include "OtException.h"
#include "OtFunction.h"
#include "OtModule.h"
#include "OtOS.h"

#include "OtFramework.h"
#include "OtNode.h"

#include "OtApp.h"

#include "OtAppObject.h"
#include "OtAppMenubar.h"
#include "OtFileOpenDialog.h"
#include "OtFileSaveDialog.h"
#include "OtWindow.h"
#include "OtDialog.h"
#include "OtPanel.h"
#include "OtVectorDisplay.h"
#include "OtView.h"

#include "OtWidget.h"
#include "OtCheckbox.h"
#include "OtCombobox.h"
#include "OtIntegerSlider.h"
#include "OtFiller.h"
#include "OtKnob.h"
#include "OtLabel.h"
#include "OtMenubar.h"
#include "OtMenu.h"
#include "OtMenuItem.h"
#include "OtMeter.h"
#include "OtPicture.h"
#include "OtRealSlider.h"
#include "OtTreeNode.h"
#include "OtTron.h"

#include "OtCamera.h"
#include "OtScene.h"

#include "OtSceneObject.h"
#include "OtSky.h"
#include "OtSkybox.h"
#include "OtTerrain.h"
#include "OtTerrainMap.h"
#include "OtWater.h"

#include "OtAmbientLight.h"
#include "OtDirectionalLight.h"
#include "OtFog.h"
#include "OtPointLight.h"
#include "OtSpotLight.h"
#include "OtSun.h"

#include "OtBlendMappedMaterial.h"
#include "OtColoredMaterial.h"
#include "OtFixedMaterial.h"
#include "OtTexturedMaterial.h"

#include "OtObject3d.h"
#include "OtGroup.h"
#include "OtMesh.h"

#include "OtFont.h"
#include "OtHeightMap.h"
#include "OtNoiseMap.h"
#include "OtMatrix.h"

#include "OtGeometry.h"
#include "OtBoxGeometry.h"
#include "OtCircleGeometry.h"
#include "OtCylinderGeometry.h"
#include "OtDodecahedronGeometry.h"
#include "OtExtrudedGeometry.h"
#include "OtIcosahedronGeometry.h"
#include "OtModelGeometry.h"
#include "OtOctahedronGeometry.h"
#include "OtPlaneGeometry.h"
#include "OtSphereGeometry.h"
#include "OtSphericalCubeGeometry.h"
#include "OtTetrahedronGeometry.h"
#include "OtTorusGeometry.h"

#include "OtAnimation.h"

#include "OtSimulation.h"
#include "OtCloth.h"
#include "OtWorld.h"
#include "OtBody.h"

#include "OtShape.h"

#include "OtObj.h"
#include "OtGui.h"


//
//	Globals
//

OtAppClass* OtGuiClass::app;


//
//	OtGuiClass::registerModule
//

void OtGuiClass::registerModule() {
	OtModuleClass::registerInternal("gui", [](OtModule module) {
		module->set("GUI", OtClassClass::create(OtGuiClass::getMeta()));

		module->set("App", OtClassClass::create(OtAppClass::getMeta()));
		module->set("AppMenubar", OtClassClass::create(OtAppMenubarClass::getMeta()));
		module->set("Window", OtClassClass::create(OtWindowClass::getMeta()));
		module->set("Dialog", OtClassClass::create(OtDialogClass::getMeta()));
		module->set("FileOpenDialog", OtClassClass::create(OtFileOpenDialogClass::getMeta()));
		module->set("FileSaveDialog", OtClassClass::create(OtFileSaveDialogClass::getMeta()));
		module->set("Panel", OtClassClass::create(OtPanelClass::getMeta()));
		module->set("View", OtClassClass::create(OtViewClass::getMeta()));
		module->set("VectorDisplay", OtClassClass::create(OtVectorDisplayClass::getMeta()));

		module->set("Camera", OtClassClass::create(OtCameraClass::getMeta()));
		module->set("Scene", OtClassClass::create(OtSceneClass::getMeta()));

		module->set("SceneObject", OtClassClass::create(OtSceneObjectClass::getMeta()));
		module->set("Skybox", OtClassClass::create(OtSkyboxClass::getMeta()));
		module->set("Sky", OtClassClass::create(OtSkyClass::getMeta()));
		module->set("Water", OtClassClass::create(OtWaterClass::getMeta()));
		module->set("Terrain", OtClassClass::create(OtTerrainClass::getMeta()));
		module->set("TerrainMap", OtClassClass::create(OtTerrainMapClass::getMeta()));

		module->set("AmbientLight", OtClassClass::create(OtAmbientLightClass::getMeta()));
		module->set("DirectionalLight", OtClassClass::create(OtDirectionalLightClass::getMeta()));
		module->set("Fog", OtClassClass::create(OtFogClass::getMeta()));
		module->set("PointLight", OtClassClass::create(OtPointLightClass::getMeta()));
		module->set("SpotLight", OtClassClass::create(OtSpotLightClass::getMeta()));
		module->set("Sun", OtClassClass::create(OtSunClass::getMeta()));

		module->set("BlendMappedMaterial", OtClassClass::create(OtBlendMappedMaterialClass::getMeta()));
		module->set("ColoredMaterial", OtClassClass::create(OtColoredMaterialClass::getMeta()));
		module->set("FixedMaterial", OtClassClass::create(OtFixedMaterialClass::getMeta()));
		module->set("TexturedMaterial", OtClassClass::create(OtTexturedMaterialClass::getMeta()));

		module->set("Object3D", OtClassClass::create(OtObject3dClass::getMeta()));
		module->set("Mesh", OtClassClass::create(OtMeshClass::getMeta()));
		module->set("Group", OtClassClass::create(OtGroupClass::getMeta()));

		module->set("Font", OtClassClass::create(OtFontClass::getMeta()));
		module->set("HeightMap", OtClassClass::create(OtHeightMapClass::getMeta()));
		module->set("NoiseMap", OtClassClass::create(OtNoiseMapClass::getMeta()));
		module->set("Matrix", OtClassClass::create(OtMatrixClass::getMeta()));

		module->set("getDefaultMaterialNames", OtFunctionClass::create(&OtColoredMaterialClass::getDefaultMaterialNames));
		module->set("loadObj", OtFunctionClass::create(&OtObjLoad));

		module->set("Geometry", OtClassClass::create(OtGeometryClass::getMeta()));
		module->set("BoxGeometry", OtClassClass::create(OtBoxGeometryClass::getMeta()));
		module->set("CircleGeometry", OtClassClass::create(OtCircleGeometryClass::getMeta()));
		module->set("CylinderGeometry", OtClassClass::create(OtCylinderGeometryClass::getMeta()));
		module->set("DodecahedronGeometry", OtClassClass::create(OtDodecahedronGeometryClass::getMeta()));
		module->set("ExtrudedGeometry", OtClassClass::create(OtExtrudedGeometryClass::getMeta()));
		module->set("IcosahedronGeometry", OtClassClass::create(OtIcosahedronGeometryClass::getMeta()));
		module->set("ModelGeometry", OtClassClass::create(OtModelGeometryClass::getMeta()));
		module->set("OctahedronGeometry", OtClassClass::create(OtOctahedronGeometryClass::getMeta()));
		module->set("PlaneGeometry", OtClassClass::create(OtPlaneGeometryClass::getMeta()));
		module->set("SphereGeometry", OtClassClass::create(OtSphereGeometryClass::getMeta()));
		module->set("SphericalCubeGeometry", OtClassClass::create(OtSphericalCubeGeometryClass::getMeta()));
		module->set("TetrahedronGeometry", OtClassClass::create(OtTetrahedronGeometryClass::getMeta()));
		module->set("TorusGeometry", OtClassClass::create(OtTorusGeometryClass::getMeta()));

		module->set("Animation", OtClassClass::create(OtAnimationClass::getMeta()));

		module->set("Simulation", OtClassClass::create(OtSimulationClass::getMeta()));
		module->set("Cloth", OtClassClass::create(OtClothClass::getMeta()));
		module->set("World", OtClassClass::create(OtWorldClass::getMeta()));
		module->set("Body", OtClassClass::create(OtBodyClass::getMeta()));

		module->set("Shape", OtClassClass::create(OtShapeClass::getMeta()));

		module->set("Widget", OtClassClass::create(OtWidgetClass::getMeta()));
		module->set("Checkbox", OtClassClass::create(OtCheckboxClass::getMeta()));
		module->set("Combobox", OtClassClass::create(OtComboboxClass::getMeta()));
		module->set("Filler", OtClassClass::create(OtFillerClass::getMeta()));
		module->set("IntegerSlider", OtClassClass::create(OtIntegerSliderClass::getMeta()));
		module->set("Knob", OtClassClass::create(OtKnobClass::getMeta()));
		module->set("Label", OtClassClass::create(OtLabelClass::getMeta()));
		module->set("Menubar", OtClassClass::create(OtMenubarClass::getMeta()));
		module->set("Menu", OtClassClass::create(OtMenuClass::getMeta()));
		module->set("MenuItem", OtClassClass::create(OtMenuItemClass::getMeta()));
		module->set("Meter", OtClassClass::create(OtMeterClass::getMeta()));
		module->set("Picture", OtClassClass::create(OtPictureClass::getMeta()));
		module->set("RealSlider", OtClassClass::create(OtRealSliderClass::getMeta()));
		module->set("TreeNode", OtClassClass::create(OtTreeNodeClass::getMeta()));
		module->set("Tron", OtClassClass::create(OtTronClass::getMeta()));

		module->set("AmbientLightController", OtClassClass::create(OtAmbientLightControllerClass::getMeta()));
		module->set("BlendMappedMaterialController", OtClassClass::create(OtBlendMappedMaterialControllerClass::getMeta()));
		module->set("CameraController", OtClassClass::create(OtCameraControllerClass::getMeta()));
		module->set("DirectionalLightController", OtClassClass::create(OtDirectionalLightControllerClass::getMeta()));
		module->set("FogController", OtClassClass::create(OtFogControllerClass::getMeta()));
		module->set("NoiseMapController", OtClassClass::create(OtNoiseMapControllerClass::getMeta()));
		module->set("HeightMapController", OtClassClass::create(OtHeightMapControllerClass::getMeta()));
		module->set("PointLightController", OtClassClass::create(OtPointLightControllerClass::getMeta()));
		module->set("SkyController", OtClassClass::create(OtSkyControllerClass::getMeta()));
		module->set("SunController", OtClassClass::create(OtSunControllerClass::getMeta()));
		module->set("SpotLightController", OtClassClass::create(OtSpotLightControllerClass::getMeta()));
		module->set("TerrainController", OtClassClass::create(OtTerrainControllerClass::getMeta()));
		module->set("TerrainMapController", OtClassClass::create(OtTerrainMapControllerClass::getMeta()));
		module->set("VectorDisplayController", OtClassClass::create(OtVectorDisplayControllerClass::getMeta()));
		module->set("WaterController", OtClassClass::create(OtWaterControllerClass::getMeta()));

		// register all the framework ENUMs
		OtFramework framework = OtFrameworkClass::instance();
		framework->addEnumsGLFW(module);
		framework->addEnumsIMGUI(module);
	});

	// connect to OS class so GUI apps can run
	OtOSClass::instance()->registerGUI(
		[]() {
			// sanity check
			if (!app) {
				OtExcept("You can't run a GUI without an instance of the [App] class");
			}

			// connect app to framework and run it
			auto framework = OtFrameworkClass::instance();
			framework->addCustomer(app);
			framework->run();
		},

		[](OtException e) {
			// remove app from framework
			// this is not required but mentioned here to keep code symetrical
			// OtFrameworkClass::instance()->removeCustomer(app);

			// output error message and terminate program
			std::wcerr << "Error: " << e.what() << std::endl;
			std::_Exit(EXIT_FAILURE);
		},

		[]() {
			auto framework = OtFrameworkClass::instance();

			// remove app from framework
			// this is not required but mentioned here to keep code symetrical
			// framework->removeCustomer(app);

			// stop the framework which will exit the program
			framework->stop();
		});
}


//
//	OtGuiClass::registerApp
//

void OtGuiClass::registerApp(OtAppClass* object) {
	// sanity check
	if (app) {
		OtExcept("You can only have one [App] instance per running program");
	}

	app = object;
}


//
//	OtGuiClass::unregisterApp
//

void OtGuiClass::unregisterApp() {
	// sanity check
	if (!app) {
		OtExcept("Internal error: you can't unregister a non-existing [App]");
	}

	app = nullptr;
}


//
//	OtGuiClass::getMeta
//

OtType OtGuiClass::getMeta() {
	static OtType type;

	if (!type) {
		type = OtTypeClass::create<OtGuiClass>("GUI", OtObjectClass::getMeta());
	}

	return type;
}
