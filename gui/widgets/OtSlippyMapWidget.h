//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "glm/glm.hpp"

#include "OtLatLon.h"
#include "OtSlippyMap.h"
#include "OtWidget.h"


//
//	OtSlippyMapWidgetClass
//

class OtSlippyMapWidgetClass;
using OtSlippyMapWidget = OtObjectPointer<OtSlippyMapWidgetClass>;

class OtSlippyMapWidgetClass : public OtWidgetClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// add OpenStreetMap layer
	inline void addOsmLayer(const std::string& host) { map.addOsmLayer(host); }

	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtSlippyMapWidgetClass>;
	OtSlippyMapWidgetClass() = default;

private:
	// properties
	OtSlippyMap map;
	std::string host;
	OtLatLon center;
	double resolution = 0.0;
};
