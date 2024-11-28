//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <functional>

#include "OtObject.h"
#include "OtByteCode.h"
#include "OtClosure.h"
#include "OtGlobal.h"
#include "OtIdentifier.h"
#include "OtSingleton.h"
#include "OtStack.h"


//
//	The ObjectTalk Virtual Machine
//

class OtVM : OtPerThreadSingleton<OtVM> {
public:
	// execute bytecode in the virtual machine
	static inline OtObject execute(OtByteCode bytecode, size_t callingParameters=0) { return instance().executeByteCode(bytecode, callingParameters); }

	// call a member function on an object with arguments
	template<typename... ARGS>
	static inline OtObject callMemberFunction(OtObject target, OtID member, ARGS... args) {
		auto& stack = instance().stack;
		const size_t count = sizeof...(ARGS) + 1;
		stack.push(target);
		(stack.push(args), ...);
		auto result = target->get(member)->operator () (count, stack.getSP(count));
		stack.pop(count);
		return result;
	}

	template<typename... ARGS>
	static inline OtObject callMemberFunction(OtObject target, const std::string& member, ARGS... args) {
		return callMemberFunction(target, OtIdentifier::create(member), std::forward<ARGS>(args)...);
	}

	static inline OtObject callMemberFunction(OtObject target, OtObject member, size_t count, OtObject* args) {
		auto& stack = instance().stack;
		stack.push(target);
		stack.push(count, args);
		count++;
		auto result = member->operator () (count, stack.getSP(count));
		stack.pop(count);
		return result;
	}

	static inline OtObject callMemberFunction(OtObject target, OtID member, size_t count, OtObject* args) {
		auto& stack = instance().stack;
		stack.push(target);
		stack.push(count, args);
		count++;
		auto result = target->get(member)->operator () (count, stack.getSP(count));
		stack.pop(count);
		return result;
	}

	// redirect member function to a new target
	static inline OtObject redirectMemberFunction(OtObject target, OtObject member, size_t count) {
		auto sp = instance().stack.getSP(count + 1);
		*sp = target;
		return member->operator () (count + 1, sp);
	}

	// debugging functions
	static inline void setStatementHook(std::function<void()> hook) {
		instance().statementHook = hook;
		instance().callHook = hook != nullptr;
	}

	// get engine parameters
	static inline OtStack* getStack() { return &instance().stack; }
	static inline OtGlobal getGlobal() { return instance().global; }
	static inline OtObject getNull() { return instance().null; }

	static inline OtClosure getClosure() { return instance().stack.getClosure(); }
	static inline OtByteCode getByteCode() { return instance().stack.getFrame().bytecode; }
	static inline size_t getPC() { return *instance().stack.getFrame().pc; }

private:
	// execute bytecode in the virtual machine
	OtObject executeByteCode(OtByteCode bytecode, size_t callingParameters);

	// clear the virtual machine (releases any memory still used by the engine)
	// virtual machine is no longer usable after this call

	// it is only here to destruct objects that might possibly allocate UI resources
	// VMs are thread singletons that are destructed when the thread ends (which is to late to release UI resources)

	// it should only be called by the UI framework
	friend class OtFramework;

	static inline void clear() {
		auto& vm = instance();
		vm.stack.clear();
		vm.global = nullptr;
		vm.null = nullptr;
	}

	// VM properties
	OtStack stack;
	OtGlobal global = OtGlobal::create();
	OtObject null = OtObject::create();

	// debugging support
	std::function<void()> statementHook;
	bool callHook = false;
};
