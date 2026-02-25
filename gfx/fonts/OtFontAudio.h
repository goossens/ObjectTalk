//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstddef>
#include <cstdint>


//
//	OtFontAudio
//

class OtFontAudio {
public:
	// access font
	inline static uint8_t* data() { return fontData; }
	inline static size_t size() { return fontSize; }

	// glyph information
#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || (__cplusplus >= 202002L)
	using Type = const char8_t*;
#else
	using Type = const char*;
#endif

	static constexpr Type zoomOut = u8"\uf101";
	static constexpr Type zoomIn = u8"\uf102";
	static constexpr Type xlrPlug = u8"\uf103";
	static constexpr Type waveform = u8"\uf104";
	static constexpr Type vSquareSwitchOn = u8"\uf105";
	static constexpr Type vSquareSwitchOff = u8"\uf106";
	static constexpr Type vRoundSwitchOn = u8"\uf107";
	static constexpr Type vRoundSwitchOff = u8"\uf108";
	static constexpr Type vExpand = u8"\uf109";
	static constexpr Type usb = u8"\uf10a";
	static constexpr Type unlock = u8"\uf10b";
	static constexpr Type undo = u8"\uf10c";
	static constexpr Type tuning = u8"\uf10d";
	static constexpr Type timeSelect = u8"\uf10e";
	static constexpr Type thunderbolt = u8"\uf10f";
	static constexpr Type stop = u8"\uf110";
	static constexpr Type stereo = u8"\uf111";
	static constexpr Type squareSwitchOn = u8"\uf112";
	static constexpr Type squareSwitchOff = u8"\uf113";
	static constexpr Type speaker = u8"\uf114";
	static constexpr Type solo = u8"\uf115";
	static constexpr Type softClipCurve = u8"\uf116";
	static constexpr Type softClip = u8"\uf117";
	static constexpr Type sliderRound3 = u8"\uf118";
	static constexpr Type sliderRound2 = u8"\uf119";
	static constexpr Type sliderRound1 = u8"\uf11a";
	static constexpr Type sliderHandle2 = u8"\uf11b";
	static constexpr Type sliderHandle1 = u8"\uf11c";
	static constexpr Type shuffle = u8"\uf11d";
	static constexpr Type scissors = u8"\uf11e";
	static constexpr Type saveAs = u8"\uf11f";
	static constexpr Type save = u8"\uf120";
	static constexpr Type roundSwitchOn = u8"\uf121";
	static constexpr Type roundSwitchOff = u8"\uf122";
	static constexpr Type rew = u8"\uf123";
	static constexpr Type repeatOne = u8"\uf124";
	static constexpr Type repeat = u8"\uf125";
	static constexpr Type redo = u8"\uf126";
	static constexpr Type record = u8"\uf127";
	static constexpr Type ram = u8"\uf128";
	static constexpr Type punchOut = u8"\uf129";
	static constexpr Type punchIn = u8"\uf12a";
	static constexpr Type prev = u8"\uf12b";
	static constexpr Type presetBA = u8"\uf12c";
	static constexpr Type presetB = u8"\uf12d";
	static constexpr Type presetAB = u8"\uf12e";
	static constexpr Type presetA = u8"\uf12f";
	static constexpr Type powerswitch = u8"\uf130";
	static constexpr Type pointer = u8"\uf131";
	static constexpr Type play = u8"\uf132";
	static constexpr Type phase = u8"\uf133";
	static constexpr Type pen = u8"\uf134";
	static constexpr Type pause = u8"\uf135";
	static constexpr Type paste = u8"\uf136";
	static constexpr Type open = u8"\uf137";
	static constexpr Type next = u8"\uf138";
	static constexpr Type mute = u8"\uf139";
	static constexpr Type mono = u8"\uf13a";
	static constexpr Type modularplug = u8"\uf13b";
	static constexpr Type modTriangle = u8"\uf13c";
	static constexpr Type modSquare = u8"\uf13d";
	static constexpr Type modSine = u8"\uf13e";
	static constexpr Type modSharkTooth = u8"\uf13f";
	static constexpr Type modSh = u8"\uf140";
	static constexpr Type modSawUp = u8"\uf141";
	static constexpr Type modSawDown = u8"\uf142";
	static constexpr Type modRandom = u8"\uf143";
	static constexpr Type midiplug = u8"\uf144";
	static constexpr Type microphone = u8"\uf145";
	static constexpr Type metronome = u8"\uf146";
	static constexpr Type loop = u8"\uf147";
	static constexpr Type lock = u8"\uf148";
	static constexpr Type keyboard = u8"\uf149";
	static constexpr Type headphones = u8"\uf14a";
	static constexpr Type hardClipCurve = u8"\uf14b";
	static constexpr Type hardClip = u8"\uf14c";
	static constexpr Type hExpand = u8"\uf14d";
	static constexpr Type forward = u8"\uf14e";
	static constexpr Type foldback = u8"\uf14f";
	static constexpr Type filterShelvingLo = u8"\uf150";
	static constexpr Type filterShelvingHi = u8"\uf151";
	static constexpr Type filterRezLowpass = u8"\uf152";
	static constexpr Type filterRezHighpass = u8"\uf153";
	static constexpr Type filterNotch = u8"\uf154";
	static constexpr Type filterLowpass = u8"\uf155";
	static constexpr Type filterHighpass = u8"\uf156";
	static constexpr Type filterBypass = u8"\uf157";
	static constexpr Type filterBell = u8"\uf158";
	static constexpr Type filterBandpass = u8"\uf159";
	static constexpr Type ffwd = u8"\uf15a";
	static constexpr Type eraser = u8"\uf15b";
	static constexpr Type duplicate = u8"\uf15c";
	static constexpr Type drumpad = u8"\uf15d";
	static constexpr Type diskio = u8"\uf15e";
	static constexpr Type digitalDot = u8"\uf15f";
	static constexpr Type digitalColon = u8"\uf160";
	static constexpr Type digital9 = u8"\uf161";
	static constexpr Type digital8 = u8"\uf162";
	static constexpr Type digital7 = u8"\uf163";
	static constexpr Type digital6 = u8"\uf164";
	static constexpr Type digital5 = u8"\uf165";
	static constexpr Type digital4 = u8"\uf166";
	static constexpr Type digital3 = u8"\uf167";
	static constexpr Type digital2 = u8"\uf168";
	static constexpr Type digital1 = u8"\uf169";
	static constexpr Type digital0 = u8"\uf16a";
	static constexpr Type cutter = u8"\uf16b";
	static constexpr Type cpu = u8"\uf16c";
	static constexpr Type copy = u8"\uf16d";
	static constexpr Type close = u8"\uf16e";
	static constexpr Type caretUp = u8"\uf16f";
	static constexpr Type caretRight = u8"\uf170";
	static constexpr Type caretLeft = u8"\uf171";
	static constexpr Type caretDown = u8"\uf172";
	static constexpr Type bluetooth = u8"\uf173";
	static constexpr Type backward = u8"\uf174";
	static constexpr Type automation4p = u8"\uf175";
	static constexpr Type automation3p = u8"\uf176";
	static constexpr Type automation2p = u8"\uf177";
	static constexpr Type arrowsVert = u8"\uf178";
	static constexpr Type arrowsHorz = u8"\uf179";
	static constexpr Type arpUpandDown = u8"\uf17a";
	static constexpr Type arpUpDown = u8"\uf17b";
	static constexpr Type arpUp = u8"\uf17c";
	static constexpr Type arpRandom = u8"\uf17d";
	static constexpr Type arpPlayOrder = u8"\uf17e";
	static constexpr Type arpDownUp = u8"\uf17f";
	static constexpr Type arpDownAndUp = u8"\uf180";
	static constexpr Type arpDown = u8"\uf181";
	static constexpr Type arpChord = u8"\uf182";
	static constexpr Type armRecording = u8"\uf183";
	static constexpr Type ar = u8"\uf184";
	static constexpr Type ahdsr = u8"\uf185";
	static constexpr Type adsr = u8"\uf186";
	static constexpr Type adr = u8"\uf187";

private:
	static uint8_t fontData[27496];
	static constexpr size_t fontSize = 27496;
};

