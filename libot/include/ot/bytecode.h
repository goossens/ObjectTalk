//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
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

#include "exception.h"
#include "source.h"
#include "object.h"
#include "internal.h"


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
		METHOD,
		EXIT,
		PUSH_TRY,
		POP_TRY
	} OtOpcode;

	// constructors
	OtByteCodeClass() = default;
	OtByteCodeClass(OtSource s) : source(s) {}

	// add instructions to code
	void debug() { emitOpcode(DEBUG); }
	void mark(size_t mark) { emitOpcode(MARK); emitMark(mark); }
	void push(OtObject value) { emitOpcode(PUSH); emitConstant(value); }
	void pop() { emitOpcode(POP); }
	void pop(size_t count) { emitOpcode(POP_COUNT); emitByte(count); }
	void dup() { emitOpcode(DUP); }
	void swap() { emitOpcode(SWAP); }
	void reserve() { emitOpcode(RESERVE); }
	void move(size_t count) { emitOpcode(MOVE); emitByte(count); }
	size_t jump(size_t offset) { emitOpcode(JUMP); return emitOffset(offset); }
	size_t jumpTrue(size_t offset) { emitOpcode(JUMP_TRUE); return emitOffset(offset); }
	size_t jumpFalse(size_t offset) { emitOpcode(JUMP_FALSE); return emitOffset(offset); }
	void method(const std::string& name, size_t count) { emitOpcode(METHOD); emitString(name); emitNumber(count); }
	void exit() { emitOpcode(EXIT); }
	size_t pushTry() { emitOpcode(PUSH_TRY); return emitOffset(0); }
	void popTry() { emitOpcode(POP_TRY); }

	void patchJump(size_t offset) { offsets[offset] = bytecode.size(); }

	// get current code size
	size_t size() { return bytecode.size(); }

	// get opcode
	inline uint8_t getOpcode(size_t& pc) {
		auto opcode = bytecode[pc++];
		return opcode;
	}

	// get variable leght number
	inline size_t getNumber(size_t& pc) {
		size_t result = 0;
		size_t digit = 0;

		while (bytecode[pc] & 128) {
			result |= (bytecode[pc++] & 127) << (7 * digit++);
		}

		return result | bytecode[pc++] << (7 * digit);
	}

	// get code parts
	inline OtSource getSource() { return source; }
	uint8_t* getCode() { return bytecode.data(); }
	inline OtObject getConstant(size_t index) { return constants[index]; }
	inline std::string getString(size_t index) { return strings[index]; }
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

	inline void emitString(const std::string string) {
		auto index = strings.size();
		strings.push_back(string);
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

	inline void emitNumber(size_t index) {
		while (index > 127) {
			bytecode.push_back(((uint8_t)(index & 127)) | 128);
			index >>= 7;
		}

		bytecode.push_back((uint8_t) index);
	}

	inline size_t emitByte(size_t index) {
		auto offset = bytecode.size();

		if (index > 127) {
			OtExcept("You have more than 127 local variables [%ld]. REALLY!", index);
		}

		bytecode.push_back(index);
		return offset;
	}

	OtSource source;
	std::vector<uint8_t> bytecode;
	std::vector<OtObject> constants;
	std::vector<std::string> strings;
	std::vector<size_t> marks;
	std::vector<size_t> offsets;
};
