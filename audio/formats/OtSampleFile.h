//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>
#include <string>

#include "SDL3/SDL.h"

#include "OtSampleBuffer.h"


//
//	OtSampleFile
//

class OtSampleFile {
public:
	// load the specified samples file);
	void load(const std::string& path);

	// clear the samples
	inline void clear() { samples = nullptr; }

	// get information
	inline bool isValid() { return samples != nullptr; }

	// see if samples are identical
	inline bool operator==(OtSampleFile& rhs) { return samples == rhs.samples; }
	inline bool operator!=(OtSampleFile& rhs) { return !operator==(rhs); }

	// get sample at specified location
	void start(float frequency=0.0f);
	float get();

	private:
	// properties
	std::shared_ptr<OtSampleBuffer> samples;
	float rootFrequency = 0.0f;
	float dt;
	float offset;
};
