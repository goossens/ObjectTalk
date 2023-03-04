//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtException.h"
#include "OtFunction.h"

#include "OtWidget.h"


//
//	Generic controller widget
//

#define OT_CONTROLLER_STRINGIFY(x) #x

#define OT_CONTROLLER(type)																\
																						\
class Ot##type##ControllerClass;														\
using Ot##type##Controller = OtObjectPointer<Ot##type##ControllerClass>;				\
																						\
class Ot##type##ControllerClass : public OtWidgetClass {								\
public:																					\
	void init(OtObject object)	{														\
		object->expectKindOf(#type);													\
		target = Ot##type(object);														\
	}																					\
																						\
	void render() {																		\
		target->renderGUI();															\
	}																					\
																						\
	static OtType getMeta() {															\
		static OtType type;																\
																						\
		if (!type) {																	\
			type = OtType::create<Ot##type##ControllerClass>(							\
				OT_CONTROLLER_STRINGIFY(type##Controller),								\
				OtWidgetClass::getMeta());												\
																						\
			type->set("__init__", OtFunction::create(									\
				&Ot##type##ControllerClass::init));										\
		}																				\
																						\
		return type;																	\
	}																					\
																						\
	static Ot##type##Controller create() {												\
		Ot##type##Controller object = Ot##type##Controller::create();					\
		object->setType(getMeta());														\
		return object;																	\
	}																					\
																						\
private:																				\
	Ot##type target;																	\
};
