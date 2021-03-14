//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.



#pragma once


//
//	Include files
//

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include "object.h"
#include "context.h"


//
//	OtInstruction
//

class OtInstruction {
public:
	typedef enum {
		MARK,
		PUSH,
		POP,
		POP_COUNT,
		DUP,
		SWAP,
		JUMP,
		JUMP_TRUE,
		JUMP_FALSE,
		METHOD,
		EXIT,
		PUSH_CONTEXT,
		POP_CONTEXT
	} OtOpcode;

	OtInstruction(OtOpcode o) : opcode(o), integer(0), value(nullptr) {}
	OtInstruction(OtOpcode o, long i) : opcode(o), integer(i), value(nullptr) {}
	OtInstruction(OtOpcode o, const std::string& s) : opcode(o), string(s), integer(0), value(nullptr) {}
	OtInstruction(OtOpcode o, const std::string& s, long i) : opcode(o), string(s), integer(i), value(nullptr) {}
	OtInstruction(OtOpcode o, OtObject v) : opcode(o), integer(0), value(v) {}

	OtOpcode opcode;
	long integer;
	std::string string;
	OtObject value;
};


//
//	OtCode
//

class OtCodeClass;
typedef std::shared_ptr<OtCodeClass> OtCode;

class OtCodeClass : public std::vector<OtInstruction> {
public:
	// constructor
	OtCodeClass(const std::string& s) : source(s) {}

	// add instructions to code
	void mark(size_t mark) { push_back(OtInstruction(OtInstruction::MARK, mark)); }
	void push(OtObject value) { push_back(OtInstruction(OtInstruction::PUSH, value)); }
	void pop() { push_back(OtInstruction(OtInstruction::POP)); }
	void pop(size_t count) { push_back(OtInstruction(OtInstruction::POP_COUNT, count)); }
	void dup() { push_back(OtInstruction(OtInstruction::DUP)); }
	void swap() { push_back(OtInstruction(OtInstruction::SWAP)); }
	void jump(size_t offset) { push_back(OtInstruction(OtInstruction::JUMP, offset)); }
	void jumpTrue(size_t offset) { push_back(OtInstruction(OtInstruction::JUMP_TRUE, offset)); }
	void jumpFalse(size_t offset) { push_back(OtInstruction(OtInstruction::JUMP_FALSE, offset)); }
	void method(const std::string& name, size_t count) { push_back(OtInstruction(OtInstruction::METHOD, name, count)); }
	void exit() { push_back(OtInstruction(OtInstruction::EXIT)); }
	void pushContext() { push_back(OtInstruction(OtInstruction::PUSH_CONTEXT)); }
	void popContext() { push_back(OtInstruction(OtInstruction::POP_CONTEXT)); }

	// patch jump instruction
	void patch(size_t location) { at(location).integer = size(); }

	// execute code
	OtObject operator ()(OtContext context, size_t start=0, size_t end=SIZE_MAX);

	// create new code object
	static OtCode create(const std::string& source);

private:
	std::string source;
};
