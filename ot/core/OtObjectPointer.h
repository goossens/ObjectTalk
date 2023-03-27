//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>


//
//  Forward declarations
//

class OtObjectClass;


//
//  Shared pointer (not thread safe, not all bells and whistles but fast)
//

template <typename T>
class OtObjectPointer {
public:
	// default constructor
	OtObjectPointer() : ptr(nullptr) {}

	// null constructor
	OtObjectPointer(std::nullptr_t n) : ptr(nullptr) {}

	// constructor for when we already have a pointer
	OtObjectPointer(T* instance) : ptr(instance) {
		static_assert(std::is_base_of<OtObjectClass, T>::value, "Instance is not derived from OtObjectClass");
		incrementReference();
	}

	// copy constructor
	OtObjectPointer(const OtObjectPointer& ref) {
		ptr = ref.ptr;
		incrementReference();
	}

	template<typename Ts>
	OtObjectPointer(const OtObjectPointer<Ts>& ref) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		assert(ptr || !ref.ptr);
		incrementReference();
	}

	// move constructors
	OtObjectPointer(OtObjectPointer&& ref) {
		ptr = ref.ptr;
		ref.ptr = nullptr;
	}

	template<typename Ts>
	OtObjectPointer(OtObjectPointer<Ts>&& ref) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		assert(ptr || !ref.ptr);
		ref.ptr = nullptr;
	}

	// destructor
	~OtObjectPointer() {
		decrementReference();
	}

	// copy assignment
	OtObjectPointer& operator=(const OtObjectPointer& ref) {
		decrementReference();
		ptr = ref.ptr;
		incrementReference();
		return *this;
	}

	OtObjectPointer& operator=(std::nullptr_t) {
		decrementReference();
		ptr = nullptr;
		return *this;
	}

	template<typename Ts>
	OtObjectPointer& operator=(const OtObjectPointer<Ts>& ref) {
		decrementReference();
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		assert(ptr || !ref.ptr);
		incrementReference();
		return *this;
	}

	// move assignment
	OtObjectPointer& operator=(OtObjectPointer&& ref) {
		decrementReference();
		ptr = ref.ptr;
		ref.ptr = nullptr;
		return *this;
	}

	template<typename Ts>
	OtObjectPointer& operator=(OtObjectPointer<Ts>&& ref) {
		decrementReference();
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		assert(ptr || !ref.ptr);
		ref.ptr = nullptr;
		return *this;
	}

	// see if pointer is "kind of"
	template<typename Ts>
	bool isKindOf() {
		return dynamic_cast<Ts*>(ptr);
	}

	// dereferencing operators
	T* operator->() { return ptr; }
	const T* operator->() const { return ptr; }
	T& operator*() { return *ptr; }
	const T& operator*() const { return *ptr; }

	// access raw pointer
	T* raw() { return ptr; }

	// check validity of reference
	operator bool() { return ptr != nullptr; }
	operator bool() const { return ptr != nullptr; }

	// equality tests
	bool operator==(const OtObjectPointer<T>& ref) const { return ptr == ref.ptr; }
	bool operator!=(const OtObjectPointer<T>& ref) const { return ptr != ref.ptr; }
	bool equals(const OtObjectPointer<T>& ref) const { return (ptr && ref.ptr) ? *ptr == *ref.ptr : false; }

	// less than operator
	bool operator<(const OtObjectPointer<T>& ref) const { return ptr < ref.ptr; }

	// get reference count
	uint64_t getReferenceCount() { return ptr ? ptr->referenceCount : 0; }

	// create a new object instance
	template<typename... Args>
	static OtObjectPointer<T> create(Args&&... args) {
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
	T* ptr = nullptr;
	template <typename Ts> friend class OtObjectPointer;
};
