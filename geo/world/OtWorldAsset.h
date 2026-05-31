//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtException.h"

#include "OtAsset.h"
#include "OtAssetBase.h"
#include "OtPath.h"

#include "OtWorld.h"


//
//	OtWorldAsset
//

class OtWorldAsset : public OtAssetBase {
public:
	// access the world
	inline OtWorld& getWorld() { return world; }
	inline void setWorld(OtWorld& w) { world = w; state = State::ready; }
	inline void clearWorld() { world.clear(); state = State::missing; }

	// asset properties
	static constexpr bool hasEditor = true;
	static constexpr bool canHandleVirtual = true;
	static constexpr const char* supportedFileTypes = ".otw";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the asset
	OtAssetBase::State load() override {
		try {
			// load the world
			if (!isVirtual()) {
				world.load(path);
			}

			world.generate();
			return State::ready;

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

private:
	// the actual world
	OtWorld world;
};
