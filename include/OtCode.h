//	ObjectTalk Scripting Language
//	Copyright 1993-2021 Johan A. Goossens
//
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.


//
//	Include files
//

#include <memory>
#include <string>
#include <utility>
#include <vector>


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
	// add instructions to code
	void mark(long value) { push_back(OtInstruction(OtInstruction::MARK, value)); }
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

	// call code
	OtObject operator ()(OtObject context) {
		// program counter
		size_t pc = 0;

		// stack
		std::vector<OtObject> stack;

		// local variables
		OtObject* sp;
		OtObject value;
		size_t cnt;

		// execute all instructions
		while (pc < size()) {
			switch (at(pc).opcode) {
				case OtInstruction::PUSH:
					stack.push_back(at(pc).value);
					break;

				case OtInstruction::POP:
					stack.pop_back();
					break;

				case OtInstruction::POP_COUNT:
					stack.resize(stack.size() - at(pc).integer);
					break;

				case OtInstruction::DUP:
					stack.push_back(stack.back());
					break;

				case OtInstruction::SWAP:
					std::swap(stack[stack.size() - 1], stack[stack.size() - 2]);
					break;

				case OtInstruction::JUMP:
					pc = at(pc).integer - 1;
					break;

				case OtInstruction::JUMP_TRUE:
					value = stack.back();
					stack.pop_back();

					if (value->operator bool()) {
						pc = at(pc).integer - 1;
					}

					break;

				case OtInstruction::JUMP_FALSE:
					value = stack.back();
					stack.pop_back();

					if (!value->operator bool()) {
						pc = at(pc).integer - 1;
					}

					break;

				case OtInstruction::METHOD:
					// get target object and call method
					cnt = at(pc).integer + 1;
					sp = &stack[stack.size() - cnt];
					value = sp[0]->get(at(pc).string)->operator ()(context, cnt, sp);

					// clean up stack
					stack.resize(stack.size() - cnt);
					stack.push_back(value);
					break;

				case OtInstruction::EXIT:
					// exit instructions
					pc = size();
					break;

				case OtInstruction::PUSH_CONTEXT:
					// push current context
					value = stack.back();
					stack.pop_back();
					value->setParent(context);
					context = value;
					break;

				case OtInstruction::POP_CONTEXT:
					// return to previous context
					context = context->getParent();
					break;
			}

			pc++;
		}

		return stack.back();
	}

	static OtCode create() { return std::make_shared<OtCodeClass>(); }
};
