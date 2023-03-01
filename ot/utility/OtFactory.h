//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	  Include files
//

#include <functional>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>


//
//	OtFactory
//

template <typename B, typename... Ts>
class OtFactory {
public:
	// constructors
	OtFactory() {
		(add<Ts>(), ...);
	}

	// add a new type to the factory
	template <typename T>
	void add() {
		list.emplace_back<std::string_view>(T::name);

		map.emplace(T::name, []() {
			return OtObjectPointer<T>::create();
		});
	}

	// create a new instance by name
	B create(std::string_view name) {
		return map.find(name)->second();
	}

	// see if a specified name can be instantiated
	bool exists(std::string_view name) {
		return map.find(name) != map.cend();
	}

	// interate through the type names
	void each(std::function<void(const char* name)> callback) {
		for(const auto& entry : list) {
			callback(entry.data());
		}
	}

private:
	std::unordered_map<std::string_view, std::function<B()>> map;
	std::vector<std::string_view> list;
};
