//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "bgfx/bgfx.h"


//
//	OtCubeMap
//

class OtCubeMap {
public:
	// constructors/destructor
	OtCubeMap() = default;
	OtCubeMap(const std::string& filename);
	~OtCubeMap();

	// load cubemap from the size specified images
	void load(const std::string& posx, const std::string& negx, const std::string& posy, const std::string& negy, const std::string& posz, const std::string& negz);

	// clear the resources
	void clear();

	// see if cubemap is valid
	bool isValid() { return bgfx::isValid(cubemap); }

	// return cubemap handle
	bgfx::TextureHandle getTextureHandle() { return cubemap; }

	// return cubemap index
	int getTextureIndex() { return cubemap.idx; }

private:
	// cubemap texture
	bgfx::TextureHandle cubemap = BGFX_INVALID_HANDLE;
};
