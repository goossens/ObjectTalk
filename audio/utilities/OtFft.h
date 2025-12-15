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


//
//	OtFft
//

void OtFft(std::complex<float>* fourier, int n) {
	// calculate 2log(m)
    auto m = -1;
    for (auto i = n; i; i >>= 1) { m++; }

	// reverse bits
	auto i2 = n / 2;
    auto j = 0;

	for (auto i = 0; i < n - 1; i++) {
		if (i < j) {
			std::swap(fourier[i], fourier[j]);
		}

		auto k = i2;

		while (k <= j) {
			j -= k;
			k /= 2;
		}

		j += k;
	}

	// compute the FFT
	auto c = std::complex<float>(-1.0f, 0.0f);
    auto l2 = 1;

	for (auto l = 0; l < m; l++) {
		auto l1 = l2;
		l2 <<= 1;
		auto u1 = 1.0f;
		auto u2 = 0.0f;

		for (j = 0; j < l1; j++) {
			for (auto i = j; i < n; i += l2) {
				auto i1 = i + l1;

				auto t1 = u1 * fourier[i1].real() - u2 * fourier[i1].imag();
				auto t2 = u1 * fourier[i1].imag() + u2 * fourier[i1].real();

				fourier[i1].real(fourier[i].real() - t1);
				fourier[i1].imag(fourier[i].imag() - t2);
				fourier[i].real(fourier[i].real() + t1);
				fourier[i].imag(fourier[i].imag() + t2);
			}

			auto z = u1 * c.real() - u2 * c.imag();
			u2 = u1 * c.imag() + u2 * c.real();
			u1 = z;
		}

		c.imag(std::sqrt(0.5f - 0.5f * c.real()));
		c.real(std::sqrt(0.5f + 0.5f * c.real()));
	}

	// for (auto i = 0; i < n; i++) {
	// 	fourier[i] /= n;
	// 	fourier[i] /= n;
	// }
}
