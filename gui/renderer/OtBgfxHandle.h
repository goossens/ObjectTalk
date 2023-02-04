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

#include "bgfx/bgfx.h"


//
//	OtBgfxHandle
//

template <typename T>
class OtBgfxHandle {
public:
	// constructors
	OtBgfxHandle() = default;
	OtBgfxHandle(T handle) { handleRef = std::make_shared<Handle<T>>(handle); }

	// see if handle is valid
	bool isValid() { return handleRef != nullptr; }

	// clear the handle reference
	void clear() { handleRef = nullptr; }

	// get the handle
	T getHandle() { return handleRef == nullptr ? T(BGFX_INVALID_HANDLE) : handleRef->get(); }

	// get the handle index
	uint16_t getIndex() {
		return handleRef ? handleRef->getIndex() : bgfx::kInvalidHandle;
	}

private:
	template <typename Ts>
	class Handle {
	public:
		// constructors/destructor (rule of 7)
		Handle() = default;
		Handle(Ts h) { handle = h; }
		Handle(const Handle&) = delete; // no copy constructor
		Handle& operator=(const Handle&) = delete; // no copy assignment
		Handle(Handle&&) = default;
		Handle& operator=(Handle&&) = default;
		~Handle() { bgfx::destroy(handle); }

		// clear the handle
		void clear() {
			if (bgfx::isValid(handle)) {
				bgfx::destroy(handle);
			}

			handle = BGFX_INVALID_HANDLE;
		}

		// get/set
		Ts get() { return handle; }

		Handle& operator=(Ts h) {
			clear();
			handle = h;
			return this;
		}

		// get the handle index
		uint16_t getIndex() { return handle.idx; }

	private:
		// the actual handle
		Ts handle = BGFX_INVALID_HANDLE;
	};

	// tracking the handle
	std::shared_ptr<Handle<T>> handleRef;
};
