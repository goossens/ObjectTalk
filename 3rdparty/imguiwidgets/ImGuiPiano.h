/*
The MIT License (MIT)
by https://github.com/frowrik

Piano keyboard for Dear ImGui

example:
static int PrevNoteActive = -1;
ImGui_PianoKeyboard("PianoTest", ImVec2(1024, 100), &PrevNoteActive, 21, 108, TestPianoBoardFunct, nullptr, nullptr);

bool TestPianoBoardFunct(void* UserData, int Msg, int Key, float Vel) {
		if (Key >= 128) return false; // midi max keys
		if (Msg == NoteGetStatus) return KeyPressed[Key];
		if (Msg == NoteOn) { KeyPressed[Key] = true; Send_Midi_NoteOn(Key, Vel*127); }
		if (Msg == NoteOff) { KeyPressed[Key] = false; Send_Midi_NoteOff(Key, Vel*127);}
		return false;
}

*/

#pragma once

#include "imgui.h"

enum ImGuiPianoKeyboardMsg {
	NoteGetStatus,
	NoteOn,
	NoteOff,
};

using ImGuiPianoKeyboardProc = bool (*)(void* UserData, int Msg, int Key, float Vel);

struct ImGuiPianoStyles {
	ImU32 Colors[5]{
		IM_COL32(255, 255, 255, 255),	// light note
		IM_COL32(0, 0, 0, 255),			// dark note
		IM_COL32(255, 255, 0, 255),		// active light note
		IM_COL32(200, 200, 0, 255),		// active dark note
		IM_COL32(75, 75, 75, 255),		// background
	};
	float NoteDarkHeight = 2.0f / 3.0f; // dark note scale h
	float NoteDarkWidth  = 2.0f / 3.0f;	// dark note scale w
};

void ImGui_PianoKeyboard(const char* IDName, ImVec2 Size, int* PrevNoteActive, int BeginOctaveNote, int EndOctaveNote, ImGuiPianoKeyboardProc Callback, void* UserData, ImGuiPianoStyles* Style = nullptr);
