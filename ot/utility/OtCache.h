//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include<list>
#include<unordered_map>

#include "OtException.h"


//
//	OtCache class
//

template<typename K, typename V>
class OtCache
{
public:
	// constructor
	OtCache() : size(1024) {}
	OtCache(size_t s) :size(s) {}

	void setSize(size_t s) {
		size = s;

		while (index.size() > size) {
			index.erase(items.back().first);
			items.pop_back();
		}
	}

	void clear() {
		items.clear();
		index.clear();
	}

	void set(const K key, const V value) {
		// see if this is a known entry
		auto pos = index.find(key);

		if (pos == index.end()) {
			// add new cache entry
			items.push_front({key, value});
			index[key] = items.begin();

			// trim cache if required
			if (index.size() > size) {
				index.erase(items.back().first);
				items.pop_back();
			}

		} else {
			// replace existing cache entry
			items.erase(pos->second);
			items.push_front({key, value});
			index[key] = items.begin();
		}
	}

	bool has(const K key) {
		return index.find(key) != index.end();
	}

	V get(const K key) {
		auto pos = index.find(key);

		if (pos == index.end()) {
			OtExcept("Invalid key for cache");
		}

		V result = pos->second->second;
		items.erase(pos->second);
		items.push_front({key, result});
		index[key] = items.begin();
		return result;
	}

private:
	std::list<std::pair<K, V>> items;
	std::unordered_map <K, typename std::list<std::pair<K, V>>::iterator> index;
	size_t size;
};
