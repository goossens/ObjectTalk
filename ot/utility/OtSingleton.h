//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <memory>


//
//	OtSingleton
//

template<typename T>
class OtSingleton {
public:
	static T* instance() {
		static std::unique_ptr<T> instance;

		if (!instance) {
			instance = std::make_unique<T>();
		}

		return instance.get();
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
	static T* instance() {
		thread_local std::unique_ptr<T> instance;

		if (!instance) {
			instance = std::make_unique<T>();
		}

		return instance.get();
	}

	OtPerThreadSingleton(const OtPerThreadSingleton&) = delete;
	OtPerThreadSingleton(OtPerThreadSingleton&&) = delete;
	OtPerThreadSingleton& operator=(const OtPerThreadSingleton&) = delete;
	OtPerThreadSingleton& operator=(OtPerThreadSingleton&&) = delete;

protected:
	OtPerThreadSingleton() = default;
};
