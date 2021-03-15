//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <vector>
#include <algorithm>

#include "ot/exception.h"
#include "ot/text.h"
#include "ot/string.h"
#include "ot/code.h"


//
//	OtTryCatch
//

class OtTryCatch {
public:
	OtTryCatch(size_t p, size_t s) : pc(p), sp(s) {}
	size_t pc;
	size_t sp;
};


//
//	OtCodeClass::operator ()
//

OtObject OtCodeClass::operator ()(OtContext context, size_t start, size_t end) {
	// calculate end if required
	if (end == SIZE_MAX) {
		end = size();
	}

	// stack
	std::vector<OtObject> stack;

	// try/catch stack
	std::vector<OtTryCatch> tryCatch;

	// local variables
	size_t pc = start;
	OtObject* sp;
	OtObject value;
	OtContext ctx;
	size_t cnt;
	size_t mark;

	// execute all instructions
	while (pc < end) {
		try {
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

				case OtInstruction::PUSH_TRY:
					// start a new try/catch cycle
					tryCatch.push_back(OtTryCatch(at(pc).integer, stack.size()));
					break;

				case OtInstruction::POP_TRY:
					// start a new try/catch cycle
					tryCatch.pop_back();
					break;

				case OtInstruction::PUSH_CONTEXT:
					// push current context
					ctx = stack.back()->cast<OtContextClass>();
					stack.pop_back();
					ctx->setParent(context);
					context = ctx;
					break;

				case OtInstruction::POP_CONTEXT:
					// return to previous context
					ctx = context->getParent();
					context->setParent(nullptr);
					context = ctx;
					break;
			}

		} catch (const OtException& e) {
			// do we have an exception handler
			if (tryCatch.size()) {
				// yes, use it
				OtTryCatch trycatch = tryCatch.back();
				tryCatch.pop_back();

				// restore program counter and stack
				pc = trycatch.pc - 1;
				stack.resize(trycatch.sp);

				// put exception on stack
				stack.push_back(OtStringClass::create(e.what()));

			} else {
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

				OT_EXCEPT("%s:\n%s", e.what(), statement.c_str());
			}
		}

		pc++;
	}

	OT_ASSERT(stack.size() == 1);
	return stack.back();
}

OtCode OtCodeClass::create(const std::string& source) {
	return std::make_shared<OtCodeClass>(source);
}
