//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include "OtObject.h"

#include "OtMessageComponent.h"


//
//	OtMessageComponentObjectClass
//

class OtMessageComponentObjectClass;
using OtMessageComponentObject = OtObjectPointer<OtMessageComponentObjectClass>;

class OtMessageComponentObjectClass : public OtObjectClass {
public:
	// manage message
	inline void show(const std::string& msg, float visible, float fade) { message->show(msg, visible, fade); }
	inline void hide() { message->hide(); }

	// get type definition
	static OtType getMeta();

protected:
	// constructor
	friend class OtObjectPointer<OtMessageComponentObjectClass>;
	OtMessageComponentObjectClass() = default;
	OtMessageComponentObjectClass(OtMessageComponent* component) : message(component) {}

private:
	// connection to component
	OtMessageComponent* message;
};
