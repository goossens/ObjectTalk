//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//  Include files
//

#include <list>
#include <memory>

#include "OtSlippyMap.h"


//
//	OtSlippyMap::render
//

void OtSlippyMap::render(const char* label, OtLatLon& center, double& resolution, ImVec2 size) {
	// use all available space by default
	if (size.x == 0.0f || size.y == 0.0f) {
		size = ImGui::GetContentRegionAvail();
	}

	// set default resolution (if required)
	if (resolution == 0.0) {
		resolution = 2.0 / static_cast<double>(size.x);
	}

	// setup screen context
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* drawlist = ImGui::GetWindowDrawList();
	ImGui::InvisibleButton(label, size);
	ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY);

	// handle user interactions
	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
			center = projector.screenToLatLon(ImGui::GetMousePos());
			resolution /= 2.0;

		} else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			auto raster = projector.latLonToRaster(center);
			auto mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

			raster.x -= static_cast<double>(mouseDelta.x) * resolution;
			raster.y -= static_cast<double>(mouseDelta.y) * resolution;

			center = projector.rasterToLatLon(raster);
			ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

		} else {
			auto zoom = ImGui::GetIO().MouseWheel;

			if (zoom != 0.0f) {
				resolution *= 1.0f + zoom * 0.05f;
			}
		}
	}

	// update projector
	projector.setWindowSize(size);
	projector.setWindowOffset(p);

	projector.setCenter(center);
	projector.setResolution(resolution);

	// render map layers
	for (auto& layer : layers) {
		layer->render(drawlist, projector);
	}
}


// //
// //  MtMap::onDoubleTouchGesture
// //

// void MtMap::onDoubleTouchGesture(int id, const MtPoint& touch)
// {
// 	// animate map pan and zoom
// 	glm::vec2 cen = projector.latLonToRaster(projector.getCenter());
// 	double resolution = projector.getResolution();
// 	cen.x += touch.x * resolution;
// 	cen.y += touch.y * resolution;
// 	projector.flyTo(projector.rasterToLatLon(cen), projector.getResolution() / 2.0);
// }


// //
// //  MtMap::onMoveGesture
// //

// void MtMap::onMoveGesture(int id, const MtPoint& touch, const MtPoint& delta)
// {
// 	// determine new map center
// 	glm::vec2 cen = projector.latLonToRaster(projector.getCenter());
// 	double resolution = projector.getResolution();
// 	cen.x -= delta.x * resolution;
// 	cen.y -= delta.y * resolution;
// 	projector.setCenter(projector.rasterToLatLon(cen));
// }


// //
// //  MtMap::onInertiaGesture
// //

// void MtMap::onInertiaGesture(int id, const MtPoint& touch, const MtPoint& delta)
// {
// 	onMoveGesture(id, touch, delta);
// }


// //
// //  MtMap::onZoomRotateGesture
// //

// void MtMap::onZoomRotateGesture(int id, const MtPoint& delta, float scale, float rotation)
// {
// 		projector.setResolution(projector.getResolution() / scale);
// }
