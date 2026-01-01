//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Non-cryptographic random numbers
//

double OtRandom(double num1, double num2);
inline double OtRandom(double num) { return OtRandom(0.0, num); }
inline double OtRandom() { return OtRandom(0.0, 1.0); }

inline float OtRandom(float num1, float num2) { return float(OtRandom(double(num1), double(num2))); }
inline float OtRandom(float num) { return float(OtRandom(0.0, double(num))); }

inline int OtRandom(int num1, int num2) { return int(OtRandom(double(num1), double(num2))); }
inline int OtRandom(int num) { return int(OtRandom(0.0, double(num))); }
