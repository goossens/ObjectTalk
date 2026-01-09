//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>
#include <cstring>

#include "OtNumbers.h"

#include "OtWaveTable.h"


//
//	OtWaveTable::load
//

void OtWaveTable::load(const std::string& path) {
	// clear old state (if required)
	clear();

	// open wave table
	std::ifstream ifs(path, std::ios::in | std::ios::binary);

	if (!ifs) {
		OtLogError("Can'sample open WAV file [{}]", path);
	}

	// read header
	auto riff = readString(ifs, 4);
	readInt4(ifs);
	auto wave = readString(ifs, 4);

	// validate header
	if (riff != "RIFF") {
		OtLogError("Invalid WAV file [{}]", path);
	}

	if (wave != "WAVE") {
		OtLogError("Invalid WAV file [{}]", path);
	}

	// read and process all chunks
	std::string chunkType(4, 0);
	size_t channels = 1;

	while (tryToReadData(ifs, chunkType.data(), 4)) {
		auto chunkSize = readInt4(ifs);

		if (chunkType == "fmt ") {
			if (readInt2(ifs) != 3) {
				OtLogError("Invalid WAV file [{}]: samples are not in floating point format", path);
			}

			channels = readInt2(ifs);

			if (channels > 2) {
				OtLogError("Invalid WAV file [{}]: only mono and stereo WAV files are supported", path);
			}

			readInt4(ifs);
			readInt4(ifs);
			readInt2(ifs);

			if (readInt2(ifs) != 32) {
				OtLogError("Invalid WAV file [{}]: only 32-bit floating point WAV files are supported", path);
			}

			ifs.seekg(chunkSize - 16, std::ios::cur);

		} else if (chunkType == "clm ") {
			auto clm = readString(ifs, chunkSize);

			if (clm.rfind("<!>2048", 0) == 0) {
				samplesPerShape = 2048;
			}

		} else if (chunkType == "uhWT") {
			ifs.seekg(chunkSize, std::ios::cur);
			samplesPerShape = 2048;

		} else if (chunkType == "data") {
			if (channels == 1) {
				// read mono file
				buffer = std::make_shared<OtAudioBuffer>(1, chunkSize / 4);
				readData(ifs, reinterpret_cast<char*>(buffer->data()), chunkSize);

			} else if (channels == 2) {
				// read stereo file (just extract left channel)
				auto fileSamples = chunkSize / 2 / 4;
				auto tmp = std::make_shared<OtAudioBuffer>(2, fileSamples);
				readData(ifs, reinterpret_cast<char*>(tmp->data()), chunkSize);
				buffer = std::make_shared<OtAudioBuffer>(1, fileSamples);

				for (size_t i = 0; i < fileSamples; i++) {
					buffer->set(0, i, tmp->get(0, i));
				}
			}

		} else {
			ifs.seekg(chunkSize, std::ios::cur);
		}
	}

	// close the file
	ifs.close();

	// final sanity check
	if (!buffer) {
		OtLogError("Invalid WAV file [{}]: no samples found", path);
	}

	if (samplesPerShape == 0) {
		samplesPerShape = buffer->getSampleCount();
	}

	// set parameters
	shapes = buffer->getSampleCount() / samplesPerShape;
	sampleLimit = static_cast<float>(samplesPerShape) - 1.0001f;
	shapeLimit = static_cast<float>(shapes) - 1.0001f;
}


//
//	OtWaveTable::clear
//

void OtWaveTable::clear() {
	buffer = nullptr;
	samplesPerShape = 0;
	shapes = 0;
	sampleLimit = 0.0f;
	shapeLimit = 0.0f;
}


//
//	OtWaveTable::get
//

float OtWaveTable::get(float sample, float shape) {
	if (buffer) {
		sample = std::clamp(sample * sampleLimit, 0.0f, sampleLimit);

		auto lowX = static_cast<size_t>(sample);
		auto highX = lowX + 1;
		auto ratioX = sample - static_cast<float>(lowX);

		if (shapes == 1) {
			// interpolate value based on sample
			return std::lerp(buffer->get(0, lowX), buffer->get(0, highX), ratioX);

		} else {
			// interpolate 2D values based on sample and shape
			// (sample and shape values are clamped to wave table range)
			// (shape value can be negative but is rectified before clamping)
			sample = std::clamp(sample * sampleLimit, 0.0f, sampleLimit);
			shape = std::clamp(std::abs(shape) * shapeLimit, 0.0f, shapeLimit);

			auto lowY = static_cast<size_t>(shape);
			auto highY = lowY + 1;
			auto ratioY = shape - static_cast<float>(lowY);

			auto v1 = buffer->get(0, lowY * samplesPerShape + lowX);
			auto v2 = buffer->get(0, lowY * samplesPerShape + highX);
			auto v3 = buffer->get(0, highY * samplesPerShape + lowX);
			auto v4 = buffer->get(0, highY * samplesPerShape + highX);

			auto x1 = std::lerp(v1, v2, ratioX);
			auto x2 = std::lerp(v3, v4, ratioX);
			return std::lerp(x1, x2, ratioY);
		}

	} else {
		return 0.0f;
	}
}
