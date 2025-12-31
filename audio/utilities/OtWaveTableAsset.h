//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
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

#include "OtWaveTable.h"


//
//	OtWaveTableAsset
//

class OtWaveTableAsset : public OtAssetBase {
public:
	// access the wave table
	inline OtWaveTable& getWaveTable() { return wavetable; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".wav";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the wave table
	OtAssetBase::State load() override {
		try {
			wavetable.load(path);
			return State::ready;

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

private:
	// the actual wave table
	OtWaveTable wavetable;
};
