//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "object.h"
#include "bytecode.h"
#include "global.h"
#include "singleton.h"
#include "stack.h"


//
//	The ObjectTalk Virtual Machine
//


class OtVM : public OtPerThreadSingleton<OtVM> {
public:
	// constructor
	OtVM();

	// get engine parameters
	static OtGlobal getGlobal() { return OtVM::instance().global; }
	static OtStack getStack() { return OtVM::instance().stack; }
	static OtObject getNull() { return OtVM::instance().null; }

	// execute bytecode in the virtual machine
	static OtObject execute(OtByteCode bytecode, size_t callingParameters=0) {
		return OtVM::instance().exec(bytecode, callingParameters);
	}

	// call a member function on an object with arguments
	template<typename... ARGS>
	static OtObject callMemberFunction(OtObject target, const std::string& member, ARGS... args) {
		const size_t count = sizeof...(ARGS) + 1;
		auto stack = OtVM::getStack();
		stack->push(target);
		(stack->push(args), ...);
		auto result = target->get(member)->operator () (count, stack->sp(count));
		stack->pop(count);
		return result;
	}

	static inline OtObject callMemberFunction(OtObject target, OtObject member, size_t count, OtObject* args) {
		auto stack = OtVM::instance().stack;
		stack->push(target);

		for (auto c = 0; c < count; c ++) {
			(stack->push(args[c]));
		}

		count++;
		auto result = member->operator () (count, stack->sp(count));
		stack->pop(count);
		return result;
	}

	// redirect member function to a new target
	static inline OtObject redirectMemberFunction(OtObject target, OtObject member, size_t count) {
		auto sp = OtVM::instance().stack->sp(count + 1);
		*sp = target;
		return member->operator () (count + 1, sp);
	}

	static inline OtObject redirectMemberFunction(OtObject target, const std::string& member, size_t count) {
		auto sp = OtVM::instance().stack->sp(count + 1);
		*sp = target;
		return target->get(member)->operator () (count + 1, sp);
	}

private:
	// execute bytecode in the virtual machine
	OtObject exec(OtByteCode bytecode, size_t callingParameters=0);

	// VM properties
	OtStack stack;
	OtGlobal global;
	OtObject null;
};
