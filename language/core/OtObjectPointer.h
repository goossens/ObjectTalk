//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "OtAssert.h"


//
//  Forward declarations
//

class OtObjectClass;


//
//  Shared pointer (not thread safe, not all the bells and whistles but fast)
//

template <typename T>
class OtObjectPointer {
public:
	// sanity check
	static_assert(std::is_base_of<OtObjectClass, T>::value, "Instance is not derived from OtObjectClass");

	// default constructor
	inline OtObjectPointer() : ptr(nullptr) {}

	// null constructor
	inline OtObjectPointer(std::nullptr_t n) : ptr(nullptr) {}

	// constructor for when we already have a pointer
	inline OtObjectPointer(T* instance) : ptr(instance) {
		incrementReference();
	}

	// copy constructor
	inline OtObjectPointer(const OtObjectPointer& ref) {
		ptr = ref.ptr;
		incrementReference();
	}

	template<typename Ts>
	inline OtObjectPointer(const OtObjectPointer<Ts>& ref) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		OtAssert(ptr || !ref.ptr);
		incrementReference();
	}

	// move constructors
	inline OtObjectPointer(OtObjectPointer&& ref) {
		ptr = ref.ptr;
		ref.ptr = nullptr;
	}

	template<typename Ts>
	inline OtObjectPointer(OtObjectPointer<Ts>&& ref) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		OtAssert(ptr || !ref.ptr);
		ref.ptr = nullptr;
	}

	// destructor
	inline ~OtObjectPointer() {
		decrementReference();
	}

	// copy assignment
	inline OtObjectPointer& operator=(const OtObjectPointer& ref) {
		decrementReference();
		ptr = ref.ptr;
		incrementReference();
		return *this;
	}

	inline OtObjectPointer& operator=(std::nullptr_t) {
		decrementReference();
		ptr = nullptr;
		return *this;
	}

	template<typename Ts>
	inline OtObjectPointer& operator=(const OtObjectPointer<Ts>& ref) {
		decrementReference();
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		OtAssert(ptr || !ref.ptr);
		incrementReference();
		return *this;
	}

	// move assignment
	inline OtObjectPointer& operator=(OtObjectPointer&& ref) {
		decrementReference();
		ptr = ref.ptr;
		ref.ptr = nullptr;
		return *this;
	}

	template<typename Ts>
	inline OtObjectPointer& operator=(OtObjectPointer<Ts>&& ref) {
		decrementReference();
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		OtAssert(ptr || !ref.ptr);
		ref.ptr = nullptr;
		return *this;
	}

	// see if pointer is "kind of"
	template<typename Ts>
	inline bool isKindOf() {
		return dynamic_cast<Ts*>(ptr);
	}

	// dereferencing operators
	inline T* operator->() { return ptr; }
	inline const T* operator->() const { return ptr; }
	inline T& operator*() { return *ptr; }
	inline const T& operator*() const { return *ptr; }

	// access raw pointer
	inline T* raw() { return ptr; }

	// check validity of reference
	inline operator bool() { return ptr != nullptr; }
	inline operator bool() const { return ptr != nullptr; }

	// equality tests
	inline bool operator==(const OtObjectPointer<T>& ref) const { return ptr == ref.ptr; }
	inline bool operator!=(const OtObjectPointer<T>& ref) const { return ptr != ref.ptr; }
	inline bool equals(const OtObjectPointer<T>& ref) const { return (ptr && ref.ptr) ? *ptr == *ref.ptr : false; }

	// less than operator
	inline bool operator<(const OtObjectPointer<T>& ref) const { return ptr < ref.ptr; }

	// get reference count
	inline uint64_t getReferenceCount() { return ptr ? ptr->referenceCount : 0; }

	// create a new object instance
	template<typename... Args>
	static inline OtObjectPointer<T> create(Args&&... args) {
		auto object = OtObjectPointer<T>(new T(std::forward<Args>(args)...));
		object->setType(T::getMeta());
		return object;
	}

private:
	// increment instance reference count
	inline void incrementReference() {
		if (ptr) {
			ptr->referenceCount++;
		}
	}

	// decrement instance reference count and delete instance if required
	inline void decrementReference() {
		if (ptr) {
			if (--ptr->referenceCount == 0) {
				delete ptr;
				ptr = nullptr;
			}
		}
	}

	// pointer to the object
	T* ptr;
	template <typename Ts> friend class OtObjectPointer;
};
