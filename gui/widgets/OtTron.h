//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <vector>

#include "imgui.h"

#include "OtWidget.h"


//
//	OtTronClass
//

class OtTronClass;
using OtTron = OtObjectPointer<OtTronClass>;

class OtTronClass : public OtWidgetClass {
public:
	// render content
	void render() override;

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtTronClass>;
	OtTronClass() = default;

private:
	// create a new set of arcs
	void createArcs();

	// state parameters
	bool opening = false;
	bool closing = true;
	float tronScale = 0.0f;
	float tronRotation = 0.0f;

	// definition of one arc
	class OtTronArc {
	public:
		float start = 0.0f;
		float end = 0.0f;
		float radius = 300.0f;
		float width = 5.0f;
		float rotation = 0.0f;
		ImU32 color;
		bool clockwise = true;
	};

	std::vector<OtTronArc> arcs;

	// color management
	static constexpr ImU32 color100 = IM_COL32(100, 180, 255, 255);
	static constexpr ImU32 color80 = IM_COL32(80, 144, 200, 255);
	static constexpr ImU32 color60 = IM_COL32(60, 108, 150, 255);
	static constexpr ImU32 color40 = IM_COL32(40, 72, 100, 255);
	static constexpr ImU32 color20 = IM_COL32(20, 36, 50, 255);
	static constexpr ImU32 background = IM_COL32(0, 12, 24, 255);

	ImU32 blend(ImU32 color1, ImU32 color2, float ratio);
};
