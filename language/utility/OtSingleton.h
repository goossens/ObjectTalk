//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	OtSingleton
//

template<typename T>
class OtSingleton {
public:
	static inline T& instance() {
		static T instance;
		return instance;
	}

	OtSingleton(const OtSingleton&) = delete;
	OtSingleton(OtSingleton&&) = delete;
	OtSingleton& operator=(const OtSingleton&) = delete;
	OtSingleton& operator=(OtSingleton&&) = delete;

protected:
	OtSingleton() = default;
};


//
//	OtPerThreadSingleton
//

template<typename T>
class OtPerThreadSingleton {
public:
	static inline T& instance() {
		thread_local T instance;
		return instance;
	}

	OtPerThreadSingleton(const OtPerThreadSingleton&) = delete;
	OtPerThreadSingleton(OtPerThreadSingleton&&) = delete;
	OtPerThreadSingleton& operator=(const OtPerThreadSingleton&) = delete;
	OtPerThreadSingleton& operator=(OtPerThreadSingleton&&) = delete;

protected:
	OtPerThreadSingleton() = default;
};
