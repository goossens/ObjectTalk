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
#include <vector>

#include "assimp/scene.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include "OtModelMesh.h"


//
//	OtModelAnimation
//

class OtModelAnimation {
public:
	// load the animation
	void load(const aiAnimation* animation, OtModelNodes& nodes);

	// update animation based on animation time (0.0 - duration)
	void update(float dt, OtModelNodes& nodes, size_t slot);

	// access properties
	inline std::string getName() { return name; }
	inline float getDuration() { return duration; }

private:
	friend class OtModel;

	// properties
	std::string name;
	float duration;
	float ticksPerSecond;

	class Channel {
	public:
		size_t node;

		std::vector<float> positionTimestamps;
		std::vector<float> rotationTimestamps;
		std::vector<float> scaleTimestamps;

		std::vector<glm::vec3> positions;
		std::vector<glm::quat> rotations;
		std::vector<glm::vec3> scales;
	};

	std::vector<Channel> channels;

	// support functions
	void getTimeFraction(const std::vector<float>& times, float dt, size_t& element, float& fraction);
};
