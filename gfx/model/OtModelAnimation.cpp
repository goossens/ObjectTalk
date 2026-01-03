//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
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
		channel.positionTimestamps.reserve(chn->mNumPositionKeys + 2);
		channel.positions.reserve(chn->mNumPositionKeys + 2);
		channel.rotationTimestamps.reserve(chn->mNumRotationKeys + 2);
		channel.rotations.reserve(chn->mNumRotationKeys + 2);
		channel.scaleTimestamps.reserve(chn->mNumScalingKeys + 2);
		channel.scales.reserve(chn->mNumScalingKeys + 2);

		for (auto j = 0u; j < chn->mNumPositionKeys; j++) {
			auto& key = chn->mPositionKeys[j];

			if (j == 0 && key.mTime > 0.0f) {
				channel.positionTimestamps.emplace_back(0.0f);
				channel.positions.emplace_back(toVec3(key.mValue));
			}

			channel.positionTimestamps.emplace_back(static_cast<float>(key.mTime / tps));
			channel.positions.emplace_back(toVec3(key.mValue));
		}

		if (channel.positions.empty()) {
			channel.positionTimestamps.emplace_back(0.0f);
			channel.positions.emplace_back(glm::vec3(0.0f));
			channel.positionTimestamps.emplace_back(duration);
			channel.positions.emplace_back(glm::vec3(0.0f));

		} else if (channel.positionTimestamps.back() < duration) {
			channel.positionTimestamps.emplace_back(duration);
			channel.positions.emplace_back(channel.positions.back());
		}

		for (auto j = 0u; j < chn->mNumRotationKeys; j++) {
			auto& key = chn->mRotationKeys[j];

			if (j == 0 && key.mTime > 0.0f) {
				channel.rotationTimestamps.emplace_back(0.0f);
				channel.rotations.emplace_back(toQuat(key.mValue));
			}

			channel.rotationTimestamps.emplace_back(static_cast<float>(key.mTime / tps));
			channel.rotations.emplace_back(toQuat(key.mValue));
		}

		if (channel.rotations.empty()) {
			channel.rotationTimestamps.emplace_back(0.0f);
			channel.rotations.emplace_back(glm::identity<glm::quat>());
			channel.rotationTimestamps.emplace_back(duration);
			channel.rotations.emplace_back(glm::identity<glm::quat>());

		} else if (channel.rotationTimestamps.back() < duration) {
			channel.rotationTimestamps.emplace_back(duration);
			channel.rotations.emplace_back(channel.rotations.back());
		}

		for (auto j = 0u; j < chn->mNumScalingKeys; j++) {
			auto& key = chn->mScalingKeys[j];

			if (j == 0 && key.mTime > 0.0f) {
				channel.scaleTimestamps.emplace_back(0.0f);
				channel.scales.emplace_back(toVec3(key.mValue));
			}

			channel.scaleTimestamps.emplace_back(static_cast<float>(key.mTime / tps));
			channel.scales.emplace_back(toVec3(key.mValue));
		}

		if (channel.scales.empty()) {
			channel.scaleTimestamps.emplace_back(0.0f);
			channel.scales.emplace_back(glm::vec3(1.0f));
			channel.scaleTimestamps.emplace_back(duration);
			channel.scales.emplace_back(glm::vec3(1.0f));

		} else if (channel.scaleTimestamps.back() < duration) {
			channel.scaleTimestamps.emplace_back(duration);
			channel.scales.emplace_back(channel.scales.back());
		}
	}
}


//
//	OtModelAnimation::update
//

void OtModelAnimation::update(float dt, OtModelNodes& nodes, size_t slot) {
	for (auto& channel : channels) {
		size_t element;
		float fraction;

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


//
//	OtModelAnimation::getTimeFraction
//

void OtModelAnimation::getTimeFraction(const std::vector<float>& times, float dt, size_t& element, float& fraction) {
	element = 0;

	while (dt > times[element]) {
		element++;
	}

	float start = times[element - 1];
	float end = times[element];
	fraction = (dt - start) / (end - start);
}
