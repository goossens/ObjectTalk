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
	OtStackFrame(OtByteCode b, size_t c) : bytecode(b), callingParameters(c) {}

	// frame data
	OtByteCode bytecode;
	size_t callingParameters;
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
	// stack access functions
	inline OtObject back() { return stack.back(); }
	inline void push(OtObject& object) { stack.emplace_back(object); }

	template<typename T>
	inline void push(T& object) { stack.emplace_back(object); }

	inline OtObject pop() { auto value = stack.back(); stack.pop_back(); return value; }
	inline void pop(size_t count) { stack.resize(stack.size() - count); }
	inline void dup() { stack.emplace_back(stack.back()); }
	inline void swap() { std::swap(stack[stack.size() - 1], stack[stack.size() - 2]); }
	inline void move(size_t count) { auto e = stack.rbegin(); std::rotate(e, e + 1, e + count + 1); }

	inline size_t size() { return stack.size(); }
	inline void resize(size_t size) { stack.resize(size); }
	inline void reserve() { stack.resize(stack.size() + 1); }
	inline OtObject* sp(size_t offset) { return &(stack[stack.size() - offset]); }

	// frame access functions
	inline void openFrame(OtByteCode bytecode, size_t callingParameters) { frames.emplace_back(bytecode, stack.size() - callingParameters); }
	inline OtObject getFrameItem(OtStackItem item) { return stack[frames[frames.size() - item.frame - 1].callingParameters + item.slot]; }
	inline void setFrameItem(OtStackItem item, OtObject object) { stack[frames[frames.size() - item.frame - 1].callingParameters + item.slot] = object; }
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
		return OtStackState(stack.size(), frames.size(), closures.size());
	}

	inline void restoreState(OtStackState& state) {
		stack.resize(state.stack);
		frames.resize(state.frames);
		closures.resize(state.closures);
	}

	// debug stack
	std::string debug() {
		std::stringstream buffer;

		buffer << "Stack:" << std::endl;
		buffer << "------" << std::endl << std::endl;
		size_t count = 0;

		for (auto element: stack) {
			buffer << std::setw(3) << std::setfill('0') << count++ << std::setfill(' ') << " ";
			buffer << OtObjectDescribe(element) << std::endl;
		}

		buffer	<< std::endl << "frames:" << std::endl;
		buffer << "-------" << std::endl << std::endl;
		count = 0;

		for (auto frame: frames) {
			buffer << std::setw(3) << std::setfill('0') << count++ << std::setfill(' ') << " ";
			buffer << frame.callingParameters << std::endl;
		}

		return buffer.str();
	}

private:
	std::vector<OtObject> stack;
	std::vector<OtStackFrame> frames;
	std::vector<OtObject> closures;
};
