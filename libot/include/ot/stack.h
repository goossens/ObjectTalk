//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
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
#include <vector>

#include "object.h"


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

	void set(size_t slot, OtObject object) { stack[slot] = std::move(object); }
	OtObject get(size_t slot) { return stack[slot]; }

	size_t size() { return stack.size(); }
	void resize(size_t size) { stack.resize(size); }
	void reserve(size_t size) { stack.resize(stack.size() + size); }
	OtObject* sp(size_t offset) { return &(stack[stack.size() - offset]); }

	// stack frame access functions
	void openFrame(size_t offset=0) { frames.push_back(stack.size() - offset); }
	size_t getFrame() { return frames.back(); }
	void closeFrame() { frames.pop_back(); }

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

		buffer  << std::endl << "frames:" << std::endl;
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
};
