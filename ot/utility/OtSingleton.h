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
	static std::shared_ptr<T> instance() {
		static std::shared_ptr<T> instance;

		if (!instance) {
			instance = std::make_shared<T>();
		}

		return instance;
	}

	OtSingleton(const OtSingleton&) = delete;
	OtSingleton& operator= (const OtSingleton) = delete;

protected:
	OtSingleton() = default;
};


//
//	OtPerThreadSingleton
//

template<typename T>
class OtPerThreadSingleton {
public:
	static std::shared_ptr<T> instance() {
		thread_local std::shared_ptr<T> instance;


		if (!instance) {
			instance = std::make_shared<T>();
		}

		return instance;
	}

	OtPerThreadSingleton(const OtPerThreadSingleton&) = delete;
	OtPerThreadSingleton& operator= (const OtPerThreadSingleton) = delete;

protected:
	OtPerThreadSingleton() = default;
};


//
//	OtObjectSingleton
//

template<typename T>
class OtObjectSingleton {
public:
	static std::shared_ptr<T> instance() {
		static std::shared_ptr<T> instance;

		if (!instance) {
			instance = std::make_shared<T>();
			instance->setType(T::getMeta());
		}

		return instance;
	}

	OtObjectSingleton(const OtObjectSingleton&) = delete;
	OtObjectSingleton& operator= (const OtObjectSingleton) = delete;

protected:
	OtObjectSingleton() = default;
};
