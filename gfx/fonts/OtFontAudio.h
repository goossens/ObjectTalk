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
	static constexpr const char* adr = u8"\uf101";
	static constexpr const char* adsr = u8"\uf102";
	static constexpr const char* ahdsr = u8"\uf103";
	static constexpr const char* ar = u8"\uf104";
	static constexpr const char* armRecording = u8"\uf105";
	static constexpr const char* arpChord = u8"\uf106";
	static constexpr const char* arpDown = u8"\uf107";
	static constexpr const char* arpDownAndUp = u8"\uf108";
	static constexpr const char* arpDownUp = u8"\uf109";
	static constexpr const char* arpPlayArder = u8"\uf10a";
	static constexpr const char* arpRandom = u8"\uf10b";
	static constexpr const char* arpUp = u8"\uf10c";
	static constexpr const char* arpUpAnDown = u8"\uf10d";
	static constexpr const char* arpUpDown = u8"\uf10e";
	static constexpr const char* arrowsHorz = u8"\uf10f";
	static constexpr const char* arrowsVert = u8"\uf110";
	static constexpr const char* automation2p = u8"\uf111";
	static constexpr const char* automation3p = u8"\uf112";
	static constexpr const char* automation4p = u8"\uf113";
	static constexpr const char* backward = u8"\uf114";
	static constexpr const char* bluetooth = u8"\uf115";
	static constexpr const char* caretDown = u8"\uf116";
	static constexpr const char* caretLeft = u8"\uf117";
	static constexpr const char* caretRight = u8"\uf118";
	static constexpr const char* caretUp = u8"\uf119";
	static constexpr const char* close = u8"\uf11a";
	static constexpr const char* copy = u8"\uf11b";
	static constexpr const char* cpu = u8"\uf11c";
	static constexpr const char* cutter = u8"\uf11d";
	static constexpr const char* digitalColon = u8"\uf11e";
	static constexpr const char* digitalDot = u8"\uf11f";
	static constexpr const char* digital0 = u8"\uf120";
	static constexpr const char* digital1 = u8"\uf121";
	static constexpr const char* cigital2 = u8"\uf122";
	static constexpr const char* digital3 = u8"\uf123";
	static constexpr const char* digital4 = u8"\uf124";
	static constexpr const char* digital5 = u8"\uf125";
	static constexpr const char* digital6 = u8"\uf126";
	static constexpr const char* digital7 = u8"\uf127";
	static constexpr const char* digital8 = u8"\uf128";
	static constexpr const char* digital9 = u8"\uf129";
	static constexpr const char* diskio = u8"\uf12a";
	static constexpr const char* drumPad = u8"\uf12b";
	static constexpr const char* duplicate = u8"\uf12c";
	static constexpr const char* eraser = u8"\uf12d";
	static constexpr const char* fwd = u8"\uf12e";
	static constexpr const char* filterBandpass = u8"\uf12f";
	static constexpr const char* filterBell = u8"\uf130";
	static constexpr const char* filterBypass = u8"\uf131";
	static constexpr const char* filterHighpass = u8"\uf132";
	static constexpr const char* filterLowpass = u8"\uf133";
	static constexpr const char* filterNotch = u8"\uf134";
	static constexpr const char* filterRezHighpass = u8"\uf135";
	static constexpr const char* filterRezLowpass = u8"\uf136";
	static constexpr const char* filterShelvingHi = u8"\uf137";
	static constexpr const char* filterShelvingLo = u8"\uf138";
	static constexpr const char* foldback = u8"\uf139";
	static constexpr const char* forward = u8"\uf13a";
	static constexpr const char* hExpand = u8"\uf13b";
	static constexpr const char* hardClip = u8"\uf13c";
	static constexpr const char* hardClipCurve = u8"\uf13d";
	static constexpr const char* headphones = u8"\uf13e";
	static constexpr const char* keyboard = u8"\uf13f";
	static constexpr const char* lock = u8"\uf140";
	static constexpr const char* logoAax = u8"\uf141";
	static constexpr const char* lLogoAbletonLink = u8"\uf142";
	static constexpr const char* logoAu = u8"\uf143";
	static constexpr const char* logoAudacity = u8"\uf144";
	static constexpr const char* logoAudiobus = u8"\uf145";
	static constexpr const char* logoCubase = u8"\uf146";
	static constexpr const char* logoFl = u8"\uf147";
	static constexpr const char* logoJuce = u8"\uf148";
	static constexpr const char* logoLadspa = u8"\uf149";
	static constexpr const char* logoLive = u8"\uf14a";
	static constexpr const char* logoLv2 = u8"\uf14b";
	static constexpr const char* logoProtools = u8"\uf14c";
	static constexpr const char* logoRackext = u8"\uf14d";
	static constexpr const char* logoReaper = u8"\uf14e";
	static constexpr const char* logoReason = u8"\uf14f";
	static constexpr const char* logoRewire = u8"\uf150";
	static constexpr const char* logoStudioone = u8"\uf151";
	static constexpr const char* logoTracktion = u8"\uf152";
	static constexpr const char* logoVst = u8"\uf153";
	static constexpr const char* logoWaveform = u8"\uf154";
	static constexpr const char* loop = u8"\uf155";
	static constexpr const char* metronome = u8"\uf156";
	static constexpr const char* microphone = u8"\uf157";
	static constexpr const char* midiplug = u8"\uf158";
	static constexpr const char* modRandom = u8"\uf159";
	static constexpr const char* modSawDown = u8"\uf15a";
	static constexpr const char* modSawUp = u8"\uf15b";
	static constexpr const char* modSh = u8"\uf15c";
	static constexpr const char* modSine = u8"\uf15d";
	static constexpr const char* modSquare = u8"\uf15e";
	static constexpr const char* modTriangle = u8"\uf15f";
	static constexpr const char* modularplug = u8"\uf160";
	static constexpr const char* mono = u8"\uf161";
	static constexpr const char* mute = u8"\uf162";
	static constexpr const char* next = u8"\uf163";
	static constexpr const char* open = u8"\uf164";
	static constexpr const char* paste = u8"\uf165";
	static constexpr const char* pause = u8"\uf166";
	static constexpr const char* pen = u8"\uf167";
	static constexpr const char* phase = u8"\uf168";
	static constexpr const char* play = u8"\uf169";
	static constexpr const char* pointer = u8"\uf16a";
	static constexpr const char* powerSwitch = u8"\uf16b";
	static constexpr const char* presetA = u8"\uf16c";
	static constexpr const char* presetAb = u8"\uf16d";
	static constexpr const char* presetB = u8"\uf16e";
	static constexpr const char* presetBa = u8"\uf16f";
	static constexpr const char* prev = u8"\uf170";
	static constexpr const char* punchIn = u8"\uf171";
	static constexpr const char* punchOut = u8"\uf172";
	static constexpr const char* ram = u8"\uf173";
	static constexpr const char* random1dice = u8"\uf174";
	static constexpr const char* random2dice = u8"\uf175";
	static constexpr const char* record = u8"\uf176";
	static constexpr const char* redo = u8"\uf177";
	static constexpr const char* repeatOne = u8"\uf178";
	static constexpr const char* repeat = u8"\uf179";
	static constexpr const char* rew = u8"\uf17a";
	static constexpr const char* roundSwitchOff = u8"\uf17b";
	static constexpr const char* roundSwitchOn = u8"\uf17c";
	static constexpr const char* save = u8"\uf17d";
	static constexpr const char* saveas = u8"\uf17e";
	static constexpr const char* scissors = u8"\uf17f";
	static constexpr const char* shuffle = u8"\uf180";
	static constexpr const char* sliderRound1 = u8"\uf181";
	static constexpr const char* sliderRound2 = u8"\uf182";
	static constexpr const char* sliderRound3 = u8"\uf183";
	static constexpr const char* sliderHandle1 = u8"\uf184";
	static constexpr const char* sliderHandle2 = u8"\uf185";
	static constexpr const char* softClip = u8"\uf186";
	static constexpr const char* softClipCurve = u8"\uf187";
	static constexpr const char* solo = u8"\uf188";
	static constexpr const char* speaker = u8"\uf189";
	static constexpr const char* squareSwitchOff = u8"\uf18a";
	static constexpr const char* squareSwitchOn = u8"\uf18b";
	static constexpr const char* stereo = u8"\uf18c";
	static constexpr const char* stop = u8"\uf18d";
	static constexpr const char* thunderbolt = u8"\uf18e";
	static constexpr const char* timeSelect = u8"\uf18f";
	static constexpr const char* undo = u8"\uf190";
	static constexpr const char* unlock = u8"\uf191";
	static constexpr const char* usb = u8"\uf192";
	static constexpr const char* vExpand = u8"\uf193";
	static constexpr const char* vRoundSwitchOff = u8"\uf194";
	static constexpr const char* vRoundSwitchOn = u8"\uf195";
	static constexpr const char* vSquareSwitchOff = u8"\uf196";
	static constexpr const char* vSquareSwitchOn = u8"\uf197";
	static constexpr const char* waveform = u8"\uf198";
	static constexpr const char* xlrPlug = u8"\uf199";
	static constexpr const char* zoomIn = u8"\uf19a";
	static constexpr const char* zoomOut = u8"\uf19b";

private:
	static uint8_t fontData[36812];
	static constexpr size_t fontSize = 36812;
};
