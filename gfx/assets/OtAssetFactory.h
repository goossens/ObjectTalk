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
#include <unordered_map>

#include "OtSingleton.h"

#include "OtAssetBase.h"


//
//	OtAssetFactory
//

class OtAssetFactory : public OtSingleton<OtAssetFactory> {
	// asset creator function
	using AssetCreator = OtAssetBase*(*)();

public:
	// register a new asset type
	void registerType(const char* extention, AssetCreator creator) {
		registry[extention] = creator;
	}

	// create a new instance
	OtAssetBase* instantiate(const std::filesystem::path& path) {
		auto ext = path.extension().string();
		return registry.find(ext) == registry.end() ? nullptr : registry[ext]();
	}

private:
	// the list of assets types (by file extension) that we can generate
	std::unordered_map<std::string, AssetCreator> registry;
};


//
//	OtAssetFactoryRegister
//

template <typename T>
class OtAssetFactoryRegister {
public:
	// constructor
	OtAssetFactoryRegister(const char* ext) {
		OtAssetFactory::instance()->registerType(ext, []() {
			return static_cast<OtAssetBase*>(new T());
		});
	}
};
