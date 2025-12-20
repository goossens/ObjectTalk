//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#pragma once


//
//	Include files
//

#include <string>

#include "OtObject.h"
#include "OtValue.h"

#include "OtSound.h"


//
//	OtSoundObject
//

class OtSoundClass;
using OtSoundObject = OtObjectPointer<OtSoundClass>;

class OtSoundClass : public OtObjectClass {
public:
	// initialize
	void init(size_t count, OtObject* parameters);

	// load audio from file
	inline void load(const std::string& path) { sound.load(path); }

	// perform actions
	inline void play() { sound.play(); }
	inline void loop() { sound.loop(); }
	inline void pause() { sound.pause(); }
	inline void resume() { sound.resume(); }
	inline void stop() { sound.stop(); }
	inline bool isPlaying() { return sound.isPlaying(); }

	inline void setGain(float gain) { sound.setGain(gain); }
	inline float getGain() { return sound.getGain(); }

	// access raw sound
	inline OtSound& getSound() { return sound; }

	// get type definition
	static OtType getMeta();

protected:
	// constructors
	friend class OtObjectPointer<OtSoundClass>;
	OtSoundClass() = default;
	OtSoundClass(const OtSound& s) : sound(s) {}

private:
	// data
	OtSound sound;
};


//
//	Allow OtSound as an ObjectTalk value
//

template <>
struct OtValue<OtSound> {
	inline static OtObject encode(OtSound sound) {
		return OtSoundObject::create(sound);
	}

	inline static OtSound decode(OtObject object) {
		object.expect<OtSoundClass>("Sound");
		return OtSoundObject(object)->getSound();
	}
};


//
//	Helper functions
//

inline OtObject OtSoundToObject(OtSound sound) {
	return OtValue<OtSound>::encode(sound);
}

inline OtSound OtSoundFromObject(OtObject object) {
	return OtValue<OtSound>::decode(object);
}

inline bool OtSoundValidateObject(OtObject object) {
	return object.isKindOf<OtSoundClass>();
}
