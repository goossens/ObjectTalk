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

	// update animation based on current time
	void update(float time, OtModelNodes& nodes, size_t slot);

private:
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
