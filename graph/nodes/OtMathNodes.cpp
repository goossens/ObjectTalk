//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2024 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include <cmath>

#include "OtGraphNode.h"
#include "OtMathNodes.h"


//
//	OtGraphNode1by1
//

class OtGraphNode1by1 : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNode1by1(const char* name) : OtGraphNodeClass(name, OtGraphNodeClass::math) {}

	// configure node
	inline void configure() override {
		addInputPin("A", a);
		addOutputPin("Result", result);
	}

protected:
	float a = 0.0f;
	float result = 0.0f;
};


#define OT_GRAPH_NODE_1_BY_1(NAME, FUNCTION) 								\
class OtGraphNode ## NAME : public OtGraphNode1by1 {						\
public:																		\
	inline OtGraphNode ## NAME() : OtGraphNode1by1(#NAME) {}				\
	inline void onExecute() override { result = FUNCTION; }					\
}

OT_GRAPH_NODE_1_BY_1(Neg, -a);
OT_GRAPH_NODE_1_BY_1(Abs, std::abs(a));

OT_GRAPH_NODE_1_BY_1(Sin, std::sin(a));
OT_GRAPH_NODE_1_BY_1(Cos, std::cos(a));
OT_GRAPH_NODE_1_BY_1(Tan, std::tan(a));


//
//	OtGraphNode2by1
//

class OtGraphNode2by1 : public OtGraphNodeClass {
public:
	// constructor
	inline OtGraphNode2by1(const char* name) : OtGraphNodeClass(name, OtGraphNodeClass::math) {}

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


#define OT_GRAPH_NODE_2_BY_1(NAME, FUNCTION) 								\
class OtGraphNode ## NAME : public OtGraphNode2by1 {						\
public:																		\
	inline OtGraphNode ## NAME() : OtGraphNode2by1(#NAME) {}				\
	inline void onExecute() override { result = FUNCTION; }					\
}

OT_GRAPH_NODE_2_BY_1(Add, a + b);
OT_GRAPH_NODE_2_BY_1(Subtract, a - b);
OT_GRAPH_NODE_2_BY_1(Multiply, a * b);
OT_GRAPH_NODE_2_BY_1(Divide, a / b);

OT_GRAPH_NODE_2_BY_1(Modulo, std::fmod(a, b));
OT_GRAPH_NODE_2_BY_1(Remainder, std::remainder(a, b));
OT_GRAPH_NODE_2_BY_1(Pow, std::pow(a, b));

OT_GRAPH_NODE_2_BY_1(Min, std::min(a, b));
OT_GRAPH_NODE_2_BY_1(Max, std::max(a, b));


//
//	OtMathNodesRegister
//

#define REGISTER(NAME) \
	graph.registerNodeType<OtGraphNode ## NAME>("Math", #NAME)

void OtMathNodesRegister(OtGraph& graph) {
	REGISTER(Add);
	REGISTER(Subtract);
	REGISTER(Multiply);
	REGISTER(Divide);

	REGISTER(Neg);
	REGISTER(Abs);

	REGISTER(Min);
	REGISTER(Max);

	REGISTER(Sin);
	REGISTER(Cos);
	REGISTER(Tan);

	REGISTER(Modulo);
	REGISTER(Remainder);
	REGISTER(Pow);
}
