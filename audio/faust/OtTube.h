//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.

//	Based on https://github.com/brummer10/guitarix


#pragma once


//
//	Include files
//

#include <cstdint>


//
//	Table types
//

struct OtTubeTable {
	double low;
	double high;
	double istep;
	int32_t size;
	double* data;
};


//
//	Tube definitions
//

enum {
	TUBE_TABLE_12AX7_68k,
	TUBE_TABLE_12AX7_250k,
	TUBE_TABLE_KT88_68k,
	TUBE_TABLE_KT88_250k,
	TUBE_TABLE_7199P_68k,
	TUBE_TABLE_7199P_250k
};


//
//	Tube table access functions
//


OtTubeTable& OtTubeGetTable(int table, int index);

static inline double OtTubeGetF(int table, double input) {
	auto& tab = OtTubeGetTable(table, 0);
	auto f = (input - tab.low) * tab.istep;
	int i = static_cast<int>(f);

	if (i < 0) {
		return static_cast<double>(tab.data[0]);
	}

	if (i >= tab.size - 1) {
		return static_cast<double>(tab.data[tab.size - 1]);
	}

	f -= i;
	return static_cast<double>(tab.data[i] * (1 - f) + tab.data[i + 1] * f);
}

static inline double OtTubeGetR(int table, double input) {
	auto& tab = OtTubeGetTable(table, 1);
	auto f = (input - tab.low) * tab.istep;
	int i = static_cast<int>(f);

	if (i < 0) {
		return static_cast<double>(tab.data[0]);
	}

	if (i >= tab.size - 1) {
		return static_cast<double>(tab.data[tab.size-1]);
	}

	f -= i;
	return static_cast<double>(tab.data[i] * (1 - f) + tab.data[i + 1] * f);
}
