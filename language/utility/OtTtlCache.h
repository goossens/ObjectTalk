//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <unordered_map>

#include "OtLog.h"


//
//	OtTtlCache class (Time-To-Live)
//

template<typename K, typename V>
class OtTtlCache {
public:
	// return number of cache entries
	inline size_t getNumberOfEntries() {
		return entries.size();
	}

	// remove all entries
	inline void clear() {
		entries.clear();
	}

	// see if entry is in cache
	inline bool has(const K& key) {
		return entries.find(key) != entries.end();
	}

	// create a new cache entry in place and return a reference
	inline V& emplace(const K& key) {
		return entries.emplace(key, V()).first->second.value;
	}

	// set a cache entry
	inline V& set(const K& key, const V& value) {
		entries[key] = Entry(value);
		auto pos = entries.find(key);
		pos->second.ttl = 0.0f;
		return pos->second.value;
	}

	// get a cache entry
	inline V& get(const K& key) {
		auto pos = entries.find(key);

		if (pos == entries.end()) {
			OtLogError("Invalid key for cache");
		}

		pos->second.ttl = 0.0f;
		return pos->second.value;
	}

	// update the cache
	inline void update(float interval, float threshold) {
		//process all entries
		for (auto i = entries.begin(); i != entries.end();) {
			// update time-to-live
			i->second.ttl += interval;

			// remove entry at end-of-life
			if (i->second.ttl > threshold) {
				i = entries.erase(i);

			} else {
				i++;
			}
		}
	}

private:
	// cache entry
	struct Entry {
		Entry() = default;
		Entry(const V& v) : value(v) {}
		V value;
		float ttl = 0.0f;
	};

	std::unordered_map<K, Entry> entries;
};
