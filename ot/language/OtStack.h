//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "OtByteCode.h"
#include "OtObject.h"


//
//	OtStackItem
//

class OtStackItem {
public:
	// constructors
	OtStackItem() = default;
	OtStackItem(size_t f, size_t s) : frame(f), slot(s) {}

	// comparison operators
	bool operator==(const OtStackItem& item) const {
		return frame == item.frame && slot == item.slot;
	}

	bool operator!=(const OtStackItem& item) const {
		return frame != item.frame || slot != item.slot;
	}

	// stack item address
	size_t frame = 0;
	size_t slot = 0;
};


//
//	OtStackFrame
//

class OtStackFrame {
public:
	// constructors
	OtStackFrame() = default;
	OtStackFrame(OtByteCode b, size_t o, size_t* p) : bytecode(b), offset(o), pc(p) {}

	// frame data
	OtByteCode bytecode;
	size_t offset;
	size_t* pc;
};


//
//	OtStackState
//

class OtStackState {
public:
	// constructors
	OtStackState() = default;
	OtStackState(size_t s, size_t f, size_t c) : stack(s), frames(f), closures(c) {}

	// stack state
	size_t stack;
	size_t frames;
	size_t closures;
};


//
//	OtStack
//

class OtStack {
public:
	// constructor/destructor
	inline OtStack() { reserve(512); }
	inline ~OtStack() { clear(); }

	// stack access functions
	inline void push(const OtObject& object) { if (sp == capacity) { reserve(capacity * 2); } stack[sp++] = object; }
	inline OtObject pop() { return stack[--sp]; }
	inline void pop(size_t count) { sp -= count; }
	inline void dup() { if (sp == capacity) { reserve(capacity * 2); } stack[sp] = stack[sp - 1]; sp++; }
	inline void swap() { std::swap(stack[sp - 1], stack[sp - 2]); }
	inline void move(size_t count) { stack[sp - count - 1] = stack[sp - 1]; sp -= count; }
	inline size_t size() { return sp; }
	inline OtObject* getSP(size_t offset) { return stack + sp - offset; }
	inline OtObject top() { return stack[sp - 1]; }

	// frame access functions
	inline void openFrame(OtByteCode bytecode, size_t callingParameters, size_t* pc) { frames.emplace_back(bytecode, sp - callingParameters, pc); }
	inline OtObject getFrameItem(OtStackItem item) { return stack[frames[frames.size() - item.frame - 1].offset + item.slot]; }
	inline void setFrameItem(OtStackItem item, OtObject object) { stack[frames[frames.size() - item.frame - 1].offset + item.slot] = object; }
	inline OtStackFrame& getFrame() { return frames.back(); }
	inline void closeFrame() { frames.pop_back(); }
	inline size_t getFrameCount() { return frames.size(); }

	// closure access functions
	inline void pushClosure(OtObject closure) { closures.emplace_back(closure); }
	inline OtObject getClosure() { return closures.back(); }
	inline void popClosure() { closures.pop_back(); }

	// manipulate stack state
	inline OtStackState getState() {
		auto& frame = frames.back();
		return OtStackState(sp, frames.size(), closures.size());
	}

	inline void restoreState(OtStackState& state) {
		sp = state.stack;
		frames.resize(state.frames);
		closures.resize(state.closures);
	}

private:
	// reserve specified capapavity on stack
	inline void reserve(size_t newCapacity) {
		stack = reinterpret_cast<OtObject*>(std::realloc(stack, sizeof(OtObject) * newCapacity));

		for (auto i = stack + capacity; i < stack + newCapacity; i++) {
			new(i) OtObject;
		}

		capacity = newCapacity;
	}

	// clear the stack and release the memory
	inline void clear() {
		for (auto i = stack; i < stack + capacity; i++) {
			i->~OtObject();
		}

		std::free(stack);
		stack = nullptr;
		sp = 0;
		capacity = 0;

		frames.clear();
		closures.clear();
	}

	// properties
	OtObject* stack = nullptr;
	size_t sp = 0;
	size_t capacity = 0;

	std::vector<OtStackFrame> frames;
	std::vector<OtObject> closures;
};
