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

#include <cmath>
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
//	Preamp tube definitions
//

enum {
	TUBE_TABLE_12AT7_68k,
	TUBE_TABLE_12AT7_250k,
	TUBE_TABLE_12AU7_68k,
	TUBE_TABLE_12AU7_250k,
	TUBE_TABLE_12AX7_68k,
	TUBE_TABLE_12AX7_250k,
	TUBE_TABLE_12AY7_68k,
	TUBE_TABLE_12AY7_250k,
	TUBE_TABLE_KT88_68k,
	TUBE_TABLE_KT88_250k,
	TUBE_TABLE_7199P_68k,
	TUBE_TABLE_7199P_250k
};


//
//	PowerAmp tube definitions
//

enum {
	TUBE_TABLE_SINGLE_ENDED_EL84_NEG,
	TUBE_TABLE_SINGLE_ENDED_EL84_POS,
	TUBE_TABLE_SINGLE_ENDED_6V6,
	TUBE_TABLE_PUSH_PULL_EL34_NEG,
	TUBE_TABLE_PUSH_PULL_EL34_POS,
	TUBE_TABLE_PUSH_PULL_EL84_NEG,
	TUBE_TABLE_PUSH_PULL_EL84_POS,
	TUBE_TABLE_PUSH_PULL_6L6_NEG,
	TUBE_TABLE_PUSH_PULL_6L6_POS
};


//
//	Tube table access functions
//

OtTubeTable& OtTubeGetPreampTable(int table);

static inline double OtTubePreamp(int table, double input) {
	auto& tab = OtTubeGetPreampTable(table);
	auto f = (input - tab.low) * tab.istep;
	int i = static_cast<int>(f);

	if (i < 0) {
		return tab.data[0];
	}

	if (i >= tab.size - 1) {
		return tab.data[tab.size - 1];
	}

	f -= i;
	return tab.data[i] * (1 - f) + tab.data[i + 1] * f;
}

OtTubeTable& OtTubeGetPowerAmpTable(int table);

static inline double OtTubePowerAmp(int negTable, int posTable, double input) {
	auto& tab = OtTubeGetPowerAmpTable(input < 0.0 ? negTable : posTable);
	auto f = std::fabs(input) * tab.istep;
	int i = static_cast<int>(f);

	if (i < 0) {
		f = tab.data[0];

	} else if (i >= tab.size - 1) {
		f = tab.data[tab.size - 1];

	} else {
		f -= i;
		f = tab.data[i] * (1 - f) + tab.data[i + 1] * f;

	}

	return std::copysign(f, input);
}
