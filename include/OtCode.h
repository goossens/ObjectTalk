//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


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

	// call code
	OtObject operator ()(OtObject context, size_t start=0, size_t end=SIZE_MAX) {
		// calculate end if required
		if (end == SIZE_MAX) {
			end = size();
		}

		// stack
		std::vector<OtObject> stack;

		// local variables
		size_t pc = start;
		OtObject* sp;
		OtObject value;
		size_t cnt;
		size_t mark;

		try {
			// execute all instructions
			while (pc < end) {
				switch (at(pc).opcode) {
					case OtInstruction::MARK:
						mark = at(pc).integer;
						break;

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
						pc = end;
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
						value = context->getParent();
						context->setParent(nullptr);
						context = value;
						break;
				}

				pc++;
			}

		} catch (const OtException& e) {
			// find start of line
			size_t startOfLine = source.rfind('\n', mark);

			if (startOfLine == std::string::npos) {
				startOfLine = 0;

			} else {
				startOfLine++;
			}

			// find end of line
			size_t nextMark = 0;
			size_t endOfLine;

			for (auto i = pc; !nextMark && i < end; i++) {
				if (at(i).opcode == OtInstruction::MARK) {
					nextMark = at(i).integer;
				}
			}

			if (nextMark == 0) {
				nextMark = source.size();
			}

			auto p = source.find_last_not_of("\t\n\v\f\r ", nextMark - 1);
			endOfLine = source.find('\n', p);

			if (endOfLine == std::string::npos) {
				endOfLine = source.size();
			}

			// find starting line number
			auto line = std::count(source.begin(), source.begin() + startOfLine, '\n') + 1;

			// get text of offending line(s)
			auto lines = source.substr(startOfLine, endOfLine - startOfLine);

			// format nicely
			std::string statement;

			OtTextSplitIterator(lines.data(), lines.data() + lines.size(), ';', [&](const char *b, const char *e) {
				std::string text(b, e - b);
				statement += OtFormat("Line %ld: %s", line++, text.c_str());
			});

			throw OtException(OtFormat("%s:\n%s", e.what(), statement.c_str()));
		}

		OT_ASSERT(stack.size() == 1);
		return stack.back();
	}

	static OtCode create(const std::string& source) {
		return std::make_shared<OtCodeClass>(source);
	}

private:
	std::string source;
};
