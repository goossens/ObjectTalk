//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <fstream>
#include <memory>
#include <string>

#include "OtLog.h"

#include "OtSampleBuffer.h"

//
//	OtWaveTable
//

class OtWaveTable {
public:
	// load a wave table from a file
	void load(const std::string& path);

	// clear the wave table
	void clear();

	// get sample at specified location
	float get(float sample, float shape);

	// get wave table information
	inline bool isValid() { return buffer != nullptr; }
	inline size_t getSampleCount() { return samplesPerShape; }
	inline size_t getShapeCount() { return shapes; }
	inline float* data() { return buffer->data(); }
	inline size_t size() { return buffer->size(); }

	// see if wave tables are identical
	inline bool operator==(OtWaveTable& rhs) { return buffer == rhs.buffer; }
	inline bool operator!=(OtWaveTable& rhs) { return !operator==(rhs); }

private:
	// properties
	std::shared_ptr<OtSampleBuffer> buffer;

	// work variables
	size_t samplesPerShape = 0;
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
