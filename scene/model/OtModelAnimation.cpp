//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#include "OtModelAnimation.h"
#include "OtModelUtils.h"


//
//	OtModelAnimation::load
//

void OtModelAnimation::load(const aiAnimation* animation) {
	name = animation->mName.C_Str();
	ticksPerSecond = (animation->mTicksPerSecond != 0) ? static_cast<float>(animation->mTicksPerSecond) : 1.0f;
	duration = static_cast<float>(animation->mDuration * ticksPerSecond);

	// process all transformation channels
	keyframes.resize(animation->mNumChannels);

	for (auto i = 0u; i < animation->mNumChannels; i++) {
		auto channel = animation->mChannels[i];
		auto& keyframe = keyframes.at(i);
		keyframe.nodeName = channel->mNodeName.C_Str();

		for (auto j = 0u; j < channel->mNumPositionKeys; j++) {
			keyframe.positionTimestamps.push_back(channel->mPositionKeys[j].mTime);
			keyframe.positions.push_back(toVec3(channel->mPositionKeys[j].mValue));
		}

		for (auto j = 0u; j < channel->mNumRotationKeys; j++) {
			keyframe.rotationTimestamps.push_back(channel->mRotationKeys[j].mTime);
			keyframe.rotations.push_back(toQuat(channel->mRotationKeys[j].mValue));
		}

		for (auto j = 0u; j < channel->mNumScalingKeys; j++) {
			keyframe.scaleTimestamps.push_back(channel->mScalingKeys[j].mTime);
			keyframe.scales.push_back(toVec3(channel->mScalingKeys[j].mValue));
		}
	}
}
