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
//	OtGuiClass::registerModule
//

void OtGuiClass::registerModule() {
	OtModuleClass::registerInternal("gui", [](OtModule module) {
		module->set("GUI", OtClass::create(OtGuiClass::getMeta()));

		module->set("App", OtClass::create(OtAppClass::getMeta()));
		module->set("AppMenubar", OtClass::create(OtAppMenubarClass::getMeta()));
		module->set("Window", OtClass::create(OtWindowClass::getMeta()));
		module->set("Dialog", OtClass::create(OtDialogClass::getMeta()));
		module->set("FileOpenDialog", OtClass::create(OtFileOpenDialogClass::getMeta()));
		module->set("FileSaveDialog", OtClass::create(OtFileSaveDialogClass::getMeta()));
		module->set("Panel", OtClass::create(OtPanelClass::getMeta()));
		module->set("View", OtClass::create(OtViewClass::getMeta()));
		module->set("VectorDisplay", OtClass::create(OtVectorDisplayClass::getMeta()));

		module->set("Camera", OtClass::create(OtCameraClass::getMeta()));
		module->set("Scene", OtClass::create(OtSceneClass::getMeta()));

		module->set("SceneObject", OtClass::create(OtSceneObjectClass::getMeta()));
		module->set("Skybox", OtClass::create(OtSkyboxClass::getMeta()));
		module->set("Sky", OtClass::create(OtSkyClass::getMeta()));
		module->set("Water", OtClass::create(OtWaterClass::getMeta()));
		module->set("Terrain", OtClass::create(OtTerrainClass::getMeta()));
		module->set("TerrainMap", OtClass::create(OtTerrainMapClass::getMeta()));

		module->set("AmbientLight", OtClass::create(OtAmbientLightClass::getMeta()));
		module->set("DirectionalLight", OtClass::create(OtDirectionalLightClass::getMeta()));
		module->set("Fog", OtClass::create(OtFogClass::getMeta()));
		module->set("PointLight", OtClass::create(OtPointLightClass::getMeta()));
		module->set("SpotLight", OtClass::create(OtSpotLightClass::getMeta()));
		module->set("Sun", OtClass::create(OtSunClass::getMeta()));

		module->set("BlendMappedMaterial", OtClass::create(OtBlendMappedMaterialClass::getMeta()));
		module->set("ColoredMaterial", OtClass::create(OtColoredMaterialClass::getMeta()));
		module->set("FixedMaterial", OtClass::create(OtFixedMaterialClass::getMeta()));
		module->set("TexturedMaterial", OtClass::create(OtTexturedMaterialClass::getMeta()));

		module->set("Object3D", OtClass::create(OtObject3dClass::getMeta()));
		module->set("Mesh", OtClass::create(OtMeshClass::getMeta()));
		module->set("Group", OtClass::create(OtGroupClass::getMeta()));

		module->set("Font", OtClass::create(OtFontClass::getMeta()));
		module->set("HeightMap", OtClass::create(OtHeightMapClass::getMeta()));
		module->set("NoiseMap", OtClass::create(OtNoiseMapClass::getMeta()));
		module->set("Matrix", OtClass::create(OtMatrixClass::getMeta()));

		module->set("getDefaultMaterialNames", OtFunction::create(&OtColoredMaterialClass::getDefaultMaterialNames));
		module->set("loadObj", OtFunction::create(&OtObjLoad));

		module->set("Geometry", OtClass::create(OtGeometryClass::getMeta()));
		module->set("BoxGeometry", OtClass::create(OtBoxGeometryClass::getMeta()));
		module->set("CircleGeometry", OtClass::create(OtCircleGeometryClass::getMeta()));
		module->set("CylinderGeometry", OtClass::create(OtCylinderGeometryClass::getMeta()));
		module->set("DodecahedronGeometry", OtClass::create(OtDodecahedronGeometryClass::getMeta()));
		module->set("ExtrudedGeometry", OtClass::create(OtExtrudedGeometryClass::getMeta()));
		module->set("IcosahedronGeometry", OtClass::create(OtIcosahedronGeometryClass::getMeta()));
		module->set("ModelGeometry", OtClass::create(OtModelGeometryClass::getMeta()));
		module->set("OctahedronGeometry", OtClass::create(OtOctahedronGeometryClass::getMeta()));
		module->set("PlaneGeometry", OtClass::create(OtPlaneGeometryClass::getMeta()));
		module->set("SphereGeometry", OtClass::create(OtSphereGeometryClass::getMeta()));
		module->set("SphericalCubeGeometry", OtClass::create(OtSphericalCubeGeometryClass::getMeta()));
		module->set("TetrahedronGeometry", OtClass::create(OtTetrahedronGeometryClass::getMeta()));
		module->set("TorusGeometry", OtClass::create(OtTorusGeometryClass::getMeta()));

		module->set("Animation", OtClass::create(OtAnimationClass::getMeta()));

		module->set("Simulation", OtClass::create(OtSimulationClass::getMeta()));
		module->set("Cloth", OtClass::create(OtClothClass::getMeta()));
		module->set("World", OtClass::create(OtWorldClass::getMeta()));
		module->set("Body", OtClass::create(OtBodyClass::getMeta()));

		module->set("Shape", OtClass::create(OtShapeClass::getMeta()));

		module->set("Widget", OtClass::create(OtWidgetClass::getMeta()));
		module->set("Checkbox", OtClass::create(OtCheckboxClass::getMeta()));
		module->set("Combobox", OtClass::create(OtComboboxClass::getMeta()));
		module->set("Filler", OtClass::create(OtFillerClass::getMeta()));
		module->set("IntegerSlider", OtClass::create(OtIntegerSliderClass::getMeta()));
		module->set("Knob", OtClass::create(OtKnobClass::getMeta()));
		module->set("Label", OtClass::create(OtLabelClass::getMeta()));
		module->set("Menubar", OtClass::create(OtMenubarClass::getMeta()));
		module->set("Menu", OtClass::create(OtMenuClass::getMeta()));
		module->set("MenuItem", OtClass::create(OtMenuItemClass::getMeta()));
		module->set("Meter", OtClass::create(OtMeterClass::getMeta()));
		module->set("Picture", OtClass::create(OtPictureClass::getMeta()));
		module->set("RealSlider", OtClass::create(OtRealSliderClass::getMeta()));
		module->set("TreeNode", OtClass::create(OtTreeNodeClass::getMeta()));
		module->set("Tron", OtClass::create(OtTronClass::getMeta()));

		module->set("AmbientLightController", OtClass::create(OtAmbientLightControllerClass::getMeta()));
		module->set("BlendMappedMaterialController", OtClass::create(OtBlendMappedMaterialControllerClass::getMeta()));
		module->set("CameraController", OtClass::create(OtCameraControllerClass::getMeta()));
		module->set("DirectionalLightController", OtClass::create(OtDirectionalLightControllerClass::getMeta()));
		module->set("FogController", OtClass::create(OtFogControllerClass::getMeta()));
		module->set("NoiseMapController", OtClass::create(OtNoiseMapControllerClass::getMeta()));
		module->set("HeightMapController", OtClass::create(OtHeightMapControllerClass::getMeta()));
		module->set("PointLightController", OtClass::create(OtPointLightControllerClass::getMeta()));
		module->set("SkyController", OtClass::create(OtSkyControllerClass::getMeta()));
		module->set("SunController", OtClass::create(OtSunControllerClass::getMeta()));
		module->set("SpotLightController", OtClass::create(OtSpotLightControllerClass::getMeta()));
		module->set("TerrainController", OtClass::create(OtTerrainControllerClass::getMeta()));
		module->set("TerrainMapController", OtClass::create(OtTerrainMapControllerClass::getMeta()));
		module->set("VectorDisplayController", OtClass::create(OtVectorDisplayControllerClass::getMeta()));
		module->set("WaterController", OtClass::create(OtWaterControllerClass::getMeta()));

		// register all the framework ENUMs
		OtFramework framework = OtFrameworkClass::instance();
		framework->addEnumsGLFW(module);
		framework->addEnumsIMGUI(module);
	});
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
