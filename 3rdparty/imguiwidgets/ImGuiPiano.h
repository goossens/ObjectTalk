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

#include "imgui.h"
#include "imgui_internal.h"

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

void ImGui_PianoKeyboard(const char* IDName, ImVec2 Size, int* PrevNoteActive, int BeginOctaveNote, int EndOctaveNote, ImGuiPianoKeyboardProc Callback, void* UserData, ImGuiPianoStyles* Style = nullptr) {
	// const
	static int NoteIsDark[12] = { 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0 };
	static int NoteLightNumber[12] = { 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6, 7 };
	static float NoteDarkOffset[12] = { 0.0f,  -2.0f / 3.0f, 0.0f, -1.0f / 3.0f, 0.0f, 0.0f, -2.0f / 3.0f, 0.0f, -0.5f, 0.0f, -1.0f / 3.0f, 0.0f };

	// fix range dark keys
	if (NoteIsDark[BeginOctaveNote % 12] > 0) BeginOctaveNote++;
	if (NoteIsDark[EndOctaveNote % 12] > 0) EndOctaveNote--;

	// bad range
	if (!IDName || !Callback || BeginOctaveNote < 0 || EndOctaveNote < 0 || EndOctaveNote <= BeginOctaveNote) return;

	// style
	static ImGuiPianoStyles ColorsBase;
	if (!Style) Style = &ColorsBase;

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) return;

	const ImGuiID id = window->GetID(IDName);

	ImDrawList* draw_list = window->DrawList;

	ImVec2 Pos = window->DC.CursorPos;
	ImVec2 MousePos = ImGui::GetIO().MousePos;

	// sizes
	int CountNotesAllign7 = (EndOctaveNote / 12 - BeginOctaveNote / 12) * 7 + NoteLightNumber[EndOctaveNote % 12] - (NoteLightNumber[BeginOctaveNote % 12] - 1);

	float NoteHeight	= Size.y;
	float NoteWidth		= Size.x / (float)CountNotesAllign7;

	float NoteHeight2	= NoteHeight * Style->NoteDarkHeight;
	float NoteWidth2	= NoteWidth * Style->NoteDarkWidth;

	// minimal size draw
	if (NoteHeight < 5.0 || NoteWidth < 3.0) return;

	// minimal size using mouse
	bool isMouseInput = (NoteHeight >= 10.0 && NoteWidth >= 5.0);

	// item
	const ImRect bb(Pos, Pos + Size);
	ImGui::ItemSize(Size, 0);
	if (!ImGui::ItemAdd(bb, id)) return;

	// item input
	bool held = false;
	if (isMouseInput) {
		ImGui::ButtonBehavior(bb, id, nullptr, &held, 0);
	}

	int		NoteMouseCollision = -1;
	float	NoteMouseVel = 0.0f;

	float OffsetX = bb.Min.x;
	float OffsetY = bb.Min.y;
	float OffsetY2 = OffsetY + NoteHeight;
	for (int RealNum = BeginOctaveNote; RealNum <= EndOctaveNote; RealNum++) {
		int i		= RealNum % 12;

		if (NoteIsDark[i] > 0) continue;

		ImRect NoteRect(
			OffsetX,
			OffsetY,
			OffsetX + NoteWidth,
			OffsetY2
		);

		if (held && NoteRect.Contains(MousePos)) {
			NoteMouseCollision	= RealNum;
			NoteMouseVel		= (MousePos.y - NoteRect.Min.y) / NoteHeight;
		}

		bool isActive = Callback(UserData, NoteGetStatus, RealNum, 0.0f);

		draw_list->AddRectFilled(	NoteRect.Min, NoteRect.Max, Style->Colors[isActive ? 2 : 0], 0.0f);

		draw_list->AddRect(			NoteRect.Min, NoteRect.Max, Style->Colors[4], 0.0f);

		OffsetX += NoteWidth;
	}

	// draw dark notes
	OffsetX = bb.Min.x;
	OffsetY = bb.Min.y;
	OffsetY2 = OffsetY + NoteHeight2;
	for (int RealNum = BeginOctaveNote; RealNum <= EndOctaveNote; RealNum++) {
		int i		= RealNum % 12;

		if (NoteIsDark[i] == 0)  {
			OffsetX += NoteWidth;
			continue;
		}

		float OffsetDark = NoteDarkOffset[i] * NoteWidth2;
		ImRect NoteRect(
			OffsetX + OffsetDark,
			OffsetY,
			OffsetX + NoteWidth2 + OffsetDark,
			OffsetY2
		);

		if (held && NoteRect.Contains(MousePos)) {
			NoteMouseCollision	= RealNum;
			NoteMouseVel		= (MousePos.y - NoteRect.Min.y) / NoteHeight2;
		}

		bool isActive = Callback(UserData, NoteGetStatus, RealNum, 0.0f);

		draw_list->AddRectFilled(	NoteRect.Min, NoteRect.Max, Style->Colors[isActive ? 3 : 1], 0.0f);
		draw_list->AddRect(			NoteRect.Min, NoteRect.Max, Style->Colors[4], 0.0f);
	}

	// mouse note click
	if (*PrevNoteActive != NoteMouseCollision) {
		Callback(UserData, NoteOff, *PrevNoteActive, 0.0f);
		*PrevNoteActive = -1;

		if (held && NoteMouseCollision >= 0) {
			Callback(UserData, NoteOn, NoteMouseCollision, NoteMouseVel);
			*PrevNoteActive = NoteMouseCollision;
		}
	}
}
