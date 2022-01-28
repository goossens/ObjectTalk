//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>

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
	bool operator == (const OtStackItem& item) const {
		return frame == item.frame && slot == item.slot;
	}

	bool operator != (const OtStackItem& item) const {
		return frame != item.frame || slot != item.slot;
	}

	// stack item address
	size_t frame = 0;
	size_t slot = 0;
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

class OtStackClass;
typedef std::shared_ptr<OtStackClass> OtStack;

class OtStackClass {
public:
	// stack access functions
	void push(OtObject object) { stack.push_back(std::move(object)); }
	OtObject pop() { auto value = stack.back(); stack.pop_back(); return value; }
	void pop(size_t count) { stack.resize(stack.size() - count); }
	void dup() { stack.push_back(stack.back()); }
	void swap() { std::swap(stack[stack.size() - 1], stack[stack.size() - 2]); }
	void move(size_t count) { auto e = stack.rbegin(); std::rotate(e, e + 1, e + count + 1); }

	size_t size() { return stack.size(); }
	void resize(size_t size) { stack.resize(size); }
	void reserve() { stack.resize(stack.size() + 1); }
	OtObject* sp(size_t offset) { return &(stack[stack.size() - offset]); }

	// stack frame access functions
	void openFrame(size_t offset=0) { frames.push_back(stack.size() - offset); }
	OtObject getFrameItem(OtStackItem item) { return stack[frames[frames.size() - item.frame - 1] + item.slot]; }
	void setFrameItem(OtStackItem item, OtObject object) { stack[frames[frames.size() - item.frame - 1] + item.slot] = object; }
	void closeFrame() { frames.pop_back(); }

	// closure access functions
	void pushClosure(OtObject closure) { closures.push_back(std::move(closure)); }
	OtObject getClosure() { return closures.back(); }
	void popClosure() { closures.pop_back(); }

	// manipulate stack state
	OtStackState getState() { return OtStackState(stack.size(), frames.size(), closures.size()); }
	void restoreState(OtStackState state) { stack.resize(state.stack); frames.resize(state.frames); closures.resize(state.closures); }

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
			buffer << frame << std::endl;
		}

		return buffer.str();
	}

	// create a new stack
	static OtStack create() { return std::make_shared<OtStackClass>(); }

private:
	std::vector<OtObject> stack;
	std::vector<size_t> frames;
	std::vector<OtObject> closures;
};
