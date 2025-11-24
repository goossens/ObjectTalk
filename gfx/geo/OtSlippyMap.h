//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>
#include <vector>

#include "imgui.h"

#include "OtLatLon.h"
#include "OtMapLayer.h"
#include "OtProjector.h"
#include "OtTileLayer.h"


//
//	OtSlippyMap class
//

class OtSlippyMap {
public:
	// manage layers
	void addLayer(std::shared_ptr<OtMapLayer> layer) { layers.push_back(layer); }
	inline void addOsmLayer(const std::string& host) { addLayer(std::make_shared<OtTileLayer>(host)); }

	// render map
	void render(const char* label, OtLatLon& center, double& resolution, ImVec2 size=ImVec2());

private:
	// properties
	OtProjector projector;
	std::vector<std::shared_ptr<OtMapLayer>> layers;
};
