//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>


//
//	OtHash
//

template <typename T, typename... REST>
void OtHashRecursive(std::size_t& hash, const T& v, const REST&... rest) {
	hash ^= std::hash<T>{}(v) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
	(OtHashRecursive(hash, rest), ...);
}

template <typename... PARS>
std::size_t OtHash(const PARS&... pars) {
	std::size_t hash = 0;
	OtHashRecursive(hash, pars...);
	return hash;
}
