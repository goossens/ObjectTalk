//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>
#include <cstdint>


//
//	OtDejaVu
//

class OtDejaVu {
public:
	// access font
	inline static uint8_t* data() { return fontData; }
	inline static size_t size() { return fontSize; }

private:
	static uint8_t fontData[340712];
	inline static constexpr size_t fontSize = 340712;
};
