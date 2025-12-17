//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://gist.github.com/agrafix/aa49c17cd32c8ba63b6a7cb8dce8b0bd


#pragma once


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <complex>

#include "pffft.h"


//
//	OtFft
//

class OtFft {
public:
	OtFft(int s) {
		size = s;
		scale = 1.0f / size;
		setup = pffft_new_setup(size, PFFFT_REAL);
		workBuffer = reinterpret_cast<float*>(pffft_aligned_malloc(size * sizeof(float)));
		resultBuffer = reinterpret_cast<std::complex<float>*>(pffft_aligned_malloc(size / 2 * sizeof(std::complex<float>)));
	}

	OtFft() {
		pffft_destroy_setup(setup);
		pffft_aligned_free(resultBuffer);
		pffft_aligned_free(workBuffer);
	}

	std::complex<float>* forward(float* inputBuffer) {
		pffft_transform_ordered(
			setup,
			inputBuffer,
			reinterpret_cast<float*>(resultBuffer),
			workBuffer,
			PFFFT_FORWARD);

		return resultBuffer;
	}

	std::complex<float>* inverse(std::complex<float>* inputBuffer) {
		pffft_transform_ordered(
			setup,
			reinterpret_cast<float*>(inputBuffer),
			reinterpret_cast<float*>(resultBuffer),
			workBuffer,
			PFFFT_BACKWARD);

		for (int i = 0; i < size; i++) {
			resultBuffer[i] *= scale;
		}

		return resultBuffer;
	}

	float* forwardFrequencyOnly(float* inputBuffer) {
		pffft_transform_ordered(
			setup,
			inputBuffer,
			reinterpret_cast<float*>(resultBuffer),
			workBuffer,
			PFFFT_FORWARD);

		auto* out = reinterpret_cast<float*>(resultBuffer);
		auto limit = size / 2;

		for (int i = 0; i < limit; i++) {
			out[i] = std::abs(resultBuffer[i]) * scale * 2.0;
		}

		std::fill(out + limit, out + size, 0.0f);
		return out;
	}

	int getSize() { return size; }

private:
	int size;
	float scale;

	PFFFT_Setup* setup;
	float* workBuffer;
	std::complex<float>* resultBuffer;
};
