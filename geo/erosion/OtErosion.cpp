#include "OtErosion.h"
//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2026 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//	Ported from https://github.com/lpmitchell/AdvancedTerrainErosion
//	Based on https://blog.runevision.com/2026/03/fast-and-gorgeous-erosion-filter.html


//
//	Include files
//

#include <algorithm>
#include <cmath>
#include <numbers>

#include "OtErosion.h"


//
//	Constants
//

static constexpr float pi = static_cast<float>(std::numbers::pi);
static constexpr float tau = pi * 2.0f;


//
//	OtErosion::sample
//

glm::vec2 OtErosion::sample(glm::vec2 position, HeightConfig& height, ErosionConfig& erosion, int seed) {
	// calculate the FBM terrain height (in the [-1, 1] range in n.x) and its derivatives (in n.yz)
	auto n = fractalNoise(position, height.frequency, height.octaves, height.lacunarity, height.gain, seed);
	n *= height.amplitude;

	// define the erosion fade target based on the altitude of the pre-eroded terrain
	// the fade target should strive to be -1 at valleys and 1 at peaks
	auto fadeTarget = std::clamp(n.x / (height.amplitude * 0.6f), -1.0f, 1.0f);

	// change terrain heights from [-1, 1] range to [0, 1] range
	n = n * 0.5f + glm::vec3(0.5f, 0.0f, 0.0f);

	// apply erosion filter and return results
	auto h = erosionFilter(position, n, fadeTarget, erosion, seed);
	auto offset = std::lerp(height.offset.x, -fadeTarget, height.offset.y) * h.magnitude;
	auto eroded = n.x + h.height + offset;
	return glm::vec2(eroded, h.ridgeMap);
}


//
//	OtErosion::erosionFilter
//

OtErosion::Erosion OtErosion::erosionFilter(const glm::vec2 position, glm::vec3 heightAndSlope, float fadeTarget, ErosionConfig& erosion, int seed) {
	auto strength = erosion.strength * erosion.scale;
	fadeTarget = std::clamp(fadeTarget, -1.0f, 1.0f);

	auto inputHeightAndSlope = heightAndSlope;
	auto freq = 1.0f / (erosion.scale * erosion.cellScale);
	auto slopeLength = std::max(glm::length(glm::vec2(heightAndSlope.y, heightAndSlope.z)), 1e-10f);
	auto magnitude = 0.0f;
	auto roundingMult = 1.0f;

	auto roundingForInput = std::lerp(erosion.rounding.y, erosion.rounding.x, std::clamp(fadeTarget + 0.5f, 0.0f, 1.0f)) * erosion.rounding.z;
	// the combined accumulating mask, based first on initial slope, and later on slope of each octave too.
	auto combiMask = easeOut(smoothStart(slopeLength * erosion.onset.x, roundingForInput * erosion.onset.x));

	// initialize the ridgeMap fadeTarget and mask
	auto ridgeMapCombiMask = easeOut(slopeLength * erosion.onset.z);
	auto ridgeMapFadeTarget = fadeTarget;

	// determine the strength of the initial slope used for gully directions
	// based on the specified mix of the actual slope and an assumed slope
	auto gullySlope = glm::mix(
		glm::vec2(heightAndSlope.y, heightAndSlope.z),
		glm::vec2(heightAndSlope.y, heightAndSlope.z) / slopeLength * erosion.assumedSlope.x,
		erosion.assumedSlope.y);

	for (auto i = 0; i < erosion.octaves; i++) {
		// calculate and add gullies to the height and slope
		auto phacelle = phacelleNoise(position * freq, glm::normalize(gullySlope), erosion.cellScale, 0.25f, erosion.normalization, seed);

		// multiply with freq since p was multiplied with freq
		// negate since we use slope directions that point down
		phacelle.z *= -freq;
		phacelle.w *= -freq;

		// amount of slope as value from 0 to 1
		auto sloping = std::abs(phacelle.y);

		// add non-masked, normalized slope to gullySlope, for use by subsequent octaves
		// it's normalized to use the steepest part of the sine wave everywhere
		gullySlope += glm::sign(phacelle.y) * glm::vec2(phacelle.z, phacelle.w) * strength * erosion.gullyWeight;

		// gullies has height offset (from -1 to 1) in x and derivative in yz
		auto gullies = glm::vec3(phacelle.x, phacelle.y * glm::vec2(phacelle.z, phacelle.w));

		// fade gullies towards fadeTarget based on combiMask.
		auto fadedGullies = glm::mix(glm::vec3(fadeTarget, 0.0f, 0.0f), gullies * erosion.gullyWeight, combiMask);

		// apply height offset and derivative (slope) according to strength of current octave
		heightAndSlope += fadedGullies * strength;
		magnitude += strength;

		// update fadeTarget to include the new octave
		fadeTarget = fadedGullies.x;

		// update the mask to include the new octave
		auto roundingForOctave = std::lerp(erosion.rounding.y, erosion.rounding.x, std::clamp(phacelle.x + 0.5f, 0.0f, 1.0f)) * roundingMult;
		auto newMask = easeOut(smoothStart(sloping * erosion.onset.y, roundingForOctave * erosion.onset.y));
		combiMask = powInv(combiMask, erosion.detail) * newMask;

		// update the ridgeMap fadeTarget and mask.
		ridgeMapFadeTarget = std::lerp(ridgeMapFadeTarget, gullies.x, ridgeMapCombiMask);
		auto newRidgeMapMask = easeOut(sloping * erosion.onset.w);
		ridgeMapCombiMask *= newRidgeMapMask;

		// prepare the next octave
		strength *= erosion.gain;
		freq *= erosion.lacunarity;
		roundingMult *= erosion.rounding.w;
	}

	auto heightAndSlopeDelta = heightAndSlope - inputHeightAndSlope;

	Erosion result;
	result.height = heightAndSlopeDelta.x;
	result.slope = glm::vec2(heightAndSlopeDelta.y, heightAndSlopeDelta.z);
	result.magnitude = magnitude;
	result.ridgeMap = ridgeMapFadeTarget * (1.0f - ridgeMapCombiMask);
	return result;
}


//
//	OtErosion::fractalNoise
//

glm::vec3 OtErosion::fractalNoise(glm::vec2 position, float frequency, int octaves, float lacunarity, float gain, int seed) {
	glm::vec3 n{0.0f};
	auto na = 1.0f;

	for (auto i = 0; i < octaves; i++) {
		auto octave = noised(seed, position * frequency);
		n.x += octave.x * na;
		n.y += octave.y * (na * frequency);
		n.z += octave.z * (na * frequency);

		na *= gain;
		frequency *= lacunarity;
	}

	// return noise (in x) and its derivatives (in yz)
	return n;
}


//
//	OtErosion::phacelleNoise
//

glm::vec4 OtErosion::phacelleNoise(glm::vec2 position, glm::vec2 normDir, float freq, float offset, float normalization, int seed) {
	// get a vector orthogonal to the input direction, with a magnitude proportional to the frequency of the stripes
	auto sideDir = glm::vec2(normDir.y, normDir.x) * glm::vec2(-1.0f, 1.0f) * freq * tau;
	offset *= tau;

	// iterate over 4x4 cells, calculating a stripe pattern for each and blending between them
	auto pInt = glm::floor(position);
	auto pFrac = glm::fract(position);
	glm::vec2 phaseDir{0.0f};
	auto weightSum = 0.0f;

	for (auto i = -1; i <= 2; i++) {
		for (auto j = -1; j <= 2; j++) {
			auto gridOffset = glm::vec2(i, j);

			// calculate a cell point by starting off with a point in the integer grid
			auto gridPoint = pInt + gridOffset;

			// calculate a random offset for the cell point between -0.5 and 0.5 on each axis
			auto randomOffset = hash2(gridPoint, seed) * 0.5f;

			// the final cell point (we don't store it) is the gridPoint plus the randomOffset
			auto vectorFromCellPoint = pFrac - gridOffset - randomOffset;

			// bell-shaped weight function which is 1 at dist 0 and nearly 0 at dist 1.5
			// due to the random offsets of up to 0.5, the closest a cell point not in the 4x4 grid
			// can be to the current point p is 1.5 units away
			auto sqrDist = glm::dot(vectorFromCellPoint, vectorFromCellPoint);
			auto weight = std::exp(-sqrDist * 2.0f);

			// subtract 0.01111 to make the function actually 0 at distance 1.5, which avoids some (very subtle) grid line artifacts
			weight = std::max(0.0f, weight - 0.01111f);

			// keep track of the total sum of weights
			weightSum += weight;

			// the waveInput is a gradient which increases in value along sideDir. Its rate of
			// change is the freq times tau, due to the multiplier pre-applied to sideDir
			auto waveInput = glm::dot(vectorFromCellPoint, sideDir) + offset;

			// add this cell's cosine and sine wave contributions to the interpolated value
			phaseDir += glm::vec2(std::cos(waveInput), std::sin(waveInput)) * weight;
		}
	}

	// Get the raw interpolated value.
	auto interpolated = phaseDir / weightSum;

	// Interpret the value as a vector whose length represents the magnitude of both waves.
	auto magnitude = std::sqrt(glm::dot(interpolated, interpolated));

	// Apply a lower threshold to show small magnitudes we're going to fully normalize.
	magnitude = std::max(1.0f - normalization, magnitude);

	// Return a vector containing the normalized cosine and sine waves, as well as the direction
	// vector, which can be multiplied onto the sine to get the derivatives of the cosine.
	return glm::vec4(interpolated / magnitude, sideDir);
}


//
//	OtErosion::noised
//

glm::vec3 OtErosion::noised(int seed, glm::vec2 position) {
	// from https://www.shadertoy.com/view/XdXBRH
	// return gradient noise (in x) and its derivatives (in yz)
	auto i = glm::floor(position);
	auto f = glm::fract(position);

	auto u = f * f * f * (f * (f * 6.0f - 15.0f) + 10.0f);
	auto du = 30.0f * f * f * (f * (f - 2.0f) + 1.0f);

	auto ga = hash2(i + glm::vec2(0.0f, 0.0f), seed);
	auto gb = hash2(i + glm::vec2(1.0f, 0.0f), seed);
	auto gc = hash2(i + glm::vec2(0.0f, 1.0f), seed);
	auto gd = hash2(i + glm::vec2(1.0f, 1.0f), seed);

	auto va = glm::dot(ga, f - glm::vec2(0.0f, 0.0f));
	auto vb = glm::dot(gb, f - glm::vec2(1.0f, 0.0f));
	auto vc = glm::dot(gc, f - glm::vec2(0.0f, 1.0f));
	auto vd = glm::dot(gd, f - glm::vec2(1.0f, 1.0f));

	auto value = va + u.x * (vb - va) + u.y * (vc - va) + u.x * u.y * (va - vb - vc + vd);

	auto derivatives =
		ga + u.x * (gb - ga) + u.y * (gc - ga) + u.x * u.y * (ga - gb - gc + gd) +
		du * (glm::vec2(u.y, u.x) * (va - vb - vc + vd) + glm::vec2(vb, vc) - va);

	return glm::vec3(value, derivatives);
}


//
//	OtErosion::hash2
//

glm::vec2 OtErosion::hash2(glm::vec2 x, int seed) {
	static glm::vec2 k{1.0f / pi , std::exp(-1.0f)};
	auto seedOffset = glm::vec2(0.06711056f, 0.00583715f) * static_cast<float>(seed);
	x = (x + seedOffset) * k + glm::vec2(k.y, k.x);
	auto t = glm::fract(x.x * x.y * (x.x + x.y));
	auto inner = 16.0f * k * t;
	return -1.0f + 2.0f * glm::fract(inner);
}


//
//	OtErosion::smoothStart
//

float OtErosion::smoothStart(float t, float smoothing) {
	return t >= smoothing ? t - 0.5f * smoothing : 0.5f * t * t / smoothing;
}


//
//	OtErosion::powInv
//

float OtErosion::powInv(float t, float power) {
	return 1.0f - std::pow(1.0f - std::clamp(t, 0.0f, 1.0f), power);
}


//
//	 OtErosion::easeOut
//

float OtErosion::easeOut(float t) {
	auto v = 1.0f - std::clamp(t, 0.0f, 1.0f);
	return 1.0f - v * v;
}
