//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cmath>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>

#include "OtLog.h"

#include "OtAudioBuffer.h"

//
//	OtWaveTable
//

class OtWaveTable {
public:
	// load a wave table from a file
	inline void load(const std::string& path) {
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

		while (tryToReadData(ifs, chunkType.data(), 4)) {
			auto chunkSize = readInt4(ifs);

			if (chunkType == "fmt ") {
				if (readInt2(ifs) != 3) {
					OtLogError("Invalid WAV file [{}]: samples are not in floating point format", path);
				}

				if (readInt2(ifs) != 1) {
					OtLogError("Invalid WAV file [{}]: only mono WAV files are supported", path);
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
					samples = 2048;
				}

			} else if (chunkType == "uhWT") {
				ifs.seekg(chunkSize, std::ios::cur);
				samples = 2048;

			} else if (chunkType == "data") {
				auto sampleCount = chunkSize / 4;
				buffer = std::make_shared<OtAudioBuffer>(1, sampleCount);
				readData(ifs, reinterpret_cast<char*>(buffer->data()), chunkSize);

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

		// set parameters
		if (samples == 0) {
			samples = buffer->getSampleCount();
		}

		shapes = buffer->getSampleCount() / samples;
		sampleLimit = static_cast<float>(samples) - 1.0001f;
		shapeLimit = static_cast<float>(shapes) - 1.0001f;
	}

	// clear the wave table
	inline void clear() {
		buffer = nullptr;
		samples = 0;
		shapes = 0;
		sampleLimit = 0.0f;
		shapeLimit = 0.0f;
	}

	// get sample at specified location
	// (sample and shape are clamped to wave table range)
	// (shape can be negative but is rectified before clamping)
	inline float get(float sample, float shape) {
		if (buffer) {
			sample = std::clamp(sample * sampleLimit, 0.0f, sampleLimit);
			shape = std::clamp(std::abs(shape) * shapeLimit, 0.0f, shapeLimit);

			auto lowX = static_cast<size_t>(sample);
			auto highX = lowX + 1;
			auto ratioX = sample - static_cast<float>(lowX);

			auto lowY = static_cast<size_t>(shape);
			auto highY = lowY + 1;
			auto ratioY = shape - static_cast<float>(lowY);

			auto v1 = buffer->get(0, lowY * samples + lowX);
			auto v2 = buffer->get(0, lowY * samples + highX);
			auto v3 = buffer->get(0, highY * samples + lowX);
			auto v4 = buffer->get(0, highY * samples + highX);

			auto x1 = std::lerp(v1, v2, ratioX);
			auto x2 = std::lerp(v3, v4, ratioX);
			return std::lerp(x1, x2, ratioY);

		} else {
			return 0.0f;
		}
	}

	// get wave table information
	inline bool isValid() { return buffer != nullptr; }
	inline size_t getSampleCount() { return samples; }
	inline size_t getShapeCount() { return shapes; }
	inline float* data() { return buffer->data(); }
	inline size_t size() { return buffer->size(); }

	// see if wave tables are identical
	inline bool operator==(OtWaveTable& rhs) {
		return buffer == rhs.buffer;
	}

	inline bool operator!=(OtWaveTable& rhs) {
		return !operator==(rhs);
	}

private:
	// properties
	std::shared_ptr<OtAudioBuffer> buffer;

	// work variables
	size_t samples = 0;
	size_t shapes = 0;
	float sampleLimit = 0.0f;
	float shapeLimit = 0.0f;

	// read a fixed length character stream and convert to std::string
	inline std::string readString(std::ifstream& ifs, size_t size) {
		std::string data(size, 0);
		readData(ifs, data.data(), size);
		return data;
	}

	// read 4 byte little endian number and convert to size_t
	inline size_t readInt4(std::ifstream& ifs) {
		std::byte data[4];
		readData(ifs, reinterpret_cast<char*>(data), 4);

		return
			(static_cast<size_t>(data[3]) << 24) |
			(static_cast<size_t>(data[2]) << 16) |
			(static_cast<size_t>(data[1]) << 8) |
			 static_cast<size_t>(data[0]);
	}

	// read 2 byte little endian number and convert to size_t
	inline size_t readInt2(std::ifstream& ifs) {
		std::byte data[2];
		readData(ifs, reinterpret_cast<char*>(data), 2);
		return (static_cast<size_t>(data[1]) << 8) | static_cast<size_t>(data[0]);
	}

	// read a number of bytes and store in provided buffer
	inline void readData(std::ifstream& ifs, char* data, size_t size) {
		ifs.read(data, size);

		if (ifs.eof()) {
			OtLogError("Corrupt WAV file");
		}
	}

	inline bool tryToReadData(std::ifstream& ifs, char* data, size_t size) {
		ifs.read(data, size);
		return !ifs.eof();
	}
};
