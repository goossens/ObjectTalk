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
	inline static constexpr const char* adr = u8"\uf101";
	inline static constexpr const char* adsr = u8"\uf102";
	inline static constexpr const char* ahdsr = u8"\uf103";
	inline static constexpr const char* ar = u8"\uf104";
	inline static constexpr const char* armRecording = u8"\uf105";
	inline static constexpr const char* arpChord = u8"\uf106";
	inline static constexpr const char* arpDown = u8"\uf107";
	inline static constexpr const char* arpDownAndUp = u8"\uf108";
	inline static constexpr const char* arpDownUp = u8"\uf109";
	inline static constexpr const char* arpPlayArder = u8"\uf10a";
	inline static constexpr const char* arpRandom = u8"\uf10b";
	inline static constexpr const char* arpUp = u8"\uf10c";
	inline static constexpr const char* arpUpAnDown = u8"\uf10d";
	inline static constexpr const char* arpUpDown = u8"\uf10e";
	inline static constexpr const char* arrowsHorz = u8"\uf10f";
	inline static constexpr const char* arrowsVert = u8"\uf110";
	inline static constexpr const char* automation2p = u8"\uf111";
	inline static constexpr const char* automation3p = u8"\uf112";
	inline static constexpr const char* automation4p = u8"\uf113";
	inline static constexpr const char* backward = u8"\uf114";
	inline static constexpr const char* bluetooth = u8"\uf115";
	inline static constexpr const char* caretDown = u8"\uf116";
	inline static constexpr const char* caretLeft = u8"\uf117";
	inline static constexpr const char* caretRight = u8"\uf118";
	inline static constexpr const char* caretUp = u8"\uf119";
	inline static constexpr const char* close = u8"\uf11a";
	inline static constexpr const char* copy = u8"\uf11b";
	inline static constexpr const char* cpu = u8"\uf11c";
	inline static constexpr const char* cutter = u8"\uf11d";
	inline static constexpr const char* digitalColon = u8"\uf11e";
	inline static constexpr const char* digitalDot = u8"\uf11f";
	inline static constexpr const char* digital0 = u8"\uf120";
	inline static constexpr const char* digital1 = u8"\uf121";
	inline static constexpr const char* cigital2 = u8"\uf122";
	inline static constexpr const char* digital3 = u8"\uf123";
	inline static constexpr const char* digital4 = u8"\uf124";
	inline static constexpr const char* digital5 = u8"\uf125";
	inline static constexpr const char* digital6 = u8"\uf126";
	inline static constexpr const char* digital7 = u8"\uf127";
	inline static constexpr const char* digital8 = u8"\uf128";
	inline static constexpr const char* digital9 = u8"\uf129";
	inline static constexpr const char* diskio = u8"\uf12a";
	inline static constexpr const char* drumPad = u8"\uf12b";
	inline static constexpr const char* duplicate = u8"\uf12c";
	inline static constexpr const char* eraser = u8"\uf12d";
	inline static constexpr const char* fwd = u8"\uf12e";
	inline static constexpr const char* filterBandpass = u8"\uf12f";
	inline static constexpr const char* filterBell = u8"\uf130";
	inline static constexpr const char* filterBypass = u8"\uf131";
	inline static constexpr const char* filterHighpass = u8"\uf132";
	inline static constexpr const char* filterLowpass = u8"\uf133";
	inline static constexpr const char* filterNotch = u8"\uf134";
	inline static constexpr const char* filterRezHighpass = u8"\uf135";
	inline static constexpr const char* filterRezLowpass = u8"\uf136";
	inline static constexpr const char* filterShelvingHi = u8"\uf137";
	inline static constexpr const char* filterShelvingLo = u8"\uf138";
	inline static constexpr const char* foldback = u8"\uf139";
	inline static constexpr const char* forward = u8"\uf13a";
	inline static constexpr const char* hExpand = u8"\uf13b";
	inline static constexpr const char* hardClip = u8"\uf13c";
	inline static constexpr const char* hardClipCurve = u8"\uf13d";
	inline static constexpr const char* headphones = u8"\uf13e";
	inline static constexpr const char* keyboard = u8"\uf13f";
	inline static constexpr const char* lock = u8"\uf140";
	inline static constexpr const char* logoAax = u8"\uf141";
	inline static constexpr const char* lLogoAbletonLink = u8"\uf142";
	inline static constexpr const char* logoAu = u8"\uf143";
	inline static constexpr const char* logoAudacity = u8"\uf144";
	inline static constexpr const char* logoAudiobus = u8"\uf145";
	inline static constexpr const char* logoCubase = u8"\uf146";
	inline static constexpr const char* logoFl = u8"\uf147";
	inline static constexpr const char* logoJuce = u8"\uf148";
	inline static constexpr const char* logoLadspa = u8"\uf149";
	inline static constexpr const char* logoLive = u8"\uf14a";
	inline static constexpr const char* logoLv2 = u8"\uf14b";
	inline static constexpr const char* logoProtools = u8"\uf14c";
	inline static constexpr const char* logoRackext = u8"\uf14d";
	inline static constexpr const char* logoReaper = u8"\uf14e";
	inline static constexpr const char* logoReason = u8"\uf14f";
	inline static constexpr const char* logoRewire = u8"\uf150";
	inline static constexpr const char* logoStudioone = u8"\uf151";
	inline static constexpr const char* logoTracktion = u8"\uf152";
	inline static constexpr const char* logoVst = u8"\uf153";
	inline static constexpr const char* logoWaveform = u8"\uf154";
	inline static constexpr const char* loop = u8"\uf155";
	inline static constexpr const char* metronome = u8"\uf156";
	inline static constexpr const char* microphone = u8"\uf157";
	inline static constexpr const char* midiplug = u8"\uf158";
	inline static constexpr const char* modRandom = u8"\uf159";
	inline static constexpr const char* modSawDown = u8"\uf15a";
	inline static constexpr const char* modSawUp = u8"\uf15b";
	inline static constexpr const char* modSh = u8"\uf15c";
	inline static constexpr const char* modSine = u8"\uf15d";
	inline static constexpr const char* modSquare = u8"\uf15e";
	inline static constexpr const char* modTriangle = u8"\uf15f";
	inline static constexpr const char* modularplug = u8"\uf160";
	inline static constexpr const char* mono = u8"\uf161";
	inline static constexpr const char* mute = u8"\uf162";
	inline static constexpr const char* next = u8"\uf163";
	inline static constexpr const char* open = u8"\uf164";
	inline static constexpr const char* paste = u8"\uf165";
	inline static constexpr const char* pause = u8"\uf166";
	inline static constexpr const char* pen = u8"\uf167";
	inline static constexpr const char* phase = u8"\uf168";
	inline static constexpr const char* play = u8"\uf169";
	inline static constexpr const char* pointer = u8"\uf16a";
	inline static constexpr const char* powerSwitch = u8"\uf16b";
	inline static constexpr const char* presetA = u8"\uf16c";
	inline static constexpr const char* presetAb = u8"\uf16d";
	inline static constexpr const char* presetB = u8"\uf16e";
	inline static constexpr const char* presetBa = u8"\uf16f";
	inline static constexpr const char* prev = u8"\uf170";
	inline static constexpr const char* punchIn = u8"\uf171";
	inline static constexpr const char* punchOut = u8"\uf172";
	inline static constexpr const char* ram = u8"\uf173";
	inline static constexpr const char* random1dice = u8"\uf174";
	inline static constexpr const char* random2dice = u8"\uf175";
	inline static constexpr const char* record = u8"\uf176";
	inline static constexpr const char* redo = u8"\uf177";
	inline static constexpr const char* repeatOne = u8"\uf178";
	inline static constexpr const char* repeat = u8"\uf179";
	inline static constexpr const char* rew = u8"\uf17a";
	inline static constexpr const char* roundSwitchOff = u8"\uf17b";
	inline static constexpr const char* roundSwitchOn = u8"\uf17c";
	inline static constexpr const char* save = u8"\uf17d";
	inline static constexpr const char* saveas = u8"\uf17e";
	inline static constexpr const char* scissors = u8"\uf17f";
	inline static constexpr const char* shuffle = u8"\uf180";
	inline static constexpr const char* sliderRound1 = u8"\uf181";
	inline static constexpr const char* sliderRound2 = u8"\uf182";
	inline static constexpr const char* sliderRound3 = u8"\uf183";
	inline static constexpr const char* sliderHandle1 = u8"\uf184";
	inline static constexpr const char* sliderHandle2 = u8"\uf185";
	inline static constexpr const char* softClip = u8"\uf186";
	inline static constexpr const char* softClipCurve = u8"\uf187";
	inline static constexpr const char* solo = u8"\uf188";
	inline static constexpr const char* speaker = u8"\uf189";
	inline static constexpr const char* squareSwitchOff = u8"\uf18a";
	inline static constexpr const char* squareSwitchOn = u8"\uf18b";
	inline static constexpr const char* stereo = u8"\uf18c";
	inline static constexpr const char* stop = u8"\uf18d";
	inline static constexpr const char* thunderbolt = u8"\uf18e";
	inline static constexpr const char* timeSelect = u8"\uf18f";
	inline static constexpr const char* undo = u8"\uf190";
	inline static constexpr const char* unlock = u8"\uf191";
	inline static constexpr const char* usb = u8"\uf192";
	inline static constexpr const char* vExpand = u8"\uf193";
	inline static constexpr const char* vRoundSwitchOff = u8"\uf194";
	inline static constexpr const char* vRoundSwitchOn = u8"\uf195";
	inline static constexpr const char* vSquareSwitchOff = u8"\uf196";
	inline static constexpr const char* vSquareSwitchOn = u8"\uf197";
	inline static constexpr const char* waveform = u8"\uf198";
	inline static constexpr const char* xlrPlug = u8"\uf199";
	inline static constexpr const char* zoomIn = u8"\uf19a";
	inline static constexpr const char* zoomOut = u8"\uf19b";

private:
	static uint8_t fontData[36812];
	inline static constexpr size_t fontSize = 36812;
};
