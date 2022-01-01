//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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
	static T& instance() {
		static std::unique_ptr<T> instance = std::make_unique<T>();
		return *instance;
	}

	OtSingleton(const OtSingleton&) = delete;
	OtSingleton& operator= (const OtSingleton) = delete;

protected:
	OtSingleton() {}
};
