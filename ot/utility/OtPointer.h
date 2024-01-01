//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
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
//	OtSharable
//

class OtSharable {
private:
	// reference count
	size_t referenceCount{0};
	template <typename T> friend class OtObjectPointer;
};


//
//  Shared pointer (not thread safe, not all bells and whistles but fast and small)
//

template <typename T>
class OtPointer {
public:
	// default constructor
	OtPointer() : ptr(nullptr) {}

	// null constructor
	OtPointer(std::nullptr_t n) : ptr(nullptr) {}

	// constructor for when we already have a pointer
	OtPointer(T* instance) : ptr(instance) {
		static_assert(std::is_base_of<OtSharable, T>::value, "Instance is not derived from OtSharable");
		incrementReference();
	}

	// copy constructor
	OtPointer(const OtPointer& ref) {
		ptr = ref.ptr;
		incrementReference();
	}

	template<typename Ts>
	OtPointer(const OtPointer<Ts>& ref) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		assert(ptr || !ref.ptr);
		incrementReference();
	}

	// move constructors
	OtPointer(OtPointer&& ref) {
		ptr = ref.ptr;
		ref.ptr = nullptr;
	}

	template<typename Ts>
	OtPointer(OtPointer<Ts>&& ref) {
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		assert(ptr || !ref.ptr);
		ref.ptr = nullptr;
	}

	// destructor
	~OtPointer() {
		decrementReference();
	}

	// copy assignment
	OtPointer& operator=(const OtPointer& ref) {
		decrementReference();
		ptr = ref.ptr;
		incrementReference();
		return *this;
	}

	OtPointer& operator=(std::nullptr_t) {
		decrementReference();
		ptr = nullptr;
		return *this;
	}

	template<typename Ts>
	OtPointer& operator=(const OtPointer<Ts>& ref) {
		decrementReference();
		static_assert(std::is_base_of<Ts, T>::value || std::is_base_of<T, Ts>::value, "Can't convert smart pointers");
		ptr = dynamic_cast<T*>(ref.ptr);
		assert(ptr || !ref.ptr);
		incrementReference();
		return *this;
	}

	// move assignment
	OtPointer& operator=(OtPointer&& ref) {
		decrementReference();
		ptr = ref.ptr;
		ref.ptr = nullptr;
		return *this;
	}

	template<typename Ts>
	OtPointer& operator=(OtPointer<Ts>&& ref) {
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
	bool operator==(const OtPointer<T>& ref) const { return ptr == ref.ptr; }
	bool operator!=(const OtPointer<T>& ref) const { return ptr != ref.ptr; }
	bool equals(const OtPointer<T>& ref) const { return (ptr && ref.ptr) ? *ptr == *ref.ptr : false; }

	// less than operator
	bool operator<(const OtPointer<T>& ref) const { return ptr < ref.ptr; }

	// get reference count
	uint64_t getReferenceCount() { return ptr ? ptr->referenceCount : 0; }

	// create a new object instance
	template<typename... Args>
	static OtPointer<T> create(Args&&... args) {
		return OtPointer<T>(new T(std::forward<Args>(args)...));
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
	template <typename Ts> friend class OtPointer;
};
