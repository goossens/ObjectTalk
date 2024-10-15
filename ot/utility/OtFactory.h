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
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "OtException.h"


//
//	OtFactory
//

template <typename B, typename... Ts>
class OtFactory {
public:
	// constructors
	inline OtFactory() {
		(add<Ts>(), ...);
	}

	// add a new type to the factory
	template <typename T>
	inline void add() {
		list.emplace_back<std::string_view>(T::name);

		map.emplace(T::name, []() {
			return std::make_shared<T>();
		});
	}

	// create a new instance by name
	inline std::shared_ptr<B> create(std::string_view name) {
		if (!exists(name)) {
			OtError("Internal error: factory can't create instance of type [{}]", name);
		}

		return map.find(name)->second();
	}

	// see if a specified name can be instantiated
	inline bool exists(std::string_view name) {
		return map.find(name) != map.cend();
	}

	// interate through the type names
	inline void each(std::function<void(const char* name)> callback) {
		for(const auto& entry : list) {
			callback(entry.data());
		}
	}

private:
	std::unordered_map<std::string_view, std::function<std::shared_ptr<B>()>> map;
	std::vector<std::string_view> list;
};
