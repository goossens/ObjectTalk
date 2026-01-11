//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtAsset.h"

#include "OtOneShotFile.h"
#include "OtOneShotFileAsset.h"
#include "OtSynthSound.h"


//
//	OtOneShotSound
//

class OtOneShotSound : public OtSynthSound {
public:
	// see if sound is ready to be used
	bool isReady() override;

	// UI to change sound properties
	bool renderSound() override;

	// (de)serialize component
	void serialize(nlohmann::json* data, std::string* basedir) override;
	void deserialize(nlohmann::json* data, std::string* basedir) override;

	// get sound type name
	const char* getTypeName() override { return name; }
	static constexpr char const* name = "OneShot";

private:
	// properties
	friend class OtSynthVoice;

	OtAsset<OtOneShotFileAsset> asset;
	OtOneShotFile sound;
};
