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

#include "OtAssetBase.h"

#include "OtSamplerSound.h"


//
//	OtSamplerSoundAsset
//

class OtSamplerSoundAsset : public OtAssetBase {
public:
	// access the MIDI file
	inline OtSamplerSound& getSamplerSound() { return samplefile; }

	// asset properties
	static constexpr bool hasEditor = false;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".wav";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the MIDI file
	OtAssetBase::State load() override {
		try {
			samplefile.load(path);
			return State::ready;

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

private:
	// the actual MIDI file
	OtSamplerSound samplefile;
};
