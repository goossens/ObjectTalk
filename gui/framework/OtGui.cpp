//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtClass.h"
#include "OtFunction.h"
#include "OtModule.h"

#include "OtFramework.h"
#include "OtComponent.h"

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
#include "OtAmbient.h"
#include "OtBackground.h"
#include "OtLight.h"
#include "OtFog.h"
#include "OtSky.h"
#include "OtSkybox.h"
#include "OtSun.h"
#include "OtTerrain.h"
#include "OtTerrainMap.h"
#include "OtWater.h"

#include "OtObject3d.h"
#include "OtGroup.h"
#include "OtMesh.h"

#include "OtBlendMap.h"
#include "OtFont.h"
#include "OtHeightMap.h"
#include "OtMaterial.h"
#include "OtNoiseMap.h"
#include "OtTexture.h"
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

#include "OtShape.h"

#include "OtMath.h"
#include "OtPerlin.h"
#include "OtPlane.h"
#include "OtVec2.h"
#include "OtVec3.h"


//
//	OtGuiClass::registerModule
//

void OtGuiClass::registerModule() {
	OtModuleClass::registerInternal("gui", [](OtModule module) {
		module->set("GUI", OtClassClass::create(OtGuiClass::getMeta()));
		module->set("Component", OtClassClass::create(OtComponentClass::getMeta()));

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
		module->set("Ambient", OtClassClass::create(OtAmbientClass::getMeta()));
		module->set("Background", OtClassClass::create(OtBackgroundClass::getMeta()));
		module->set("Light", OtClassClass::create(OtLightClass::getMeta()));
		module->set("Fog", OtClassClass::create(OtFogClass::getMeta()));
		module->set("Skybox", OtClassClass::create(OtSkyboxClass::getMeta()));
		module->set("Sun", OtClassClass::create(OtSunClass::getMeta()));
		module->set("Sky", OtClassClass::create(OtSkyClass::getMeta()));
		module->set("Water", OtClassClass::create(OtWaterClass::getMeta()));
		module->set("Terrain", OtClassClass::create(OtTerrainClass::getMeta()));
		module->set("TerrainMap", OtClassClass::create(OtTerrainMapClass::getMeta()));

		module->set("Object3D", OtClassClass::create(OtObject3dClass::getMeta()));
		module->set("Mesh", OtClassClass::create(OtMeshClass::getMeta()));
		module->set("Group", OtClassClass::create(OtGroupClass::getMeta()));

		module->set("BlendMap", OtClassClass::create(OtBlendMapClass::getMeta()));
		module->set("Font", OtClassClass::create(OtFontClass::getMeta()));
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

		module->set("Shape", OtClassClass::create(OtShapeClass::getMeta()));

		module->set("Math", OtClassClass::create(OtSimulationClass::getMeta()));
		module->set("Perlin", OtClassClass::create(OtPerlinClass::getMeta()));
		module->set("Vec2", OtClassClass::create(OtVec2Class::getMeta()));
		module->set("Vec3", OtClassClass::create(OtVec3Class::getMeta()));

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

		module->set("AmbientController", OtClassClass::create(OtAmbientControllerClass::getMeta()));
		module->set("CameraController", OtClassClass::create(OtCameraControllerClass::getMeta()));
		module->set("LightController", OtClassClass::create(OtLightControllerClass::getMeta()));
		module->set("FogController", OtClassClass::create(OtFogControllerClass::getMeta()));
		module->set("NoiseMapController", OtClassClass::create(OtNoiseMapControllerClass::getMeta()));
		module->set("HeightMapController", OtClassClass::create(OtHeightMapControllerClass::getMeta()));
		module->set("SkyController", OtClassClass::create(OtSkyControllerClass::getMeta()));
		module->set("SunController", OtClassClass::create(OtSunControllerClass::getMeta()));
		module->set("TerrainController", OtClassClass::create(OtTerrainControllerClass::getMeta()));
		module->set("TerrainMapController", OtClassClass::create(OtTerrainMapControllerClass::getMeta()));
		module->set("VectorDisplayController", OtClassClass::create(OtVectorDisplayControllerClass::getMeta()));
		module->set("WaterController", OtClassClass::create(OtWaterControllerClass::getMeta()));

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
