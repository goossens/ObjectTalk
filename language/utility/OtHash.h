//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <functional>


//
//	OtHash
//

struct OtHash {
	template <typename T, typename... REST>
	static inline void recursive(std::size_t& hash, const T& v, const REST&... rest) {
		hash ^= std::hash<T>{}(v) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		(recursive(hash, rest), ...);
	}

	template <typename... PARS>
	static inline std::size_t generate(const PARS&... pars) {
		std::size_t hash = 0;
		recursive(hash, pars...);
		return hash;
	}

	// hash a uint32_t integer into a float in the range [0, 1]
	static float toFloat(uint32_t x);
	static float toFloat(uint32_t x, uint32_t y);
	static float toFloat(uint32_t x, uint32_t y, uint32_t z);
	static float toFloat(uint32_t x, uint32_t y, uint32_t z, uint32_t w);
};
