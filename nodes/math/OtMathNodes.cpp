//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "OtNodesFactory.h"


//
//	Ot1by1Node
//

class Ot1by1Node : public OtNodeClass {
public:
	// constructor
	inline Ot1by1Node(const char* name) : OtNodeClass(name, OtNodeClass::math) {}

	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addOutputPin("Result", result);
	}

protected:
	float a = 0.0f;
	float result = 0.0f;
};

#define OT_NODE_NAME(NAME) Ot ## NAME ## Node

#define OT_NODE_1_BY_1(NAME, FUNCTION) 										\
class OT_NODE_NAME(NAME) : public Ot1by1Node {								\
public:																		\
	inline OT_NODE_NAME(NAME)() : Ot1by1Node(#NAME) {}						\
	inline void onExecute() override { result = FUNCTION; }					\
	static constexpr const char* name = #NAME;								\
};																			\
																			\
static OtNodesFactoryRegister<OT_NODE_NAME(NAME)> type ## NAME("Math");


OT_NODE_1_BY_1(Neg, -a)
OT_NODE_1_BY_1(Abs, std::abs(a))

OT_NODE_1_BY_1(Sin, std::sin(a))
OT_NODE_1_BY_1(Cos, std::cos(a))
OT_NODE_1_BY_1(Tan, std::tan(a))


//
//	Ot2by1Node
//

class Ot2by1Node : public OtNodeClass {
public:
	// constructor
	inline Ot2by1Node(const char* name) : OtNodeClass(name, OtNodeClass::math) {}

	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addInputPin("B", b);
		addOutputPin("Result", result);
	}

protected:
	float a = 0.0f;
	float b = 0.0f;
	float result = 0.0f;
};


#define OT_NODE_2_BY_1(NAME, FUNCTION) 										\
class OT_NODE_NAME(NAME) : public Ot2by1Node {								\
public:																		\
	inline OT_NODE_NAME(NAME)() : Ot2by1Node(#NAME) {}						\
	inline void onExecute() override { result = FUNCTION; }					\
	static constexpr const char* name = #NAME;								\
};																			\
																			\
static OtNodesFactoryRegister<OT_NODE_NAME(NAME)> type ## NAME("Math");

OT_NODE_2_BY_1(Add, a + b)
OT_NODE_2_BY_1(Subtract, a - b)
OT_NODE_2_BY_1(Multiply, a * b)
OT_NODE_2_BY_1(Divide, a / (b == 0.0f ? 0.000001 : b))

OT_NODE_2_BY_1(Modulo, std::fmod(a, b))
OT_NODE_2_BY_1(Remainder, std::remainder(a, b))
OT_NODE_2_BY_1(Pow, std::pow(a, b))

OT_NODE_2_BY_1(Min, std::min(a, b))
OT_NODE_2_BY_1(Max, std::max(a, b))