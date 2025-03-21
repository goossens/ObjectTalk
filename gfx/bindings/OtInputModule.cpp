//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "imgui.h"

#include "OtFunction.h"
#include "OtInteger.h"
#include "OtModule.h"
#include "OtReal.h"

#include "OtMathModule.h"


//
//	RegisterEnums
//

static void RegisterEnums(OtModule& module) {
	module->set("keyTab", OtInteger::create(ImGuiKey_Tab));
	module->set("keyLeftArrow", OtInteger::create(ImGuiKey_LeftArrow));
	module->set("keyRightArrow", OtInteger::create(ImGuiKey_RightArrow));
	module->set("keyUpArrow", OtInteger::create(ImGuiKey_UpArrow));
	module->set("keyDownArrow", OtInteger::create(ImGuiKey_DownArrow));
	module->set("keyPageUp", OtInteger::create(ImGuiKey_PageUp));
	module->set("keyPageDown", OtInteger::create(ImGuiKey_PageDown));
	module->set("keyHome", OtInteger::create(ImGuiKey_Home));
	module->set("keyEnd", OtInteger::create(ImGuiKey_End));
	module->set("keyInsert", OtInteger::create(ImGuiKey_Insert));
	module->set("keyDelete", OtInteger::create(ImGuiKey_Delete));
	module->set("keyBackspace", OtInteger::create(ImGuiKey_Backspace));
	module->set("keySpace", OtInteger::create(ImGuiKey_Space));
	module->set("keyEnter", OtInteger::create(ImGuiKey_Enter));
	module->set("keyEscape", OtInteger::create(ImGuiKey_Escape));
	module->set("keyLeftCtrl", OtInteger::create(ImGuiKey_LeftCtrl));
	module->set("keyLeftShift", OtInteger::create(ImGuiKey_LeftShift));
	module->set("keyLeftAlt", OtInteger::create(ImGuiKey_LeftAlt));
	module->set("keyLeftSuper", OtInteger::create(ImGuiKey_LeftSuper));
	module->set("keyRightCtrl", OtInteger::create(ImGuiKey_RightCtrl));
	module->set("keyRightShift", OtInteger::create(ImGuiKey_RightShift));
	module->set("keyRightAlt", OtInteger::create(ImGuiKey_RightAlt));
	module->set("keyRightSuper", OtInteger::create(ImGuiKey_RightSuper));
	module->set("keyMenu", OtInteger::create(ImGuiKey_Menu));
	module->set("key0", OtInteger::create(ImGuiKey_0));
	module->set("key1", OtInteger::create(ImGuiKey_1));
	module->set("key2", OtInteger::create(ImGuiKey_2));
	module->set("key3", OtInteger::create(ImGuiKey_3));
	module->set("key4", OtInteger::create(ImGuiKey_4));
	module->set("key5", OtInteger::create(ImGuiKey_5));
	module->set("key6", OtInteger::create(ImGuiKey_6));
	module->set("key7", OtInteger::create(ImGuiKey_7));
	module->set("key8", OtInteger::create(ImGuiKey_8));
	module->set("key9", OtInteger::create(ImGuiKey_9));
	module->set("keyA", OtInteger::create(ImGuiKey_A));
	module->set("keyB", OtInteger::create(ImGuiKey_B));
	module->set("keyC", OtInteger::create(ImGuiKey_C));
	module->set("keyD", OtInteger::create(ImGuiKey_D));
	module->set("keyE", OtInteger::create(ImGuiKey_E));
	module->set("keyF", OtInteger::create(ImGuiKey_F));
	module->set("keyG", OtInteger::create(ImGuiKey_G));
	module->set("keyH", OtInteger::create(ImGuiKey_H));
	module->set("keyI", OtInteger::create(ImGuiKey_I));
	module->set("keyJ", OtInteger::create(ImGuiKey_J));
	module->set("keyK", OtInteger::create(ImGuiKey_K));
	module->set("keyL", OtInteger::create(ImGuiKey_L));
	module->set("keyM", OtInteger::create(ImGuiKey_M));
	module->set("keyN", OtInteger::create(ImGuiKey_N));
	module->set("keyO", OtInteger::create(ImGuiKey_O));
	module->set("keyP", OtInteger::create(ImGuiKey_P));
	module->set("keyQ", OtInteger::create(ImGuiKey_Q));
	module->set("keyR", OtInteger::create(ImGuiKey_R));
	module->set("keyS", OtInteger::create(ImGuiKey_S));
	module->set("keyT", OtInteger::create(ImGuiKey_T));
	module->set("keyU", OtInteger::create(ImGuiKey_U));
	module->set("keyV", OtInteger::create(ImGuiKey_V));
	module->set("keyW", OtInteger::create(ImGuiKey_W));
	module->set("keyX", OtInteger::create(ImGuiKey_X));
	module->set("keyY", OtInteger::create(ImGuiKey_Y));
	module->set("keyZ", OtInteger::create(ImGuiKey_Z));
	module->set("keyF1", OtInteger::create(ImGuiKey_F1));
	module->set("keyF2", OtInteger::create(ImGuiKey_F2));
	module->set("keyF3", OtInteger::create(ImGuiKey_F3));
	module->set("keyF4", OtInteger::create(ImGuiKey_F4));
	module->set("keyF5", OtInteger::create(ImGuiKey_F5));
	module->set("keyF6", OtInteger::create(ImGuiKey_F6));
	module->set("keyF7", OtInteger::create(ImGuiKey_F7));
	module->set("keyF8", OtInteger::create(ImGuiKey_F8));
	module->set("keyF9", OtInteger::create(ImGuiKey_F9));
	module->set("keyF10", OtInteger::create(ImGuiKey_F10));
	module->set("keyF11", OtInteger::create(ImGuiKey_F11));
	module->set("keyF12", OtInteger::create(ImGuiKey_F12));
	module->set("keyApostrophe", OtInteger::create(ImGuiKey_Apostrophe));
	module->set("keyComma", OtInteger::create(ImGuiKey_Comma));
	module->set("keyMinus", OtInteger::create(ImGuiKey_Minus));
	module->set("keyPeriod", OtInteger::create(ImGuiKey_Period));
	module->set("keySlash", OtInteger::create(ImGuiKey_Slash));
	module->set("keySemicolon", OtInteger::create(ImGuiKey_Semicolon));
	module->set("keyEqual", OtInteger::create(ImGuiKey_Equal));
	module->set("keyLeftBracket", OtInteger::create(ImGuiKey_LeftBracket));
	module->set("keyBackslash", OtInteger::create(ImGuiKey_Backslash));
	module->set("keyRightBracket", OtInteger::create(ImGuiKey_RightBracket));
	module->set("keyGraveAccent", OtInteger::create(ImGuiKey_GraveAccent));
	module->set("keyCapsLock", OtInteger::create(ImGuiKey_CapsLock));
	module->set("keyScrollLock", OtInteger::create(ImGuiKey_ScrollLock));
	module->set("keyNumLock", OtInteger::create(ImGuiKey_NumLock));
	module->set("keyPrintScreen", OtInteger::create(ImGuiKey_PrintScreen));
	module->set("keyPause", OtInteger::create(ImGuiKey_Pause));
	module->set("keyKeypad0", OtInteger::create(ImGuiKey_Keypad0));
	module->set("keyKeypad1", OtInteger::create(ImGuiKey_Keypad1));
	module->set("keyKeypad2", OtInteger::create(ImGuiKey_Keypad2));
	module->set("keyKeypad3", OtInteger::create(ImGuiKey_Keypad3));
	module->set("keyKeypad4", OtInteger::create(ImGuiKey_Keypad4));
	module->set("keyKeypad5", OtInteger::create(ImGuiKey_Keypad5));
	module->set("keyKeypad6", OtInteger::create(ImGuiKey_Keypad6));
	module->set("keyKeypad7", OtInteger::create(ImGuiKey_Keypad7));
	module->set("keyKeypad8", OtInteger::create(ImGuiKey_Keypad8));
	module->set("keyKeypad9", OtInteger::create(ImGuiKey_Keypad9));
	module->set("keyKeypadDecimal", OtInteger::create(ImGuiKey_KeypadDecimal));
	module->set("keyKeypadDivide", OtInteger::create(ImGuiKey_KeypadDivide));
	module->set("keyKeypadMultiply", OtInteger::create(ImGuiKey_KeypadMultiply));
	module->set("keyKeypadSubtract", OtInteger::create(ImGuiKey_KeypadSubtract));
	module->set("keyKeypadAdd", OtInteger::create(ImGuiKey_KeypadAdd));
	module->set("keyKeypadEnter", OtInteger::create(ImGuiKey_KeypadEnter));
	module->set("keyKeypadEqual", OtInteger::create(ImGuiKey_KeypadEqual));
	module->set("gamepadStart", OtInteger::create(ImGuiKey_GamepadStart));
	module->set("gamepadBack", OtInteger::create(ImGuiKey_GamepadBack));
	module->set("gamepadFaceLeft", OtInteger::create(ImGuiKey_GamepadFaceLeft));
	module->set("gamepadFaceRight", OtInteger::create(ImGuiKey_GamepadFaceRight));
	module->set("gamepadFaceUp", OtInteger::create(ImGuiKey_GamepadFaceUp));
	module->set("gamepadFaceDown", OtInteger::create(ImGuiKey_GamepadFaceDown));
	module->set("gamepadDpadLeft", OtInteger::create(ImGuiKey_GamepadDpadLeft));
	module->set("gamepadDpadRight", OtInteger::create(ImGuiKey_GamepadDpadRight));
	module->set("gamepadDpadUp", OtInteger::create(ImGuiKey_GamepadDpadUp));
	module->set("gamepadDpadDown", OtInteger::create(ImGuiKey_GamepadDpadDown));
	module->set("gamepadL1", OtInteger::create(ImGuiKey_GamepadL1));
	module->set("gamepadR1", OtInteger::create(ImGuiKey_GamepadR1));
	module->set("gamepadL2", OtInteger::create(ImGuiKey_GamepadL2));
	module->set("gamepadR2", OtInteger::create(ImGuiKey_GamepadR2));
	module->set("gamepadL3", OtInteger::create(ImGuiKey_GamepadL3));
	module->set("gamepadR3", OtInteger::create(ImGuiKey_GamepadR3));
	module->set("gamepadLStickLeft", OtInteger::create(ImGuiKey_GamepadLStickLeft));
	module->set("gamepadLStickRight", OtInteger::create(ImGuiKey_GamepadLStickRight));
	module->set("gamepadLStickUp", OtInteger::create(ImGuiKey_GamepadLStickUp));
	module->set("gamepadLStickDown", OtInteger::create(ImGuiKey_GamepadLStickDown));
	module->set("gamepadRStickLeft", OtInteger::create(ImGuiKey_GamepadRStickLeft));
	module->set("gamepadRStickRight", OtInteger::create(ImGuiKey_GamepadRStickRight));
	module->set("gamepadRStickUp", OtInteger::create(ImGuiKey_GamepadRStickUp));
	module->set("gamepadRStickDown", OtInteger::create(ImGuiKey_GamepadRStickDown));
	module->set("mouseLeft", OtInteger::create(ImGuiKey_MouseLeft));
	module->set("mouseRight", OtInteger::create(ImGuiKey_MouseRight));
	module->set("mouseMiddle", OtInteger::create(ImGuiKey_MouseMiddle));
	module->set("mouseX1", OtInteger::create(ImGuiKey_MouseX1));
	module->set("mouseX2", OtInteger::create(ImGuiKey_MouseX2));
	module->set("mouseWheelX", OtInteger::create(ImGuiKey_MouseWheelX));
	module->set("mouseWheelY", OtInteger::create(ImGuiKey_MouseWheelY));
	module->set("modCtrl", OtInteger::create(ImGuiMod_Ctrl));
	module->set("modShift", OtInteger::create(ImGuiMod_Shift));
	module->set("modAlt", OtInteger::create(ImGuiMod_Alt));
	module->set("modSuper", OtInteger::create(ImGuiMod_Super));
}


//
//	Functions
//

static bool isButtonDown(int key) {
	return ImGui::IsKeyDown((ImGuiKey) key);
}

static bool isButtonReleased(int key) {
	return ImGui::IsKeyReleased((ImGuiKey) key);
}

static bool isButtonPressed(int key) {
	return ImGui::IsKeyPressed((ImGuiKey) key);
}

static bool isDoubleClicked() {
	return ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
}

static bool hasMouseMoved() {
	static bool initialized = false;
	static ImVec2 mousePos;

	if (initialized) {
		auto newMousePos = ImGui::GetMousePos();
		auto moved = newMousePos.x != mousePos.x || newMousePos.y != mousePos.y;
		mousePos = newMousePos;
		return moved;

	} else {
		mousePos = ImGui::GetMousePos();
		initialized = true;
		return false;
	}
}

static OtObject getMousePos() {
	auto pos = ImGui::GetMousePos();
	return OtVec2::create(pos.x, pos.y);
}

static OtObject getMouseDrag() {
	auto drag = ImGui::GetMouseDragDelta();
	ImGui::ResetMouseDragDelta();
	return OtVec2::create(drag.x, drag.y);
}

static OtObject getDeltaTime() {
	return OtReal::create(ImGui::GetIO().DeltaTime);
}

static OtObject getTime() {
	return OtReal::create(ImGui::GetTime());
}


//
//	Module registration
//

static OtModuleRegistration registration{"input", [](OtModule module) {
	// register enums
	RegisterEnums(module);

	// register functions
	module->set("isButtonDown", OtFunction::create(&isButtonDown));
	module->set("isButtonReleased", OtFunction::create(&isButtonReleased));
	module->set("isButtonPressed", OtFunction::create(&isButtonPressed));
	module->set("isDoubleClicked", OtFunction::create(&isDoubleClicked));
	module->set("hasMouseMoved", OtFunction::create(&hasMouseMoved));
	module->set("getMousePos", OtFunction::create(&getMousePos));
	module->set("getMouseDrag", OtFunction::create(&getMouseDrag));

	module->set("getDeltaTime", OtFunction::create(&getDeltaTime));
	module->set("getTime", OtFunction::create(&getTime));
}};
