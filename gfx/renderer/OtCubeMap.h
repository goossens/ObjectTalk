//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <filesystem>
#include <string>

#include "OtBgfxHandle.h"


//
//	OtCubeMap
//

class OtCubeMap {
public:
	// load cubemap from the specified file
	void load(const std::filesystem::path& path);

	// clear the resources
	void clear() { cubemap.clear(); }

	// see if cubemap is valid
	bool isValid() { return cubemap.isValid(); }

	// return cubemap handle
	bgfx::TextureHandle getTextureHandle() { return cubemap.getHandle(); }

	// return cubemap index
	int getTextureIndex() { return cubemap.getIndex(); }

private:
	// cubemap texture
	OtBgfxHandle<bgfx::TextureHandle> cubemap;

	// specific cubemap loaders
	void loadJSON(const std::filesystem::path& path);
	void loadCubemapImage(const std::filesystem::path& path);
};
