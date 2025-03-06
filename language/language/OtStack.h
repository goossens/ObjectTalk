//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <vector>

#include "OtByteCode.h"
#include "OtObject.h"


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
	inline ~OtStack() { release(); }

	// stack access functions
	inline void push(const OtObject& object) { if (sp == capacity) { reserve(capacity * 2); } stack[sp++] = object; }
	inline void push(size_t count, OtObject* objects) { if (sp + count >= capacity) { reserve(capacity * 2); } while (count--) { stack[sp++] = *objects++; } }
	inline OtObject pop() { auto result = stack[--sp]; stack[sp] = nullptr; return result; }
	inline void pop(size_t count) { while (count--) { stack[--sp] = nullptr; } }
	inline void dup() { if (sp == capacity) { reserve(capacity * 2); } stack[sp] = stack[sp - 1]; sp++; }
	inline void swap() { std::swap(stack[sp - 1], stack[sp - 2]); }
	inline void move(size_t count) { stack[sp - count - 1] = stack[sp - 1]; sp -= count; }
	inline size_t size() { return sp; }
	inline OtObject* getSP(size_t offset) { return stack + sp - offset; }
	inline OtObject* raw() { return stack; }
	inline OtObject top() { return stack[sp - 1]; }

	// frame access functions
	inline void openFrame(OtByteCode bytecode, size_t callingParameters, size_t* pc) { frames.emplace_back(bytecode, sp - callingParameters, pc); }
	inline OtObject getFrameItem(size_t slot) { return stack[frames.back().offset + slot]; }
	inline OtObject getFrameItem(size_t frame, size_t slot) { return stack[frames[frames.size() - frame - 1].offset + slot]; }
	inline void setFrameItem(size_t slot, OtObject object) { stack[frames.back().offset + slot] = object; }
	inline void setFrameItem(size_t frame, size_t slot, OtObject object) { stack[frames[frames.size() - frame - 1].offset + slot] = object; }
	inline OtStackFrame& getFrame(size_t frame) { return frames[frame]; }
	inline OtStackFrame& getFrame() { return frames.back(); }
	inline void closeFrame() { frames.pop_back(); }
	inline size_t getFrameCount() { return frames.size(); }

	// closure access functions
	inline void pushClosure(OtObject closure) { closures.emplace_back(closure); }
	inline OtObject getClosure() { return closures.back(); }
	inline void popClosure() { closures.pop_back(); }

	// manipulate stack state
	inline OtStackState getState() {
		return OtStackState(sp, frames.size(), closures.size());
	}

	inline void restoreState(OtStackState& state) {
		sp = state.stack;
		frames.resize(state.frames);
		closures.resize(state.closures);
	}

	// clear the stack
	inline void clear() {
		if (stack) {
			for (auto i = stack; i < stack + capacity; i++) {
				*i = nullptr;
			}

			sp = 0;

			frames.clear();
			closures.clear();
		}
	}

private:
	// reserve specified capacity on stack
	inline void reserve(size_t newCapacity) {
		auto newStack = new OtObject[newCapacity];

		if (stack) {
			for (size_t i = 0; i < capacity; i++) {
				newStack[i] = stack[i];
			}

			delete[] stack;
		}

		stack = newStack;
		capacity = newCapacity;
	}

	// release the stack's memory
	inline void release() {
		if (stack) {
			for (auto i = stack; i < stack + capacity; i++) {
				i->~OtObject();
			}

			delete[] stack;
			stack = nullptr;
			sp = 0;
			capacity = 0;

			frames.clear();
			closures.clear();
		}
	}

	// properties
	OtObject* stack = nullptr;
	size_t sp = 0;
	size_t capacity = 0;

	std::vector<OtStackFrame> frames;
	std::vector<OtObject> closures;
};
