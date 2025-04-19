//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtLog.h"

#include "OtModelAnimation.h"
#include "OtModelUtils.h"


//
//	OtModelAnimation::load
//

void OtModelAnimation::load(const aiAnimation* animation, OtModelNodes& nodes) {
	name = animation->mName.C_Str();

	auto tps = (animation->mTicksPerSecond != 0.0) ? animation->mTicksPerSecond : 1.0;
	ticksPerSecond = static_cast<float>(tps);
	duration = static_cast<float>(animation->mDuration / tps);

	// process all channels
	channels.resize(animation->mNumChannels);

	for (auto i = 0u; i < animation->mNumChannels; i++) {
		auto& chn = animation->mChannels[i];
		auto& channel = channels.at(i);
		std::string nodeName = chn->mNodeName.C_Str();

		if (!nodes.hasNode(nodeName)) {
			OtLogError("Node [{}] used in animation [{}] is not part of model's hierarchy", nodeName, name);
		}

		channel.node = nodes.getNodeID(nodeName);

		for (auto j = 0u; j < chn->mNumPositionKeys; j++) {
			channel.positionTimestamps.push_back(static_cast<float>(chn->mPositionKeys[j].mTime / tps));
			channel.positions.push_back(toVec3(chn->mPositionKeys[j].mValue));
		}

		for (auto j = 0u; j < chn->mNumRotationKeys; j++) {
			channel.rotationTimestamps.push_back(static_cast<float>(chn->mRotationKeys[j].mTime / tps));
			channel.rotations.push_back(toQuat(chn->mRotationKeys[j].mValue));
		}

		for (auto j = 0u; j < chn->mNumScalingKeys; j++) {
			channel.scaleTimestamps.push_back(static_cast<float>(chn->mScalingKeys[j].mTime / tps));
			channel.scales.push_back(toVec3(chn->mScalingKeys[j].mValue));
		}
	}
}


//
//	OtModelAnimation::update
//

void OtModelAnimation::update(float time, OtModelNodes& nodes, size_t slot) {
	auto dt = std::fmod(time, duration);

	for (auto& channel : channels) {
		size_t element;
		float fraction;

		if (channel.positions.size() > 1 && channel.rotations.size() > 1 && channel.scales.size() > 1) {
			getTimeFraction(channel.positionTimestamps, dt, element, fraction);
			auto position1 = channel.positions[element - 1];
			auto position2 = channel.positions[element];
			auto position = glm::mix(position1, position2, fraction);

			getTimeFraction(channel.rotationTimestamps, dt, element, fraction);
			auto rotation1 = channel.rotations[element - 1];
			auto rotation2 = channel.rotations[element];
			auto rotation = glm::slerp(rotation1, rotation2, fraction);

			getTimeFraction(channel.scaleTimestamps, dt, element, fraction);
			auto scale1 = channel.scales[element - 1];
			auto scale2 = channel.scales[element];
			auto scale = glm::mix(scale1, scale2, fraction);

			nodes.setAnimationTransformParts(channel.node, slot, position, rotation, scale);
		}
	}
}


//
//	OtModelAnimation::getTimeFraction
//

void OtModelAnimation::getTimeFraction(const std::vector<float>& times, float dt, size_t& element, float& fraction) {
	auto elements = times.size();
	element = 0;

	while (element < elements && dt > times[element]) {
		element++;
	}

	if (element == elements) {
		element--;
	}

	float start = times[element - 1];
	float end = times[element];
	fraction = (dt - start) / (end - start);
}
