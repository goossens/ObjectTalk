//	ObjectTalk Scripting Language
//	Copyright(c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <cstdint>
#include <vector>


//
//	OtMidiMessage
//

class OtMidiMessage : public std::vector<uint8_t> {
public:
	// constructors
	OtMidiMessage() = default;
	OtMidiMessage(int command) : std::vector<uint8_t>(1, static_cast<uint8_t>(command)) {}

	OtMidiMessage(int command, int p1) : std::vector<uint8_t>(2) {
		(*this)[0] = static_cast<uint8_t>(command);
		(*this)[1] = static_cast<uint8_t>(p1);
	}

	OtMidiMessage(int command, int p1, int p2) : vector<uint8_t>(3) {
		(*this)[0] = static_cast<uint8_t>(command);
		(*this)[1] = static_cast<uint8_t>(p1);
		(*this)[2] = static_cast<uint8_t>(p2);
	}

	OtMidiMessage(const OtMidiMessage& message) : vector<uint8_t>() {
		(*this) = message;
	}

	OtMidiMessage(const std::vector<uint8_t>& message) : vector<uint8_t>() {
		setMessage(message);
	}

	OtMidiMessage(uint8_t* message, size_t size) : vector<uint8_t>() {
		setMessage(message, size);
	}

	// assignment operator
	inline OtMidiMessage& operator=(const OtMidiMessage& message) {
		std::vector<uint8_t>::operator=(static_cast<const std::vector<uint8_t> &>(message));
		return *this;
	}

	// initialize message
	inline void setMessage(const std::vector<uint8_t>& message) {
		resize(message.size());

		for (size_t i = 0; i < size(); i++) {
			(*this)[i] = message[i];
		}
	}

	void setMessage(uint8_t* message, size_t size) {
		resize(size);

		for (size_t i = 0; i < size; i++) {
			(*this)[i] = *message++;
		}
	}

	// note-message convenience functions
	inline int getChannel() { return (*this)[0] & 0x0f; }
	inline int getKeyNumber() { return (isNote() || isAfterTouch()) ? static_cast<int>((*this)[1]) : -1; }
	inline int getVelocity() { return isNote() ? static_cast<int>((*this)[2]) : -1; }

	// controller-message convenience functions
	inline int getControlType() { return isController() ? static_cast<int>((*this)[1]) : -1; }
	inline int getControlValue() { return isController() ? static_cast<int>((*this)[2]) : -1; }

	// message-type convenience functions
	inline bool isNote() { return isNoteOn() || isNoteOff(); }
	inline bool isNoteOn() { return ((*this)[0] & 0xF0) == 0x90; }
	inline bool isNoteOff() { return ((*this)[0] & 0xF0) == 0x80; }
	inline bool isAfterTouch() { return ((*this)[0] & 0xF0) == 0xA0; }
	inline bool isController() { return ((*this)[0] & 0xF0) == 0xB0; }
	inline bool isPatchChange() { return ((*this)[0] & 0xF0) == 0xC0; }
	inline bool isPressure() { return ((*this)[0] & 0xF0) == 0xD0; }
	inline bool isPitchBend() { return ((*this)[0] & 0xF0) == 0xE0; }
	inline bool isMeta() { return (*this)[0] == 0xFF; }

	inline bool isSustain() { return isController() && (*this)[1] == 64; }
	inline bool isSustainOn() { return isSustain() && static_cast<int>((*this)[2]) >= 64; }
	inline bool isSustainOff() { return isSustain() && static_cast<int>((*this)[2]) < 64; }
	inline bool isSoft() { return isController() && (*this)[1] == 67; }
	inline bool isSoftOn() { return isSoft() && static_cast<int>((*this)[2]) >= 64; }
	inline bool isSoftOff() { return isSoft() && static_cast<int>((*this)[2]) < 64; }
	inline bool isAllSoundsOff() { return isController() && (*this)[1] == 120; }
	inline bool isAllNotesOff() { return isController() && (*this)[1] == 123; }

	// helper functions to create various MidiMessages:
	inline void makeNoteOn(int channel, int key, int velocity) {
		resize(3);
		(*this)[0] = static_cast<uint8_t>(0x90 | (0x0F & channel));
		(*this)[1] = static_cast<uint8_t>(key & 0x7F);
		(*this)[2] = static_cast<uint8_t>(velocity & 0x7F);
	}

	inline void makeNoteOff(int channel, int key, int velocity)  {
		resize(3);
		(*this)[0] = static_cast<uint8_t>(0x80 | (0x0F & channel));
		(*this)[1] = static_cast<uint8_t>(key & 0x7F);
		(*this)[2] = static_cast<uint8_t>(velocity & 0x7F);
	}

	inline void makeControlChangeMessage(int channel, int type, int value) {
		resize(3);
		(*this)[0] = static_cast<uint8_t>(0xB0 | (0x0F & channel));
		(*this)[1] = static_cast<uint8_t>(type & 0x7F);
		(*this)[2] = static_cast<uint8_t>(value & 0x7F);
	}

	inline void makeAllSoundsOff(int channel) {
		makeControlChangeMessage(channel, 120, 0);
	}

	inline void makeAllNotesOff(int channel) {
		makeControlChangeMessage(channel, 123, 0);
	}

	inline void makePitchBend(int channel, int value) {
		resize(3);
		int lsb = value & 0x7F;
		int msb = (value >> 7) & 0x7F;
		(*this)[0] = static_cast<uint8_t>(0xE0 | (0x0F & channel));
		(*this)[1] = static_cast<uint8_t>(0x7F & lsb);
		(*this)[2] = static_cast<uint8_t>(0xF & msb);
	}
};
