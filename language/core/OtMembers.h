//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//


#include <functional>
#include <limits>

#include "OtIdentifier.h"
#include "OtObjectPointer.h"


//
//	Forward declarations
//

class OtObjectClass;
using OtObject = OtObjectPointer<OtObjectClass>;


//
//	OtMembers
//

class OtMembers {
public:
	// constructors/destructor (rule of 6)
	OtMembers() { reserve(4); }
	OtMembers(const OtMembers&) = delete;
	OtMembers(OtMembers&&) = delete;
	~OtMembers() { clear(); }

	OtMembers& operator=(const OtMembers&) = delete;
	OtMembers& operator=(OtMembers&&) = delete;

	// access the members
	inline bool has(OtID id) { return find(id) != nullptr; }
	inline OtObject get(OtID id) { return find(id)->object; }
	inline void set(OtID id, OtObject member) { assign(id, member); }
	inline void unset(OtID id) { erase(id); }
	inline void unsetAll() { clear(); }

	// iterate through the members
	inline void each(std::function<void(OtID, OtObject object)> callback) {
		for (auto i = begin(); i < end(); i++) {
			callback(i->id, i->object);
		}
	}

	inline void eachID(std::function<void(OtID)> callback) {
		for (auto i = begin(); i < end(); i++) {
			callback(i->id);
		}
	}

private:
	// a single entry in the hash table
	static constexpr size_t noNext = std::numeric_limits<size_t>::max();

	class Entry {
	public:
		// constructors
		Entry() = default;
		Entry(OtID i, OtObject o, size_t n=noNext) : id(i), object(o), next(n) {}

		// properties
		OtID id = 0;
		OtObject object;
		size_t next = noNext;
	};

	// hash table
	Entry* entries = nullptr;
	size_t* buckets = nullptr;
	size_t size = 0;
	size_t capacity = 0;

	// add an entry
	void add(OtID id, OtObject object) {
		// grow hash table (if required)
		if (size == capacity) {
			reserve(capacity << 1);
		}

		auto hash = id & (capacity - 1);
		entries[size] = Entry(id, object, buckets[hash]);
		buckets[hash] = size;
		size++;
	}

	// assign a new value
	void assign(OtID id, OtObject object) {
		auto entry = find(id);

		if (entry) {
			entry->object = object;

		} else {
			add(id, object);
		}
	}

	// erase an entry
	void erase(OtID id) {
		auto hash = id & (capacity - 1);
		auto i = find(id);

		if (i) {
			// unchain old entry
			auto* next = &buckets[hash];

			while (entries[*next].id != id) {
				next = &(entries[*next].next);
			}

			*next = entries[*next].next;

			// remove old entry
			auto last = end() - 1;

			while (i < last) {
				*i = *(i + 1);
				i++;
			}

			*i = Entry();
			size--;
			rehash();
		}
	}

	// find an entry
	Entry* find(OtID id) {
		auto hash = id & (capacity - 1);
		auto next = buckets[hash];

		if (next != noNext) {
			while (next != noNext) {
				auto& entry = entries[next];

				if (entry.id == id) {
					return &entry;

				} else {
					next = entry.next;
				}
			}
		}

		return nullptr;
	}

	// reserve specified capacity for hash table
	void reserve(size_t newCapacity) {
		auto newEntries = new Entry[newCapacity];

		if (capacity) {
			for (size_t i = 0; i < capacity; i++) {
				newEntries[i] = entries[i];
			}

			delete[] entries;
			delete[] buckets;
		}

		entries = newEntries;
		capacity = newCapacity;
		buckets = new size_t[capacity << 1];

		rehash();
	}

	// clear members
	void clear() {
		delete[] entries;
		delete[] buckets;
		entries = nullptr;
		buckets = nullptr;
		capacity = 0;
		size = 0;
	}

	void rehash() {
		for (size_t i = 0; i < capacity << 1; i++) {
			buckets[i] = noNext;
		}

		for (size_t i = 0; i < size; i++) {
			auto hash = entries[i].id & (capacity - 1);
			entries[i].next = buckets[hash];
			buckets[hash] = i;
		}
	}

	// iterator support
	Entry const* begin() const noexcept { return entries; }
	Entry const* end() const noexcept { return entries + size; }
};
