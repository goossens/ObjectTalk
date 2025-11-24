//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#include "constants.glsl"
#include "hash.glsl"
#include "utilities.glsl"

// uniforms
layout(std140, set=1, binding=0) uniform UBO {
	mat4 viewProjectionMatrix;
	mat4 modelMatrix;

	vec4 baseColor;
	vec4 tipColor;

	float patchWidth;
	float patchDepth;
	int segments;
	int blades;

	float bladeWidth;
	float bladeHeight;
	float BladePointiness;
	float BladeCurve;

	float time;
	float windDirection;
	float windStrength;

	float widthVariation;
	float heightVariation;
	float windVariation;
	float colorVariation;
};

// vertex data
struct Vertex {
	float x;		// range -0.5 to 0.5
	float y;		// range  0.0 to 1.0
	vec4 hash1;		// range -0.5 to 0.5
	vec4 hash2;		// range -0.5 to 0.5
	vec3 pos;		// range -0.5 to 0.5
	vec3 normal;
	float side;		// front (1) or back (-1)
};

// support functions
vec3 bezier(vec3 P0, vec3 P1, vec3 P2, vec3 P3, float t) {
	return (1.0 - t) * (1.0 - t) * (1.0 - t) * P0 +
		3.0 * (1.0 - t) * (1.0 - t) * t * P1 +
		3.0 * (1.0 - t) * t * t * P2 +
		t * t * t * P3;
}

vec3 bezierGrad(vec3 P0, vec3 P1, vec3 P2, vec3 P3, float t) {
	return 3.0 * (1.0 - t) * (1.0 - t) * (P1 - P0) +
		6.0 * (1.0 - t) * t * (P2 - P1) +
		3.0 * t * t * (P3 - P2);
}

vec3 hash(vec3 p) {
	return hash33(p) * 2.0 - 1.0;
}

float noise(vec3 p) {
	vec3 i = floor(p);
	vec3 f = fract(p);
	vec3 u = f * f * (3.0 - 2.0 * f);

	return mix(mix(mix(dot(hash(i + vec3(0.0, 0.0, 0.0)), f - vec3(0.0, 0.0, 0.0)),
					   dot(hash(i + vec3(1.0, 0.0, 0.0)), f - vec3(1.0, 0.0, 0.0)), u.x),
				   mix(dot(hash(i + vec3(0.0, 1.0, 0.0)), f - vec3(0.0, 1.0, 0.0)),
					   dot(hash(i + vec3(1.0, 1.0, 0.0)), f - vec3(1.0, 1.0, 0.0)), u.x), u.y),
			   mix(mix(dot(hash(i + vec3(0.0, 0.0, 1.0)), f - vec3(0.0, 0.0, 1.0)),
					   dot(hash(i + vec3(1.0, 0.0, 1.0)), f - vec3(1.0, 0.0, 1.0)), u.x),
				   mix(dot(hash(i + vec3(0.0, 1.0, 1.0)), f - vec3(0.0, 1.0, 1.0)),
					   dot(hash(i + vec3(1.0, 1.0, 1.0)), f - vec3(1.0, 1.0, 1.0)), u.x), u.y), u.z);
}

// determine vertex data
Vertex getVertexData(vec3 pos) {
	// determine instance and vertex derived values
	Vertex vertex;
	vertex.x = pos.x;
	vertex.y = pos.y;
	vertex.hash1 = hash41(float(gl_InstanceIndex)) - 0.5;
	vertex.hash2 = hash41(blades + float(gl_InstanceIndex)) - 0.5;
	int verticesPerSide = (segments + 1) * 2;
	vertex.side = gl_VertexIndex < verticesPerSide ? 1.0 : -1.0;

	// determine blade translation
	vec3 offset = vec3(vertex.hash1.x * patchWidth, 0.0, vertex.hash1.y * patchDepth);

	// determine blade rotation
	float angle = remap(vertex.hash1.z, -0.5, 0.5, -PI, PI);

	// determine wind influence
	float adjustedWindDirection = windDirection + vertex.hash2.z * windVariation;
	vec3 windAxis = vec3(cos(adjustedWindDirection), 0.0, sin(adjustedWindDirection));
	float windTime = time * remap(windStrength, 0.0, 1.0, 0.2, 1.0);
	float windVelocity = noise(vec3(vertex.hash1.x, vertex.hash1.y, 0.0) + windTime) * windStrength;
	float windLean = windVelocity * vertex.y;
	float randomWindLean = noise(vec3(vertex.hash1.x, vertex.hash1.y, time * 4.0)) * windVelocity * 0.5;
	float leanFactor = vertex.hash1.w * BladeCurve * 2.0 + randomWindLean;

	// determine bend for grass blade
	vec3 p1 = vec3(0.0, 0.0, 0.0);
	vec3 p2 = vec3(0.0, 0.33, 0.0);
	vec3 p3 = vec3(0.0, 0.66, 0.0);
	vec3 p4 = vec3(0.0, cos(leanFactor), sin(leanFactor));
	vec3 curve = bezier(p1, p2, p3, p4, vertex.y);

	// determine size factors for grass blade
	float widthFactor = remap(vertex.hash2.x, -0.5, 0.5, 1.0 - widthVariation, 1.0);
	float heightFactor = remap(vertex.hash2.y, -0.5, 0.5, 1.0 - heightVariation, 1.0);

	// determine grass blade pointiness
	float pointiness = remap(BladePointiness, 0.0, 1.0, 15.0, 1.0);

	// create blade geometry
	vec3 localPos = vec3(
		vertex.x * bladeWidth * widthFactor * (1.0 - pow(vertex.y, pointiness)),
		vertex.y * bladeHeight * heightFactor * curve.y,
		bladeHeight * curve.z);

	// determine vertex position
	mat3 grassMat = rotateY(angle) * rotateAxis(windAxis, windLean);
	vertex.pos = grassMat * localPos + offset;

	// determine blade normal
	vec3 curveGrad = bezierGrad(p1, p2, p3, p4, vertex.y);
	mat2 curveRot90 = mat2(0.0, 1.0, -1.0, 0.0) * vertex.side;
	vertex.normal = grassMat * vec3(0.0, curveRot90 * curveGrad.yz);

	// return vertex data
	return vertex;
}
