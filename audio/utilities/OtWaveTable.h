//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstring>
#include <fstream>
#include <memory>
#include <string>

#include "OtLog.h"
#include "OtNumbers.h"

#include "OtAudioBuffer.h"
#include "OtAudioUtilities.h"

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
			OtLogError("Can't open WAV file [{}]", path);
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
					loopSize = 2048;
				}

			} else if (chunkType == "uhWT") {
				ifs.seekg(chunkSize, std::ios::cur);
				loopSize = 2048;

			} else if (chunkType == "data") {
				auto samples = chunkSize / 4;
				buffer = std::make_shared<OtAudioBuffer>(1, samples);
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

		// set loop parameters
		if (loopSize == 0) {
			loopSize = buffer->getSampleCount();
		}

		loopCount = buffer->getSampleCount() / loopSize;
		loopLimit = static_cast<float>(loopSize);
	}

	// clear the wave table
	inline void clear() {
		buffer = nullptr;
		loopSize = 0;
		loopCount = 0;
		loopLimit = 0.0f;

		lastFrequency = -1.0f;
		t = 0.0f;
		dt = 0.0f;
	}

	// get the next sample
	inline float get(float frequency) {
		if (isValid()) {
			if (frequency != lastFrequency) {
				lastFrequency = frequency;
				dt = frequency * loopLimit * OtAudioSettings::dt;
			}

			auto result = getSample(t);
			t += dt;

			if (t >= loopLimit) {
				t -= loopLimit;
			}

			return result;

		} else {
			return 0.0f;
		}
	}

	// synchronize phase
	inline void synchronize(float phase) {
		t = OtAudioUtilities::fraction<float>(phase) * loopLimit;
	}

	// get wave table information
	inline bool isValid() { return buffer != nullptr; }
	inline size_t size() { return buffer->size(); }
	inline float* data() { return buffer->data(); }

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
	size_t loopSize = 0;
	size_t loopCount = 0;
	float loopLimit = 0.0f;

	float lastFrequency = -1.0f;
	float t = 0.0f;
	float dt = 0.0f;

	// get sample at specified index
	inline float getSample(float index) {
		auto low = static_cast<size_t>(index);
		auto high = (low == buffer->size() - 1) ? 0 : low + 1;
		auto ratio = index - static_cast<float>(low);

		float result = 0.0f;

		for (size_t i = 0; i < 4; i++) {
			result += std::lerp(buffer->get(0, low + i * loopSize), buffer->get(0, high + i * loopSize), ratio);
		}

		return result;
		// return std::lerp(buffer->get(0, low), buffer->get(0, high), ratio);
	}

	// read a fixed length character stream and convert to std::string
	inline std::string readString(std::ifstream& ifs, size_t size) {
		std::string buffer(size, 0);
		readData(ifs, buffer.data(), size);
		return buffer;
	}

	// read 4 byte little endian number and convert to size_t
	inline size_t readInt4(std::ifstream& ifs) {
		std::byte buffer[4];
		readData(ifs, reinterpret_cast<char*>(buffer), 4);

		return
			(static_cast<size_t>(buffer[3]) << 24) |
			(static_cast<size_t>(buffer[2]) << 16) |
			(static_cast<size_t>(buffer[1]) << 8) |
			 static_cast<size_t>(buffer[0]);
	}

	// read 2 byte little endian number and convert to size_t
	inline size_t readInt2(std::ifstream& ifs) {
		std::byte buffer[2];
		readData(ifs, reinterpret_cast<char*>(buffer), 2);
		return (static_cast<size_t>(buffer[1]) << 8) | static_cast<size_t>(buffer[0]);
	}

	// read a number of bytes and store in provided buffer
	inline void readData(std::ifstream& ifs, char* buffer, size_t size) {
		ifs.read(buffer, size);

		if (ifs.eof()) {
			OtLogError("Corrupt WAV file");
		}
	}

	inline bool tryToReadData(std::ifstream& ifs, char* buffer, size_t size) {
		ifs.read(buffer, size);
		return !ifs.eof();
	}
};
