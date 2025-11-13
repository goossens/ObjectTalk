//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


#include "constants.glsl"
#include "hash.glsl"
#include "utilities.glsl"

// uniforms
uniform vec4 u_grass[6];
#define u_patchWidth u_grass[0].x
#define u_patchDepth u_grass[0].y
#define u_segments int(u_grass[0].z)
#define u_blades int(u_grass[0].w)

#define u_bladeWidth u_grass[1].x
#define u_bladeHeight u_grass[1].y
#define u_BladePointiness u_grass[1].z
#define u_BladeCurve u_grass[1].w

#define u_time u_grass[2].x
#define u_windDirection u_grass[2].y
#define u_windStrength u_grass[2].z

#define u_widthVariation u_grass[3].x
#define u_heightVariation u_grass[3].y
#define u_windVariation u_grass[3].z
#define u_colorVariation u_grass[3].w

#define u_baseColor u_grass[4].rgb
#define u_tipColor u_grass[5].rgb

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

float hash(vec3 p) {
	return hash13(p) * 2.0 - 1.0;
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
Vertex getVertexData(int instanceID, int vertexID) {
	// determine vertex and instance derived values
	Vertex vertex;
	int verticesPerSide = (u_segments + 1) * 2;
	vertex.x = float(vertexID & 0x1) - 0.5;
	vertex.y = float((vertexID % verticesPerSide) / 2) / u_segments;
	vertex.hash1 = hash41(float(instanceID)) - 0.5;
	vertex.hash2 = hash41(u_blades + float(instanceID)) - 0.5;
	vertex.side = vertexID < verticesPerSide ? 1.0 : -1.0;

	// determine blade translation
	vec3 offset = vec3(vertex.hash1.x * u_patchWidth, 0.0, vertex.hash1.y * u_patchDepth);

	// determine blade rotation
	float angle = remap(vertex.hash1.z, -0.5, 0.5, -PI, PI);
	// angle = 0.0;

	// determine wind influence
	float windDirection = u_windDirection + vertex.hash2.z * u_windVariation;
	vec3 windAxis = vec3(cos(windDirection), 0.0, sin(windDirection));
	float windTime = u_time * remap(u_windStrength, 0.0, 1.0, 0.2, 1.0);
	float windVelocity = noise(vec3(vertex.hash1.x, vertex.hash1.y, 0.0) + windTime) * u_windStrength;
	float windLean = windVelocity * vertex.y;
	float randomWindLean = noise(vec3(vertex.hash1.x, vertex.hash1.y, u_time * 4.0)) * windVelocity * 0.5;
	float leanFactor = vertex.hash1.w * u_BladeCurve * 2.0 + randomWindLean;

	// determine bend for grass blade
	vec3 p1 = vec3(0.0, 0.0, 0.0);
	vec3 p2 = vec3(0.0, 0.33, 0.0);
	vec3 p3 = vec3(0.0, 0.66, 0.0);
	vec3 p4 = vec3(0.0, cos(leanFactor), sin(leanFactor));
	vec3 curve = bezier(p1, p2, p3, p4, vertex.y);

	// determine size factors for grass blade
	float widthFactor = remap(vertex.hash2.x, -0.5, 0.5, 1.0 - u_widthVariation, 1.0);
	float heightFactor = remap(vertex.hash2.y, -0.5, 0.5, 1.0 - u_heightVariation, 1.0);

	// determine grass blade pointiness
	float pointiness = remap(u_BladePointiness, 0.0, 1.0, 15.0, 1.0);

	// create blade geometry
	vec3 localPos = vec3(
		vertex.x * u_bladeWidth * widthFactor * (1.0 - pow(vertex.y, pointiness)),
		vertex.y * u_bladeHeight * heightFactor * curve.y,
		u_bladeHeight * curve.z);

	// determine vertex position
	mat3 grassMat = rotateY(angle) * rotateAxis(windAxis, windLean);
	vertex.pos = mul(grassMat, localPos) + offset;

	// determine blade normal
	vec3 curveGrad = bezierGrad(p1, p2, p3, p4, vertex.y);
	mat2 curveRot90 = mat2(0.0, 1.0, -1.0, 0.0) * vertex.side;
	vertex.normal = mul(grassMat, vec3(0.0, mul(curveRot90, curveGrad.yz)));

	// return vertex data
	return vertex;
}
