//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.



#pragma once


//
//	Include files
//

#include <cstdint>
#include <string>
#include <vector>

#include "OtException.h"
#include "OtInternal.h"
#include "OtObject.h"
#include "OtSource.h"


//
//	OtByteCode
//

class OtByteCodeClass;
typedef std::shared_ptr<OtByteCodeClass> OtByteCode;

class OtByteCodeClass : public OtInternalClass {
public:
	// possible opcodes
	typedef enum {
		DEBUG,
		MARK,
		PUSH,
		POP,
		POP_COUNT,
		DUP,
		SWAP,
		MOVE,
		RESERVE,
		JUMP,
		JUMP_TRUE,
		JUMP_FALSE,
		MEMBER,
		METHOD,
		EXIT,
		PUSH_TRY,
		POP_TRY
	} OtOpcode;

	// constructors
	OtByteCodeClass() = default;
	OtByteCodeClass(OtSource s) : source(s) {}

	// add instructions to code
	inline void debug() { emitOpcode(DEBUG); }
	inline void mark(size_t mark) { emitOpcode(MARK); emitMark(mark); }
	inline void push(OtObject value) { emitOpcode(PUSH); emitConstant(value); }
	inline void pop() { emitOpcode(POP); }
	inline void pop(size_t count) { emitOpcode(POP_COUNT); emitByte(count); }
	inline void dup() { emitOpcode(DUP); }
	inline void swap() { emitOpcode(SWAP); }
	inline void reserve() { emitOpcode(RESERVE); }
	inline void move(size_t count) { emitOpcode(MOVE); emitByte(count); }
	inline size_t jump(size_t offset) { emitOpcode(JUMP); return emitOffset(offset); }
	inline size_t jumpTrue(size_t offset) { emitOpcode(JUMP_TRUE); return emitOffset(offset); }
	inline size_t jumpFalse(size_t offset) { emitOpcode(JUMP_FALSE); return emitOffset(offset); }
	inline void member(const char* name) { emitOpcode(MEMBER); emitSelector(name); }
	inline void member(const std::string& name) { emitOpcode(MEMBER); emitSelector(name); }
	inline void method(const char* name, size_t count) { emitOpcode(METHOD); emitSelector(name); emitNumber(count); }
	inline void method(const std::string& name, size_t count) { emitOpcode(METHOD); emitSelector(name); emitNumber(count); }
	inline void exit() { emitOpcode(EXIT); }
	inline size_t pushTry() { emitOpcode(PUSH_TRY); return emitOffset(0); }
	inline void popTry() { emitOpcode(POP_TRY); }

	// patch previous jump
	inline void patchJump(size_t offset) { offsets[offset] = bytecode.size(); }

	// get current code size
	inline size_t size() { return bytecode.size(); }

	// get opcode
	inline uint8_t getOpcode(size_t& pc) {
		auto opcode = bytecode[pc++];
		return opcode;
	}

	// get variable length number
	inline size_t getNumber(size_t& pc) {
		size_t result = 0;
		size_t digit = 0;

		while (bytecode[pc] & 0x80) {
			size_t value = bytecode[pc++] & 0x7f;
			result |= value << (7 * digit++);
		}

		return result | (size_t) bytecode[pc++] << (7 * digit);
	}

	// get code parts
	inline OtSource getSource() { return source; }
	uint8_t* getCode() { return bytecode.data(); }
	inline OtObject getConstant(size_t index) { return constants[index]; }
	inline size_t getSelector(size_t index) { return selectors[index]; }
	inline size_t getOffset(size_t index) { return offsets[index]; }
	inline size_t getMark(size_t index) { return marks[index]; }
	inline size_t getNextMark(size_t index) { return index + 1 < marks.size() ? marks[index + 1] : source->size(); }

	// disassemble the bytecode
	std::string disassemble();

	// get type definition
	static OtType getMeta();

	// create new code object
	static OtByteCode create(OtSource source);

private:
	// emit parts to bytecode
	inline void emitOpcode(OtOpcode opcode) {
		bytecode.push_back((uint8_t) opcode);
	}

	inline void emitConstant(OtObject constant) {
		auto index = constants.size();
		constants.push_back(constant);
		emitNumber(index);
	}

	inline void emitSelector(const char* name) {
		auto index = selectors.size();
		selectors.push_back(OtSelector::create(name));
		emitNumber(index);
	}

	inline void emitSelector(const std::string& name) {
		auto index = selectors.size();
		selectors.push_back(OtSelector::create(name));
		emitNumber(index);
	}

	inline void emitMark(size_t mark) {
		auto index = marks.size();
		marks.push_back(mark);
		emitNumber(index);
	}

	inline size_t emitOffset(size_t offset) {
		auto index = offsets.size();
		offsets.push_back(offset);
		emitNumber(index);
		return index;
	}

	inline void emitNumber(size_t number) {
		while (number > 127) {
			bytecode.push_back(((uint8_t)(number & 127)) | 128);
			number >>= 7;
		}

		bytecode.push_back((uint8_t) number);
	}

	inline void emitByte(size_t index) {
		if (index > 127) {
			OtExcept("You have more than 127 local variables [%ld]. REALLY!", index);
		}

		bytecode.push_back(index);
	}

	OtSource source;
	std::vector<uint8_t> bytecode;
	std::vector<OtObject> constants;
	std::vector<size_t> selectors;
	std::vector<size_t> marks;
	std::vector<size_t> offsets;
};
