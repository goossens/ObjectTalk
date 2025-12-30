//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAsset.h"
#include "OtAssetBase.h"

#include "OtAudio.h"


//
//	OtAudioAsset
//

class OtAudioAsset : public OtAssetBase {
public:
	// access the audio
	inline OtAudio& getAudio() { return audio; }

	// asset properties
	static constexpr bool hasEditor = true;
	static constexpr bool canHandleVirtual = false;
	static constexpr const char* supportedFileTypes = ".ota";
	inline const char* getSupportedFileTypes() override { return supportedFileTypes; }

protected:
	// load the audio
	OtAssetBase::State load() override {
		try {
			audio.load(path);
			return State::ready;

		} catch (const OtException& exception) {
			errorMessage = exception.what();
			return State::invalid;
		}
	}

private:
	// the actual audio
	OtAudio audio;
};
